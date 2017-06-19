//
// Created by Adam Zvada on 17.06.17.
//

#include "lexan.h"
#include "input.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum {LETTER, NUMBER, WHITE_SPACE, END, NO_TYPE} InputCharType;


const char *symbTable[39] = {
        "IDENT",
        "NUMB",
        "PLUS", "MINUS", "MULTIPLY", "DIVIDE",
        "LESS", "LESS_OR_EQ", "GRATHER", "GRATHER_OR_EQ", "EQ", "NOT_EQ",
        "LPAR", "RPAR",
        "ASSIGN",
        "kwPROGRAM",
        "kwBEGIN", "kwEND",
        "kwVAR", "kwCONST",
        "kwFUNC",
        "kwIF", "kwTHEN", "kwELSE",
        "kwSWITCH",
        "kwFOR", "kwWHILE",
        "kwTO", "kwDO",
        "kwWRITE", "kwREAD",
        "kwAND", "kwOR",
        "kwMOD",
        "SEMICOLON", "COMMA", "DOT",
        "EOI", "ERR"
}; //symbol names in the same order as in LexSymbolType

static int character;   // vstupni znak
static InputCharType input; // vstupni symbol

void readInput(void) {
    character = getChar();
    if ((character>='A' && character<='Z') || (character>='a' && character<='z'))
        input = LETTER;
    else if (character>='0' && character<='9')
        input = NUMBER;
    else if (character == EOF)
        input = END;
    else if (character <= ' ')
        input = WHITE_SPACE;
    else
        input = NO_TYPE;
}

const struct {const char* slovo; LexSymbolType symb;} keyWordTable[] = {
        {"program", kwPROGRAM},
        {"begin", kwBEGIN}, {"end", kwEND},
        {"var", kwVAR}, {"const", kwCONST},
        {"function", kwFUNC},
        {"if", kwIF},
        {"then", kwTHEN},
        {"else", kwELSE},
        {"switch", kwSWITCH},
        {"for", kwFOR},
        {"while", kwWHILE},
        {"to", kwTO},
        {"do", kwDO},
        {"write", kwWRITE},
        {"read", kwREAD},
        {"and", kwAND},
        {"or", kwOR},
        {"mod", kwMOD},
        {NULL, (LexSymbolType) 0}
};

LexSymbolType keyWord(char* id) {
    int i = 0;
    while (keyWordTable[i].slovo)
        if (strcmp(id, keyWordTable[i].slovo)==0)
            return keyWordTable[i].symb;
        else
            i++;
    return IDENT;
}

void error(const char* text) {
    printf("\n%s\n", text);
    exit(1);
}

LexicalSymbol readLexem(void) {
    LexicalSymbol data;
    data.number = 0;
    int delkaId = 0;
    q0:
    switch (character) {
        case '{':
            readInput();
            goto q1;
        case '(':
            data.type = LPAR;
            readInput();

            return data;
        case ')':
            data.type = RPAR;
            readInput();

            return data;
        case '+':
            data.type = PLUS;
            readInput();

            return data;
        case '-':
            data.type = MINUS;
            readInput();

            return data;
        case '<':
            readInput();
            goto q4;
        case '>':
            readInput();
            goto q41;
        case ':':
            readInput();
            goto q5;
        case '$':
            data.type = NUMB;
            readInput();
            goto q31;
        case '&':
            data.type = NUMB;
            readInput();
            goto q32;
        case ';':
            data.type = SEMICOLON;
            readInput();

            return data;
        case ',':
            data.type = COMMA;
            readInput();

            return  data;
        case '.':
            data.type = DOT;
            readInput();

            return  data;
        case '=':
            readInput();
            goto q51;
        case '!':
            readInput();
            goto q52;
        default:;
    }

    switch (input) {
        case WHITE_SPACE:
            readInput();
            goto q0;
        case END:
            data.type = EOI;
            return data;
        case LETTER:
            delkaId = 1;
            data.ident[0] = character;
            readInput();
            goto q2;
        case NUMBER:
            data.number = character - '0';
            data.type = NUMB;
            readInput();
            goto q3;
        default:
            data.type = ERR;
            error("Nedovoleny znak.");

            return data;
    }

    q1:
    switch(character) {
        case '}':
            readInput();
            goto q0;
        default:;
    }
    switch(input) {
        case END:
            data.type = ERR;
            error("Neocekavany konec souboru v komentari.");
            return data;
        default:
            readInput();
            goto q1;
    }

    q2:
    switch(input) {
        case LETTER:
        case NUMBER:
            data.ident[delkaId] = character;
            delkaId++;
            readInput();
            goto q2;
        default:
            data.ident[delkaId] = 0;
            data.type = keyWord(data.ident);
            return data;
    }

    q3:
    switch(input) {
        case NUMBER:
            data.number = 10 * data.number + (character - '0');
            readInput();
            goto q3;
        default:
            return data;
    }

    q31: //HEXA
    switch (input) {
        case NUMBER:
        case LETTER:
            if ((character < 'a' && character > 'F') ||
                character > 'f') {
                data.type = ERR;
                error("Unvalid hexa number");
                return data;
            }
            data.number = 16 * data.number + ((character - '0') > 9 ? (character > 'F' ? character - 'a' + 10 : character - 'A' + 10) : character - '0');
            readInput();
            goto q31;
        default:
            return data;
    }

    q32: //OCTAL
    switch (input) {
        case NUMBER:
        case LETTER:
            if (character > '7' || character < '0') {
                data.type = ERR;
                error("Unvalid octal number");
                return data;
            }
            data.number = 8 * data.number + (character - '0');
            readInput();
            goto q32;
        default:
            return data;
    }
    q4:
    switch(character) {
        case '=':
            data.type = LESS_OR_EQ;
            readInput();
            return data;
        default:;
    }
    switch(input) {
        default:
            data.type = LESS;
            return data;
    }

    q41:
    switch (character) {
        case '=':
            data.type = GRATHER_OR_EQ;
            readInput();
            return data;
        default:;
    }
    switch(input) {
        default:
            data.type = GRATHER;
            return data;
    }

    q5:
    switch(character) {
        case '=':
            readInput();
            data.type = ASSIGN;
            return data;
        default:;
    }
    switch(input) {
        default:
            data.type = ERR;
            error("Ocekava se \'=\'.");
            return data;
    }


    q51:
    switch(character) {
        case '=':
            readInput();
            data.type = EQ;
            return data;
    }
    switch(input) {
        default:
            data.type = ERR;
            error("Ocekava se \'=\'.");
            return data;
    }

    q52:
    switch(character) {
        case '=':
            readInput();
            data.type = NOT_EQ;
            return data;
    }
    switch(input) {
        default:
            data.type = ERR;
            error("Ocekava se \'=\'.");
            return data;
    }

}

int initLexan(const char * fileName) {
    if(!initInput(fileName)) return 0;
    readInput();
    return 1;
}

