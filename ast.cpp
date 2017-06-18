//
// Created by Adam Zvada on 17.06.17.
//

#include "ast.h"


//--------------------------------------------------------------------------------


// core LLVM data struct
static LLVMContext * context;

// helper object for generating LLVM instructions
static IRBuilder<> * builder;

// LLVM construct that contains function and global variables
static Module * module;

// symtable of code
static std::map<std::string, Value *> namedValues;
static SymboleTable * symboleTable;

//--------------------------------------------------------------------------------

void llvmAstInit(LLVMContext & theContext, Module * theModule, IRBuilder<> & theBuilder, SymboleTable * symbTable) {
    context = &theContext;
    module = theModule;
    builder = &theBuilder;
    symboleTable = symbTable;
}

void SetGlobalVariables() {

    for (auto var : symboleTable->GetAllGlobalVar()) {
        Constant * varInteger = ConstantInt::get(Type::getInt64Ty(*context), var->value, true);
        GlobalVariable * globalVar;
        if (var->type == CONST) {
            globalVar = new GlobalVariable(*module, Type::getInt64Ty(*context), true, GlobalValue::CommonLinkage, 0, var->ident);
        } else if (var->type == VAR) {
            globalVar = new GlobalVariable(*module, Type::getInt64Ty(*context), false, GlobalValue::CommonLinkage, 0, var->ident);
        } else  {
            //UNDEF type
            continue;
        }
        globalVar->setAlignment(4);
        globalVar->setInitializer(varInteger);
        namedValues[var->ident] = module->getGlobalVariable(var->ident);
    }
}


//TODO LOCAL VAR

//-------------------------------CODEGEN-------------------------------------------------


Value * Var::GenerateIR() {
    cout << "Creating variable value" << endl;

    llvm::Value * var = namedValues[_name];

    if (!var) {
        std::cout << "Variable with idenifier \"" << _name <<  "was not decleared." << std::endl;
    }

    return var;
}

Value * Numb::GenerateIR() {
    std::cout << "Creating integer value" << std::endl;

    return ConstantInt::get(Type::getInt64Ty(*context), _value, true);
}

Value * BinOp::GenerateIR() {
    std::cout << "Performing binary operation " << _op << std::endl;

    llvm::Value * left = _left->GenerateIR();
    llvm::Value * right = _right->GenerateIR();

    if (!left || !right) {
        std::cout << "Error on left or right operand" << std::endl;
        return nullptr;
    }

    //TODO, add other operations
    switch (_op) {
        case PLUS:
            return builder->CreateAdd(left, right, "addtmp");
        case MINUS:
            return builder->CreateSub(left, right, "subtmp");
        case MULTIPLY:
            return builder->CreateMul(left, right, "multmp");
        case DIVIDE:
            return builder->CreateSDiv(left, right, "divtmp");
        default:
            std::cout << "Unsupported OP, should not be evaluated" << std::endl;
            return nullptr;
    }
}

Value * UnaryMinus::GenerateIR() {
    std::cout << "Creating negative value" << std::endl;
    Value * expr = _expression->GenerateIR();
    if (!expr) {
        std::cout << "Error on expression for unanry minus" << std::endl;
    }
    return builder->CreateNeg(_expression->GenerateIR(), "minus");
}

Value * StatmList::GenerateIR() {

    if (!_statement) {
        cout << "No execuate block" << endl;
        return nullptr;
    }

    do {
        _statement->GenerateIR();
    } while(!_next);

    return nullptr;
}

Value * Assign::GenerateIR() {
    //TODO
    return nullptr;
}

Value * Write::GenerateIR() {

    Value * expr = _expression->GenerateIR();

    vector<Value*> printVal;
    printVal.push_back(builder->CreateGlobalStringPtr("value = %d\n"));
    printVal.push_back(expr);

    return builder->CreateCall(printFunc(), printVal);
}

Value * If::GenerateIR() {
    //TODO
    return nullptr;
}

Value * While::GenerateIR() {
    //TODO
    return nullptr;
}

Value * For::GenerateIR() {
    //TODO
    return nullptr;
}

Value * Prog::GenerateIR() {

    SetGlobalVariables();

    return _statmentList->GenerateIR();
}


//--------------------------------------------------------------------------------

Constant * Write::printFunc() {
    std::vector<Type *> args;
    args.push_back(Type::getInt8PtrTy(*context));
    FunctionType *printfType = FunctionType::get(builder->getInt32Ty(), args, true);
    return module->getOrInsertFunction("printf", printfType);
}
