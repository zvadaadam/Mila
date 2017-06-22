#include <iostream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

#include "parser.h"
#include "ast.h"

using namespace llvm;
using namespace std;


// Very much inspired on "http://llvm.org/docs/tutorial"
int createObject(Module *module) {

    // Initialize the target registry etc.


    InitializeNativeTarget();
    InitializeNativeTargetAsmParser();
    InitializeNativeTargetAsmPrinter();

    auto TargetTriple = sys::getDefaultTargetTriple();
    module->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        errs() << Error;
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module->setDataLayout(TheTargetMachine->createDataLayout());
    module->setTargetTriple(TargetTriple);

    auto Filename = "output.o";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::F_None);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }

    //Print generated code
    module->dump();

    legacy::PassManager pass;
    auto FileType = TargetMachine::CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
        errs() << "TheTargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*module);
    dest.flush();

    outs() << "Wrote " << Filename << "\n";

    return 0;
}

int main() {

    //string fileName = "../examples/global.p";
    //string fileName = "../examples/readVar.p";
    //string fileName = "../examples/if.p";
    //string fileName = "../examples/while.p";
    //string fileName = "../examples/whileBreak.p";
    //string fileName = "../examples/forCycle.p";
    //string fileName = "../examples/for.p";
    string fileName = "../examples/arrayTest.p";


    //Important objects!
    LLVMContext context;
    IRBuilder<> builder(context);
    Module *module = new Module("mainTest", context);
    map<string, Value *> namedValues;
    BasicBlock * breakTarget;

    // create -> MAIN()

    Type *returnType = Type::getInt32Ty(context);
    //vector<Type*> argTypes;
    FunctionType *mainFunctionType = FunctionType::get(returnType, false);
    // ExternalLinkage - accessible through the whole program
    Function *mainFunction = Function::Create(mainFunctionType, Function::ExternalLinkage, "main", module);

    BasicBlock *mainBlock = BasicBlock::Create(context, "entry", mainFunction);
    builder.SetInsertPoint(mainBlock);

    breakTarget = mainBlock;

    Parser *parser = new Parser(fileName, context, module, builder, breakTarget);

    Prog *program = parser->Program();

    program->GenerateIR();


    //Set Return value
    builder.CreateRet(ConstantInt::get(builder.getInt32Ty(), 0));

    createObject(module);

    delete module;

    llvm_shutdown();

    system("g++ output.o");

    return 0;
}

