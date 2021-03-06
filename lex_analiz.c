#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sin_analiz.c"


Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk,Token);
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if(lastToken)
        lastToken->next = tk;
    else
        tokens = tk;
    lastToken = tk;
    return tk;
}

char *createString(char *startCh, char *endCh)
{
    char *result = (char*)malloc((endCh-startCh)*sizeof(char));
    int index=0;
    char aux;

    while((endCh-startCh) > 0)
    {
        aux = *(++startCh);
        startCh--;
		if((*startCh) == '\\')
		{
			if(aux == 't') result[index] = '\t';
			if(aux == 'r') result[index] = '\r';
			if(!isalpha(aux)) result[index] = aux;
			startCh++;
		}
		else result[index] = *startCh;
        index++;
        startCh++;
    }
    result[index] = '\0';

    return result;
}

int convertInt(char *str)
{
    int result;

    if(str[0] == '0')
        if(str[1] == 'x') result = (int)strtol(str, NULL, 0);
        else result = (int)strtol(str, NULL, 8);
    else
        result = atoi(str);

    return result;
}

int getNextToken()
{
    char ch;
    char *auxCh = "abfnrtv\'\"?\\0";
    int state=0;

    int n;
    Token *tk;
    char *pc, *pStartCh;

    while(true)
    {
        ch = *pCrtCh;
        switch(state)
        {
            case 0: {
                if(isalpha(ch) || ch == '_') { state = 1; pStartCh = pCrtCh; ++pCrtCh; }
                else if(ch == '0') { state = 3; pStartCh = pCrtCh; ++pCrtCh; }
                else if(isdigit(ch) && ch != '0') { state = 9; pStartCh = pCrtCh; ++pCrtCh; }
                else if(ch == '\'') { state = 16; pStartCh = pCrtCh; ++pCrtCh; }
                else if(ch == '"') { state = 20; pStartCh = pCrtCh; ++pCrtCh; }
                else if(ch == '/') { state = 23; ++pCrtCh; }
                else if(ch == ' ' || ch == '\r' || ch == '\t') { ++pCrtCh; }
                else if(ch == '\n') { line++; ++pCrtCh; }
                else if(ch == '!') { state = 28; ++pCrtCh; }
                else if(ch == '=') { state = 31; ++pCrtCh; }
                else if(ch == '&') { state = 34; ++pCrtCh; }
                else if(ch == '|') { state = 36; ++pCrtCh; }
                else if(ch == '<') { state = 38; ++pCrtCh; }
                else if(ch == '>') { state = 41; ++pCrtCh; }
                else if(ch == '.') { state = 44; ++pCrtCh; }
                else if(ch == '+') { state = 45; ++pCrtCh; }
                else if(ch == '-') { state = 46; ++pCrtCh; }
                else if(ch == '*') { state = 47; ++pCrtCh; }
                else if(ch == ',') { state = 48; ++pCrtCh; }
                else if(ch == ';') { state = 49; ++pCrtCh; }
                else if(ch == ')') { state = 51; ++pCrtCh; }
                else if(ch == '(') { state = 50; ++pCrtCh; }
                else if(ch == ']') { state = 53; ++pCrtCh; }
                else if(ch == '[') { state = 52; ++pCrtCh; }
                else if(ch == '}') { state = 54; ++pCrtCh; }
                else if(ch == '{') { state = 55; ++pCrtCh; }
                else if(ch == '\0') { addTk(END); return END; }
                else error("Caracter invalid");
                break;
            }
            case 1: {
                if(isalnum(ch) || ch == '_') ++pCrtCh;
                else state = 2;
                break;
            }
            case 2: {
                //ID
                n = pCrtCh - pStartCh;
                if(n==5 && !memcmp(pStartCh,"break",5)) tk=addTk(BREAK);
                else if(n==4 && !memcmp(pStartCh,"char",4)) tk=addTk(CHAR);
                else if(n==6 && !memcmp(pStartCh,"double",6)) tk=addTk(DOUBLE);
                else if(n==4 && !memcmp(pStartCh,"else",4)) tk=addTk(ELSE);
                else if(n==3 && !memcmp(pStartCh,"for",3)) tk=addTk(FOR);
                else if(n==2 && !memcmp(pStartCh,"if",2)) tk=addTk(IF);
                else if(n==3 && !memcmp(pStartCh,"int",3)) tk=addTk(INT);
                else if(n==6 && !memcmp(pStartCh,"return",3)) tk=addTk(RETURN);
                else if(n==6 && !memcmp(pStartCh,"struct",3)) tk=addTk(STRUCT);
                else if(n==4 && !memcmp(pStartCh,"void",3)) tk=addTk(VOID);
                else if(n==5 && !memcmp(pStartCh,"while",3)) tk=addTk(WHILE);
                else
                {
                    tk=addTk(ID);
                    tk->text = createString(pStartCh, pCrtCh);
                }
                return tk->code;
            }
            case 3: {
                if(ch == 'x') { state = 4; ++pCrtCh; }
                else if(ch == 'e' || ch == 'E') { state = 12; ++pCrtCh; }
                else if(ch == '.') { state = 10; ++pCrtCh; }
                else state = 7;
                break;
            }
            case 4: {
                if(isalnum(ch)) { state = 5; ++pCrtCh; }
                break;
            }
            case 5: {
                if(isalnum(ch)) { ++pCrtCh; }
                else state = 6;
                break;
            }
            case 6: {
                //CT_INT
                pc = createString(pStartCh, pCrtCh);
                tk = addTk(CT_INT);
                tk->c = convertInt(pc);
                free(pc);
                return CT_INT;
            }
            case 7: {
                if(isdigit(ch) && ch != '9' && ch != '8') { ++pCrtCh; }
                else if(ch == '8' || ch == '9') { state = 8; ++pCrtCh; }
                else state = 6;
                break;
            }
            case 8: {
                if(isdigit(ch)) { ++pCrtCh; }
                else if(ch == '.') { state = 10; ++pCrtCh; }
                else if(ch == 'e' || ch == 'E') { state = 12; ++pCrtCh; }
                break;
            }
            case 9: {
                if(isdigit(ch)) { ++pCrtCh; }
                else if(ch == '.') { state = 10; ++pCrtCh; }
                else if(ch == 'e' || ch == 'E') { state = 12; ++pCrtCh; }
                else state = 6;
                break;
            }
            case 10: {
                if(isdigit(ch)) { state = 11; ++pCrtCh; }
                break;
            }
            case 11: {
                if(isdigit(ch)) { state = 11; ++pCrtCh; }
                else if(ch == 'e' || ch == 'E') { state = 12; ++pCrtCh; }
                else state = 15;
                break;
            }
            case 12: {
                if(ch == '+' || ch == '-') { state = 13; ++pCrtCh; }
                else state = 13;
                break;
            }
            case 13: {
                if(isdigit(ch)) { state = 14; ++pCrtCh; }
                break;
            }
            case 14: {
                if(isdigit(ch)) { ++pCrtCh; }
                else state = 15;
                break;
            }
            case 15: {
                //CT_REAL
                pc = createString(pStartCh, pCrtCh);
                tk = addTk(CT_REAL);
                tk->d = atof(pc);
                free(pc);
                return CT_REAL;
            }
            case 16: {
                if(ch == '\\') { state = 17; ++pCrtCh; }
                else if(ch != '\'' && ch != '\\') { state = 18; ++pCrtCh; }
                break;
            }
            case 17: {
                if(strchr(auxCh, ch) != NULL) { state = 18; ++pCrtCh; }
                break;
            }
            case 18: {
                if(ch == '\'') { state = 19; ++pCrtCh; }
                break;
            }
            case 19: {
                //CT_CHAR
                tk=addTk(CT_CHAR);
				if(pStartCh[1] == '\\')
				{
					if(!isalpha(pStartCh[2])) tk->c = pStartCh[2];
					else if(pStartCh[2] == 't') tk->c = '\t';
					else if(pStartCh[2] == 'r') tk->c = '\r';
					else if(pStartCh[2] == 'n') tk->c = '\n';
				}
				else tk->c = pStartCh[1];
                return CT_CHAR;
            }
            case 20: {
                if(ch == '\"') { state = 22; ++pCrtCh; }
                else if(ch == '\\') { state = 21; ++pCrtCh; }
                else { ++pCrtCh; }
                break;
            }
            case 21: {
                if(strchr(auxCh, ch) != NULL) { state = 20; ++pCrtCh; }
                break;
            }
            case 22: {
                //CT_STRING
                tk = addTk(CT_STRING);
                tk->text = createString(pStartCh, pCrtCh);
                return CT_STRING;
            }
            case 23: {
                if(ch == '/') { state = 25; ++pCrtCh; }
                else if(ch == '*') { state = 26; ++pCrtCh; }
                else state = 24;
                break;
            }
            case 24: {
                addTk(DIV);
                return DIV;
            }
            case 25: {
                if(ch != '\r' && ch != '\0' && ch != '\n') ++pCrtCh;
                else state = 0;
                break;
            }
            case 26: {
                if(ch == '\n') line++;
                if(ch == '*') { state = 27; ++pCrtCh; }
                else ++pCrtCh;
                break;
            }
            case 27: {
                if(ch == '\n') line++;
                if(ch == '*') ++pCrtCh;
                else if(ch == '/') { state = 0; ++pCrtCh; }
                else { state = 26; ++pCrtCh; }
                break;
            }
            case 28: {
                if(ch == '!') { state = 30; ++pCrtCh; }
                else state = 29;
                break;
            }
            case 29: {
                addTk(NOT);
                return NOT;
            }
            case 30: {
                addTk(NOTEQ);
                return NOTEQ;
            }
            case 31: {
                if(ch == '=') { state = 33; ++pCrtCh; }
                else state = 32;
                break;
            }
            case 32: {
                addTk(ASSIGN);
                return ASSIGN;
            }
            case 33: {
                addTk(EQUAL);
                return EQUAL;
            }
            case 34: {
                if(ch == '&') { state = 35; ++pCrtCh; }
                break;
            }
            case 35: {
                addTk(AND);
                return AND;
            }
            case 36: {
                if(ch == '|') { state = 37; ++pCrtCh; }
                break;
            }
            case 37: {
                addTk(OR);
                return OR;
            }
            case 38: {
                if(ch == '=') { state = 40; ++pCrtCh; }
                else state = 39;
                break;
            }
            case 39: {
                addTk(LESS);
                return LESS;
            }
            case 40: {
                addTk(LESSEQ);
                return LESSEQ;
            }
            case 41: {
                if(ch == '=') { state = 43; ++pCrtCh; }
                else state = 42;
                break;
            }
            case 42: {
                addTk(GREATER);
                return GREATER;
            }
            case 43: {
                addTk(GREATEREQ);
                return GREATEREQ;
            }
            case 44: {
                addTk(DOT);
                return DOT;
            }
            case 45: {
                addTk(ADD);
                return ADD;
            }
            case 46: {
                addTk(SUB);
                return SUB;
            }
            case 47: {
                addTk(MUL);
                return MUL;
            }
            case 48: {
                addTk(COMMA);
                return COMMA;
            }
            case 49: {
                addTk(SEMICOLON);
                return SEMICOLON;
            }
            case 50: {
                addTk(LPAR);
                return LPAR;
            }
            case 51: {
                addTk(RPAR);
                return RPAR;
            }
            case 52: {
                addTk(LBRACKET);
                return LBRACKET;
            }
            case 53: {
                addTk(RBRACKET);
                return RBRACKET;
            }
            case 54: {
                addTk(RACC);
                return RACC;
            }
            case 55: {
                addTk(LACC);
                return LACC;
            }
            default: {
                error("Stare invalida");
                break;
            }
        }
    }
}

void afisare()
{
    Token *tk;

    for(tk=tokens; tk!=NULL; tk=tk->next)
    {
        printf("%4d : %s",tk->line,convertAtomsName(tk->code));

        if(tk->code == 1)
            printf(" \t\t-> %s ",tk->text);
        else if(tk->code == 4)
            printf(" \t-> %s ",tk->text);
        else if(tk->code == 2)
            printf(" \t\t-> %d ",tk->c);
        else if(tk->code == 5)
            printf(" \t\t-> %c ",tk->c);
        else if(tk->code == 3)
            printf(" \t\t-> %f ",tk->d);
        
        printf("\n");
    }
}