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
    LPAR, RPAR,
    ASSIGN,
    kwPROGRAM,
    kwBEGIN, kwEND,
    kwVAR, kwCONST,
    kwFUNC,
    kwIF, kwTHEN, kwELSE,
    kwSWITCH,
    kwFOR, kwWHILE,
    kwTO, kwDO,
    kwWRITE, kwREAD,
    SEMICOLON, COMMA,
    EOI, ERR
} LexSymbolType;

extern const char *symbTable[35];

#define MAX_IDENT_LEN 32

typedef struct LexicalSymbol {
    LexSymbolType type;
    char      ident[MAX_IDENT_LEN];  /* atribut symbolu IDENT */
    int       number;                 /* atribut symbolu NUMB */
} LexicalSymbol;

LexicalSymbol readLexem(void);

int initLexan(const char*);


#endif //TESTLLVM_LEXAN_H
