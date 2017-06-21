//
// Created by Adam Zvada on 17.06.17.
//

#ifndef TESTLLVM_AST_H
#define TESTLLVM_AST_H


#include <stdio.h>
#include <iostream>
#include <map>

#include "lexan.h"
#include "tabsym.h"

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace std;
using namespace llvm;

//-------------------------------------------------------------

void llvmAstInit(LLVMContext & theContext, Module * theModule, IRBuilder<> & theBuilder, BasicBlock * breakTarget, SymboleTable * symbTable);

//-------------------------------------------------------------

class Node {
public:
    virtual Node * Optimize() { return this; }
    virtual void Translate() = 0;
    virtual ~Node() {}
    /*
     * Emits IR for AST node, Value represents SSA
     */
    virtual llvm::Value * GenerateIR() = 0;
};

//-------------------------------------------------------------

class Expr : public Node {
};

class Var : public Expr {
public:
    Var(const string & name, bool rvalue = false) : _value(0), _name(name), _rvalue(rvalue) {}
    Var(int value, const string & name, bool rvalue = false) : _value(value), _name(name), _rvalue(rvalue) {}
    virtual ~Var() {}
    virtual Var * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();

    string GetName() const { return _name; }
private:
    int _value;
    string _name;
    bool _rvalue;
};

class Numb : public Expr {
public:
    Numb(int value) : _value(value) {}
    virtual ~Numb() {}
    virtual Numb * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();
private:
    int _value;
};

class BinOp : public Expr {
public:
    BinOp(LexSymbolType op, Expr * left, Expr * right) : _op(op), _left(left), _right(right) {}
    virtual ~BinOp() {}
    virtual BinOp * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();
private:
    LexSymbolType _op;
    Expr * _left;
    Expr * _right;
};

class UnaryMinus : public Expr {
public:
    UnaryMinus(Expr * expression) : _expression(expression) {}
    virtual ~UnaryMinus() {}
    virtual UnaryMinus * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();
private:
    Expr * _expression;
};

//-------------------------------------------------------------


class Statm : public Node {
};

class StatmList : public Statm {
public:
    StatmList(Statm * statement, StatmList * statementList) : _statement(statement), _next(statementList) {}
    virtual ~StatmList() {}
    virtual StatmList * Optimize() {return this;}
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();
private:
    Statm * _statement;
    StatmList * _next;
};

class Assign : public Statm {
public:
    Assign(Var * var, Expr * expr) : _var(var), _expr(expr) {}
    virtual ~Assign() {}
    virtual Assign * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();

    Var * GetVar() const { return _var; }
private:
    Var * _var;
    Expr * _expr;
};

class Write : public Statm {
public:
    Write(Expr * expression) : _expression(expression) {}
    virtual ~Write() {}
    virtual Write * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();
private:
    Expr * _expression;

    Constant * printFunc();
};

class Read : public Statm {
public:
    Read(Var * var) : _var(var) {}
    virtual ~Read() {}
    virtual Read * Optimize() { return this; }
    virtual void Translate() {}
    virtual Value * GenerateIR();

private:
    Var * _var;

    Constant * scanfFunc();
};

class If : public Statm {
public:
    If(Expr * expression, Statm * thenStatement, Statm * elseStatement) : _condition(expression), _then(thenStatement), _else(elseStatement) {}
    virtual ~If() {}
    virtual If * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();
private:
    Expr * _condition;
    Statm * _then;
    Statm * _else;
};

class While : public Statm {
public:
    While(Expr * condition, Statm * statement) : _condition(condition), _statement(statement) {}
    virtual ~While() {}
    virtual While * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();
private:
    Expr * _condition;
    Statm * _statement;
};

//For Loop is created by using while loop

//class For : public Statm {
//public:
//    For(Statm * assign, bool isAscending, Expr * toExpr, Statm * body) : _initAssign(assign), _isAscending(isAscending), _to(toExpr), _body(body) {}
//    virtual ~For() {}
//    virtual For * Optimize() { return this; }
//    virtual void Translate() {}
//    virtual llvm::Value * GenerateIR();
//
//private:
//    Statm * _initAssign;
//    bool _isAscending;
//    Expr * _to;
//    Statm * _body;
//};


class Break : public Statm {
public:
    Break() {}
    virtual ~Break() {}
    virtual Break * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();

private:
};

//class Empty : public Statm {
//
//
//};


//-------------------------------------------------------------


class Prog : public Node {
public:
    Prog(string programIdent, StatmList * statmentList) : _programIdent(programIdent), _statmentList(statmentList) {}
    virtual ~Prog() {}
    virtual Prog * Optimize() { return this; }
    virtual void Translate() {}
    virtual llvm::Value * GenerateIR();

    string NameProgram() const { return _programIdent; }
private:
    StatmList * _statmentList;
    string _programIdent;
};


#endif //TESTLLVM_AST_H
