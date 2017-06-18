//
// Created by Adam Zvada on 17.06.17.
//

#ifndef TESTLLVM_TABSYM_H
#define TESTLLVM_TABSYM_H


#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>

#include "llvm/IR/Value.h"


using namespace std;

/**
 *  Symbole table for declared identifiers (variable or constants).
 *  We accept just integer values.
 *  Store in linked list(O(n)) because gonna be used for small programes and init of maps etc would be costly.
 *
 */

enum SymboleType {
    CONST, VAR, ARRAY, FUNC, UNDEF
};

struct Symbole {
    string ident;

    SymboleType type;

    int value;

    Symbole(string ident, SymboleType type, int value) : ident(ident), type(type), value(value) {}
};

class SymboleTable {
public:
    SymboleTable() : _root(nullptr), _isLocalScope(false) {};

    ~SymboleTable();

    vector<Symbole *> & GetAllGlobalVar() const;

    vector<Symbole *> & GetAllLocalVar() const;

    void DeclareConst(string ident, int val);

    void DeclareVar(string ident);

    void DeclareFunc(const string & name);

    int GetValue(string & ident) const;

    Symbole * FindIdent(string & ident) const;

    SymboleType GetConstOrVar(string & ident, int * value);

    void Error(string & ident, string errorWhat) const { cout << "Identifier " << ident << " " << errorWhat << endl; }

    void EnterLocalScope();

    void LeaveLocalScope();
private:
    Symbole * _root;

    bool _isLocalScope;

    map<string, Symbole *> _globalTable;
    map<string, Symbole *> _localTable;

    //TODO FUNC
    map<string, llvm::Value *> _funcTable;
};


#endif //TESTLLVM_TABSYM_H
