//
// Created by Adam Zvada on 17.06.17.
//

#include "parser.h"
#include <sstream>
#include <cstdio>
#include <cstdlib>

#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <fstream>


void Parser::PrintSymb(LexicalSymbol lexSymbol) {
    printf("<%s", symbTable[lexSymbol.type]);
    switch (lexSymbol.type) {
        case IDENT:
            printf(", %s", lexSymbol.ident);
            break;
        case NUMB:
            printf(", %d", lexSymbol.number);
            break;
        default:
            break;
    }
    printf(">\n");
}


Parser::Parser(string filename, llvm::LLVMContext & context, llvm::Module * module, llvm::IRBuilder<> & builder, BasicBlock * breakTarget) {
    _symTable = new SymboleTable();

    // init static variables for LLVM and pass
    llvmAstInit(context, module, builder, breakTarget, _symTable);

    if (!initLexan(filename.c_str())) {
        throw ParserException("Lexer had problem with init.");
    }
    this->_symbole = readLexem();
}



void Parser::Compare(LexSymbolType symboleType) {
    if (_symbole.type == symboleType) {
        ReadToken();
    } else {
        ostringstream os;
        os << "Error with COMPARE of my symbole type " << (char)_symbole.type << " and given type " << (char)symboleType << endl;
        ParserException(os.str());
    }
}

void Parser::CompareIndent(string & ident) {
    if (_symbole.type == IDENT) {
        ident = string(_symbole.ident);
        ReadToken();
    } else {
        ostringstream os;
        os << "Error with COMPARE IDENT where my symbole type is " << (char)_symbole.type << endl;
        ParserException(os.str());
    }
}


void Parser::CompareNumb(int & num) {
    if (_symbole.type == NUMB) {
        num = _symbole.number;
        ReadToken();
        cout << _symbole.type << endl;
    } else {
        ostringstream os;
        os << "Error with COMPARE NUM where my symbole type is " << (char)_symbole.type << endl;
        ParserException(os.str());
    }
}

void Parser::ReadToken() {
    _symbole = readLexem();
    PrintSymb(_symbole);
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------


// <program> -> 'kwPROGRAM' 'IDENT' 'SEMICOLON' <decleration> <block> 'PERIOD'
Prog * Parser::Program() {


    Compare(kwPROGRAM);
    string programIdent;
    CompareIndent(programIdent);
    Compare(SEMICOLON);

    Decleration();

    StatmList * statmentList = ProgramBlock();

    Compare(DOT);

    return new Prog(programIdent, statmentList);
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

// <decleration> -> kwCONST <constantDeleration> <decleration> |  kwVAR <variableDecleration> <decleration> | kwFUNC <functionsDecleration> <decleration> | EPS
void Parser::Decleration() {

    switch (_symbole.type) {
        case kwVAR:
            ReadToken();
            DeclerationVar();
            Decleration();
            break;
        case kwCONST:
            ReadToken();
            DeclerationConst();
            Decleration();
            break;
        case kwFUNC:
            ReadToken();
            DeclerationFunc();
            Decleration();
            break;
        default:
            //EPS
            break;
    }

}

// <variableDecleration> -> IDENT <nextVariableDecleration> SEMICOLON | IDENT COLON INTEGER SEMICOLON | IDENT COLON ARRAY LBRA <numbarr> DOT DOT <numbarr> RBRA SEMICOLON
void Parser::DeclerationVar() {

    string varIdent = "";
    CompareIndent(varIdent);

    //_symTable->DeclareVar(varIdent);

    if (_symbole.type == COLON) {
        ReadToken();
        if (_symbole.type == kwARRAY) {
            ReadToken();
            Compare(LBRA);

            Numb * start = NumbArr();

            Compare(DOT);
            Compare(DOT);

            Numb * end = NumbArr();

            Compare(RBRA);

            _symTable->DeclareVarArr(varIdent, start->GetNumb(), end->GetNumb());

        } else if (_symbole.type == kwINTEGER) {
            ReadToken();
            _symTable->DeclareVar(varIdent);
        }
    } else {
        _symTable->DeclareVar(varIdent);
        DeclerationVarNext();
    }

    Compare(SEMICOLON);
}


// <nextVariableDecleration> -> COMMA IDENT <nextVariableDecleration> | EPS
void Parser::DeclerationVarNext() {
    if (_symbole.type == COMMA) {
        ReadToken();

        string varIdent;
        CompareIndent(varIdent);
        _symTable->DeclareVar(varIdent);

        DeclerationVarNext();
    }
}


// <constatntDecleration> -> IDENT ASSIGN NUMB <nextConstantDecleration> SEMICOLON
void Parser::DeclerationConst() {
    string constIdent;
    int value;

    CompareIndent(constIdent);
    Compare(ASSIGN);
    CompareNumb(value);

    _symTable->DeclareConst(constIdent, value);

    DeclerationConstNext();
    Compare(SEMICOLON);

}

// <nextConstantDecleration> -> COMMA IDENT ASSIGN NUMB <nextConstDecleration> | EPS
void Parser::DeclerationConstNext() {
    if (_symbole.type == COMMA) {
        ReadToken();

        string constIdent;
        int value;

        CompareIndent(constIdent);
        Compare(ASSIGN);
        CompareNumb(value);

        _symTable->DeclareConst(constIdent, value);

        DeclerationConstNext();
    }
}


void Parser::DeclerationFunc() {

    //TODO

    cout << "FUNCTION Not implemented yet" << endl;
    exit(1);
}


// <programBlock> -> 'kwBEGIN' <statement> <statementNext> 'kwEND'
StatmList * Parser::ProgramBlock() {
    Compare(kwBEGIN);

    StatmList * listStatm = new StatmList(Statement(), StatementNext());

    Compare(kwEND);

    return listStatm;
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------


// <statement> -> <assignment> | <write> | <if> | <for> | <while> | <switch> | <break> | EPS
Statm * Parser::Statement() {

    switch (_symbole.type) {
        case IDENT:
            return AssigmentStat();
        case kwWRITE:
            return WriteStat();
        case kwREAD:
            return ReadStat();
        case kwIF:
            return IfStat();
        case kwFOR:
            return ForStat();
        case kwWHILE:
            return WhileStat();
        case kwSWITCH:
            return SwitchStat();
        case kwBREAK:
            return BreakStat();
        default:
            break;
    }

    return nullptr;
}

// <statementNext> -> kwSEMICOLON <statement> <statementNext> | EPS
StatmList * Parser::StatementNext() {
    if(_symbole.type == SEMICOLON) {
        ReadToken();
        return new StatmList(Statement(), StatementNext());
    }
    return nullptr;
}

// <assigment> -> IDENT ASSIGN EXPR | IDENT LBRA <expression> RBRA ASSIGN EXPR
Statm * Parser::AssigmentStat() {
    string ident;
    CompareIndent(ident);

    Var * var;
    if (_symbole.type == LBRA) {
        ReadToken();
        Expr * index = Expression();
        Compare(RBRA);

        var = new VarArray(ident, index);
    } else {
        var = new Var(_symTable->GetValue(ident), ident);
    }

    Compare(ASSIGN);
    Expr * expression = Expression();

    return new Assign(var, expression);
}

// <write> -> kwWRITE <expr>
Statm * Parser::WriteStat() {
    Compare(kwWRITE);

    Expr * expression = Expression();

    Statm * statement = new Write(expression);

    return statement;
}

// <read> -> kwREAD IDENT
Statm * Parser::ReadStat() {
    Compare(kwREAD);

    string ident;
    CompareIndent(ident);
    Var * var = new Var(_symTable->GetValue(ident), ident);

    Statm * statement = new Read(var);

    return statement;
}

// <if> -> kwIF <condition> kwTHEN <statement> <elseStatement> | kwIF <condition> kwTHEN <programBlock> <elseStatement>
Statm * Parser::IfStat() {
    Compare(kwIF);
    Expr * expression = Condition();
    Compare(kwTHEN);

    if (_symbole.type == kwBEGIN) {
        return new If(expression, ProgramBlock(), Else());
    } else {
        return new If(expression, Statement(), Else());
    }
}


/**
 * For loop created by using while loop
 */
// TODO, accept for i to 20 do ... without int of var
// <for> -> kwFOR <assigment> kwTO <expression> kwDO <programBlock> | kwFOR <assigment> kwTO <expression> kwDO <statement>
Statm * Parser::ForStat() {
    bool isAscendning;

    Compare(kwFOR);
    Statm * init = AssigmentStat();
    if (_symbole.type == kwTO) {
        isAscendning = true;
    } else if (_symbole.type == kwDOWNTO) {
        isAscendning = false;
    } else {
        cout << "Error, expects either 'TO' or 'DOWNTO'." << endl;
        exit(1);
    }
    ReadToken();

    Expr * endExpression = Expression();

    Compare(kwDO);

    Var * var = dynamic_cast<Assign*>(init)->GetVar();

    LexSymbolType ascOrDesc = isAscendning ? PLUS : MINUS;

    Expr * condition = new BinOp(NOT_EQ, new Var(var->GetName(), true), endExpression);

    Statm * updateValue = new Assign(new Var(var->GetName(), false), new BinOp(ascOrDesc, new Var(var->GetName(), true), new Numb(1)));

    StatmList * body;
    if (_symbole.type == kwBEGIN) {
        body = new StatmList(updateValue, ProgramBlock());
    } else {
        body = new StatmList(updateValue, new StatmList(Statement(), nullptr));
    }


    Statm * whileBlock = new While(condition, body);

    StatmList * forStatement = new StatmList(init, new StatmList(whileBlock, nullptr));

    return forStatement;
}

// <while> -> kwWHILE <condition> kwDO <statement> | kwWHILE <condition> kwDO <programBlock>
Statm * Parser::WhileStat() {
    Compare(kwWHILE);
    Expr * condition = Condition();
    Compare(kwDO);

    if (_symbole.type == kwBEGIN) {
        return new While(condition, ProgramBlock());
    } else {
        return new While(condition, Statement());
    }
}

// <switch> ->
Statm * Parser::SwitchStat() {
    //TODO
    return nullptr;
}

// <break> -> kwBREAK
Statm * Parser::BreakStat() {
    ReadToken();

    return new Break();
}


// <condition> -> <expression> <operator> <expression>
Expr * Parser::Condition() {

    Expr * left = Expression();
    LexSymbolType op = Operator();
    Expr * right = Expression();

    return new BinOp(op, left, right);
}

// <elseStatement> -> kwELSE <statement> | kwELSE <programBlock> | EPS
Statm * Parser::Else() {
    Statm * statement = nullptr;
    if (_symbole.type == kwELSE) {
        ReadToken();
        if (_symbole.type == kwBEGIN) {
            statement = ProgramBlock();
        } else {
            statement = Statement();
        }
    }

    return statement;
}

// <operator> -> EQ | NOT_EQ | LESS | GRATHER | LESS_OR_EQ | GRATHER_OR_EQ | OR | AND | MOD
LexSymbolType Parser::Operator() {
    switch (_symbole.type) {
        case EQ:
            ReadToken();
            return LexSymbolType::EQ;
        case NOT_EQ:
            ReadToken();
            return LexSymbolType::NOT_EQ;
        case LESS:
            ReadToken();
            return LexSymbolType::LESS;
        case GRATHER:
            ReadToken();
            return LexSymbolType::GRATHER;
        case LESS_OR_EQ:
            ReadToken();
            return LexSymbolType::LESS_OR_EQ;
        case GRATHER_OR_EQ:
            ReadToken();
            return LexSymbolType::GRATHER_OR_EQ;
        case kwOR:
            ReadToken();
            return LexSymbolType::kwOR;
        case kwAND:
            ReadToken();
            return LexSymbolType::kwAND;
        case kwMOD:
            ReadToken();
            return LexSymbolType::kwMOD;
        default:
            throw ParserException("Unsupported Operator");
            //exit(1);
    }
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

// <expression> -> MINUS <term> <expressionNext> | <term> <expressionNext>
Expr * Parser::Expression() {
    if (_symbole.type == MINUS) {
        ReadToken();

        return ExpressionNext(new UnaryMinus(Term()));
    }

    return ExpressionNext(Term());
}

// <expressionNext> -> PLUS <term> <expressionNext> | MINUS <term> <expressionNext> | EPS
Expr * Parser::ExpressionNext(Expr * inheretedExp) {

    switch (_symbole.type) {
        case PLUS:
            ReadToken();
            return ExpressionNext(new BinOp(PLUS, inheretedExp, Term()));
        case MINUS:
            ReadToken();
            return ExpressionNext(new BinOp(MINUS, inheretedExp, Term()));
        default:
            return inheretedExp;
    }
}

// <term> -> <factor> <termNext>
Expr * Parser::Term() {
    return TermNext(Factor());
}

// <termNext> -> MULTIPLY <factor> <termNext> | DIVIDE <factor> <termNext> | kwMOD <factor> <termNext> | EPS
Expr * Parser::TermNext(Expr * inheretedExp) {
    switch (_symbole.type) {
        case MULTIPLY:
            ReadToken();
            return TermNext(new BinOp(MULTIPLY, inheretedExp, Term()));
        case DIVIDE:
            ReadToken();
            return TermNext(new BinOp(DIVIDE, inheretedExp, Term()));
        case kwMOD:
            ReadToken();
            return  TermNext(new BinOp(kwMOD, inheretedExp, Term()));
        default:
            return inheretedExp;
    }
}

// <factor> -> IDENT | NUMB | ( <expression> )
Expr * Parser::Factor() {
    switch (_symbole.type) {
        case IDENT: {
            string ident;
            CompareIndent(ident);
            return VarOrConst(ident);
        }
        case NUMB: {
            int value;
            CompareNumb(value);
            return new Numb(value);
        }
        case LPAR: {
            ReadToken();
            Expr * expression = Expression();
            Compare(RPAR);
            return expression;
        }
        default:
            throw ParserException("Factor has unsupported symbol.");
            //exit(1);
    }
}

// <numbarray> -> MINUS NUMB | NUMB
Numb * Parser::NumbArr() {
    int num;
    if (_symbole.type == MINUS) {
        ReadToken();
        CompareNumb(num);
        num = num * (-1);
    } else {
        CompareNumb(num);
    }

    return new Numb(num);
}


Expr * Parser::VarOrConst(string & id) {
    int value;
    SymboleType type = _symTable->GetConstOrVar(id, &value);

    switch (type) {
        case CONST:
            return new Numb(value);
        case VAR:
            return new Var(value, id, true);
        case ARRAY: {
            Compare(LBRA);

            Expr * index = Expression();

            Compare(RBRA);

            return new VarArray(id, index, true);
        }
        default:
            return nullptr;
    }
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------


