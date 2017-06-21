//
// Created by Adam Zvada on 17.06.17.
//

#ifndef TESTLLVM_PARSER_H
#define TESTLLVM_PARSER_H


#include <iostream>

#include "lexan.h"
#include "ast.h"
#include "tabsym.h"

#define DEBUG_MODE 1

using namespace std;


class ParserException {
public:
    ParserException(string what) {
        cout << what << endl;
        exit(1);
    }
};


class Parser {
public:
    Parser(string filename, llvm::LLVMContext & context, llvm::Module * module, llvm::IRBuilder<> & builder, BasicBlock * breakTarget);


    void Compare(LexSymbolType symboleType);
    void CompareIndent(string & ident);
    void CompareNumb(int & num);

    void ReadToken();

    void PrintSymb(LexicalSymbol lexSymbol);


    Prog * Program();


    void Decleration();

    void DeclerationVar();
    void DeclerationVarNext();

    void DeclerationConst();
    void DeclerationConstNext();

    void DeclerationFunc();


    StatmList * ProgramBlock();

    Statm * Statement();
    StatmList * StatementNext();

    Statm * AssigmentStat();
    Statm * WriteStat();
    Statm * ReadStat();
    Statm * IfStat();
    Statm * ForStat();
    Statm * WhileStat();
    Statm * SwitchStat();
    Statm * BreakStat();

    Expr * Condition();
    Statm * Else();

    LexSymbolType Operator();

    Expr * Expression();
    Expr * ExpressionNext(Expr * inheretedExp);

    Expr * Term();
    Expr * TermNext(Expr * inheretedExp);

    Expr * VarOrConst(string & id);
    Expr * Factor();
private:
    LexicalSymbol _symbole;
    SymboleTable * _symTable;
};



#endif //TESTLLVM_PARSER_H
