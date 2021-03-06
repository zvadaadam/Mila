//
// Created by Adam Zvada on 17.06.17.
//

#ifndef TESTLLVM_LEXAN_H
#define TESTLLVM_LEXAN_H


typedef enum {
    IDENT,
    NUMB,
    PLUS, MINUS, MULTIPLY, DIVIDE,
    LESS, LESS_OR_EQ, GRATHER, GRATHER_OR_EQ, EQ, NOT_EQ,
    LPAR, RPAR, LBRA, RBRA,
    ASSIGN,
    kwPROGRAM,
    kwBEGIN, kwEND,
    kwVAR, kwCONST, kwINTEGER,
    kwARRAY,
    kwFUNC,
    kwIF, kwTHEN, kwELSE,
    kwSWITCH,
    kwFOR, kwWHILE,
    kwTO, kwDOWNTO, kwDO,
    kwBREAK,
    kwWRITE, kwREAD,
    kwOR, kwAND,
    kwMOD,
    SEMICOLON, COLON, COMMA, DOT,
    EOI, ERR
} LexSymbolType;

extern const char * symbTable[46];

#define MAX_IDENT_LEN 32

typedef struct LexicalSymbol {
    LexSymbolType type;
    char      ident[MAX_IDENT_LEN];  /* atribut symbolu IDENT */
    int       number;                 /* atribut symbolu NUMB */
} LexicalSymbol;

LexicalSymbol readLexem(void);

int initLexan(const char*);


#endif //TESTLLVM_LEXAN_H
