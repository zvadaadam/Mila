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
        Constant * varInteger = ConstantInt::get(Type::getInt32Ty(*context), var->value, true);
        GlobalVariable * globalVar;
        if (var->type == CONST) {
            globalVar = new GlobalVariable(*module, Type::getInt32Ty(*context), true, GlobalValue::ExternalLinkage, 0, var->ident);
        } else if (var->type == VAR) {
            globalVar = new GlobalVariable(*module, Type::getInt32Ty(*context), false, GlobalValue::ExternalLinkage, 0, var->ident);
        } else  {
            //UNDEF type
            continue;
        }
        globalVar->setAlignment(4);
        globalVar->setInitializer(varInteger);
        namedValues[var->ident] = module->getGlobalVariable(var->ident);

        //ConstantInt * intVal = dyn_cast<ConstantInt>(module->getGlobalVariable(var->ident)->getInitializer());
        //cout << intVal->getSExtValue() << endl;
    }
}

//TODO LOCAL VAR

//-------------------------------CODEGEN-------------------------------------------------


Value * Var::GenerateIR() {
    cout << "Creating variable value: " << _name << " " << _value << endl;

    Value * var = namedValues[_name];

    if (!var) {
        std::cout << "Variable with idenifier \"" << _name << "was not decleared." << std::endl;
    }

    if (_rvalue) {
        return builder->CreateLoad(var, _name.c_str());
    }

    return var;
}

Value * Numb::GenerateIR() {
    std::cout << "Creating integer value: " << _value << std::endl;

    return ConstantInt::get(Type::getInt32Ty(*context), _value, true);
}

Value * BinOp::GenerateIR() {
    std::cout << "Performing binary operation " << _op << std::endl;

    llvm::Value * left = _left->GenerateIR();
    llvm::Value * right = _right->GenerateIR();

    if (!left || !right) {
        std::cout << "Error on left or right operand" << std::endl;
        return nullptr;
    }

    //TODO, add other operations, LESS, LESS_OR_EQ, GRATHER, GRATHER_OR_EQ, EQ, NOT_EQ,
    switch (_op) {
        case PLUS:
            return builder->CreateAdd(left, right, "addtmp");
        case MINUS:
            return builder->CreateSub(left, right, "subtmp");
        case MULTIPLY:
            return builder->CreateMul(left, right, "multmp");
        case DIVIDE:
            return builder->CreateSDiv(left, right, "divtmp");
        case LESS:
            return builder->CreateICmpSLT(left, right, ".lttmp");
        case LESS_OR_EQ:
            return builder->CreateICmpSLE(left, right, ".ltetmp");
        case GRATHER:
            return builder->CreateICmpSGT(left, right, ".gttmp");
        case GRATHER_OR_EQ:
            return builder->CreateICmpSGE(left, right, ".gtetmp");
        case EQ:
            return builder->CreateICmpEQ(left, right, ".eqtmp");
        case NOT_EQ:
            return builder->CreateICmpNE(left, right, ".neqtmp");
        case kwOR:
            return builder->CreateOr(left, right, ".ortmp");
        case kwAND:
            return builder->CreateAnd(left, right, ".andtmp");
        case kwMOD:
            return builder->CreateSRem(left, right, ".modtmp");
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

    StatmList * curStatementList = this;

    while(curStatementList->_next) {
        curStatementList->_statement->GenerateIR();
        curStatementList = curStatementList->_next;
    }
    if(curStatementList->_statement) {
        curStatementList->_statement->GenerateIR();
    }


    return Constant::getNullValue(Type::getInt32Ty(*context));
}

Value * Assign::GenerateIR() {

    int * value;
    string ident = _var->GetName();
    SymboleType type = symboleTable->GetConstOrVar(ident, value);
    if (type == CONST) {
        cout << "Cannot assign to CONST variable" << endl;
    }

    Value * expr = _expr->GenerateIR();
    Value * var = _var->GenerateIR();

    return builder->CreateStore(expr, var);
}

Value * Read::GenerateIR() {

    Value * var = _var->GenerateIR();

    vector<Value*> scanVal;
    scanVal.push_back(builder->CreateGlobalStringPtr("%d"));
    scanVal.push_back(var);

    return builder->CreateCall(scanfFunc(), scanVal, "scanfCall");
}

Value * Write::GenerateIR() {

    Value * expr = _expression->GenerateIR();

    vector<Value*> printVal;
    printVal.push_back(builder->CreateGlobalStringPtr("%d\n"));
    printVal.push_back(expr);

    return builder->CreateCall(printFunc(), printVal, "printfCall");
}

Value * If::GenerateIR() {

    Value * condition = _condition->GenerateIR();
    if (!condition) {
        return nullptr;
    }

    Function * mainFunc = builder->GetInsertBlock()->getParent();

    BasicBlock * thenBlock = llvm::BasicBlock::Create(*context, "then", mainFunc);
    BasicBlock * elseBlock = llvm::BasicBlock::Create(*context, "else");
    BasicBlock * conditionBlock = llvm::BasicBlock::Create(*context, "ifcont");

    builder->CreateCondBr(condition, thenBlock, elseBlock);

    builder->SetInsertPoint(thenBlock);
    Value * thenVal = _then->GenerateIR();
    if (!thenVal) {
        return nullptr;
    }

    builder->CreateBr(conditionBlock);

    thenBlock = builder->GetInsertBlock();

    mainFunc->getBasicBlockList().push_back(elseBlock);
    builder->SetInsertPoint(elseBlock);

    Value * elseVal = _else->GenerateIR();
    if (!elseVal) {
        return nullptr;
    }

    builder->CreateBr(conditionBlock);

    elseBlock = builder->GetInsertBlock();

    mainFunc->getBasicBlockList().push_back(conditionBlock);
    builder->SetInsertPoint(conditionBlock);

    return conditionBlock;
}

Value * While::GenerateIR() {





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
    vector<Type *> args;
    args.push_back(Type::getInt8PtrTy(*context));
    FunctionType *printfType = FunctionType::get(builder->getInt32Ty(), args, true);
    return module->getOrInsertFunction("printf", printfType);

    //return module->getOrInsertFunction("printf", FunctionType::get(IntegerType::getInt32Ty(*context), PointerType::get(Type::getInt8Ty(*context), 0), true));
}

Constant * Read::scanfFunc() {
    std::vector<Type *> args;
    args.push_back(Type::getInt8PtrTy(*context));
    FunctionType *scanfType = FunctionType::get(builder->getInt32Ty(), args, true);
    return module->getOrInsertFunction("scanf", scanfType);
}

