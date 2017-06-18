//
// Created by Adam Zvada on 17.06.17.
//

#include "tabsym.h"

SymboleTable::~SymboleTable() {
    Symbole * curSymbole = _root;

    for (auto val : _globalTable) {
        delete val.second;
    }

    for (auto val : _localTable) {
        delete val.second;
    }
}

vector<Symbole *> & SymboleTable::GetAllGlobalVar() const {
    vector<Symbole *> allGlobal;

    for (auto var : _globalTable) {
        allGlobal.push_back(var.second);
    }
}

vector<Symbole *> & SymboleTable::GetAllLocalVar() const {
    vector<Symbole *> allLocal;

    for (auto var : _localTable) {
        allLocal.push_back(var.second);
    }
}

void SymboleTable::DeclareConst(string ident, int val) {
    auto it = _globalTable.find(ident);
    if (it == _globalTable.end()) {
        cout << "Constant \"" << ident << "\" was already decleared" << endl;
        exit(1);
    }

    Symbole *newSymbole = new Symbole(ident, SymboleType::CONST, val);

    if (!_isLocalScope) {
        _globalTable.insert(make_pair(ident, newSymbole));
    } else {
        _localTable.insert(make_pair(ident, newSymbole));
    }
}

void SymboleTable::DeclareVar(string ident) {
    auto it = _globalTable.find(ident);
    if (it == _globalTable.end()) {
        cout << "Variable \"" << ident << "\" was already decleared" << endl;
        exit(1);
    }

    Symbole * newSymbole = new Symbole(ident, SymboleType::VAR, 0);

    if (!_isLocalScope) {
        _globalTable.insert(make_pair(ident, newSymbole));
    } else {
        _localTable.insert(make_pair(ident, newSymbole));
    }
}

void SymboleTable::DeclareFunc(const string & name) {
    //TODO
}


Symbole * SymboleTable::FindIdent(string & ident) const {
    auto it = _globalTable.find(ident);
    if (it == _globalTable.end()) {
        return nullptr;
    }
    return it->second;
}


int SymboleTable::GetValue(string & ident) const {

    Symbole * symbole = FindIdent(ident);
    if (!symbole) {
        Error(ident, "was not decleared");
        return 0;
    }

    return symbole->value;
}


SymboleType SymboleTable::GetConstOrVar(string & ident, int * value) {
    Symbole * symbole = FindIdent(ident);

    switch (symbole->type) {
        case CONST:
            *value = symbole->value;
            return CONST;
        case VAR:
            *value = symbole->value;
            return VAR;
        default:
            Error(ident, "not decleared!");
            exit(1);
    }


}

void SymboleTable::EnterLocalScope() {
    _isLocalScope = true;
}

void SymboleTable::LeaveLocalScope() {
    _isLocalScope = false;

    for (auto val : _localTable) {
        delete val.second;
    }
    _localTable.clear();
}

