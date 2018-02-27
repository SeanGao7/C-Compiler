#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include "globals.h"

/* Lexical Analyzer for the calculator language */

/* states in the scanner DFA */
typedef enum { 
  START,INNUM,INID,WHILE1,WHILE2,WHILE3,WHILE4,WHILE5,INT1,INT2,INT3,
  VOD1,VOD2,VOD3,VOD4,IF1,ELSE1,ELSE2,ELSE3,ELSE4,RETURN1,RETURN2,
  RETURN3,RETURN4,RETURN5,RETURN6,SLASH,CMT,CMTSATR,GT,LT,NOT,EQUAL,
  BREAK1,BREAK2,BREAK3,BREAK4,BREAK5,DONE
} StateType;

bool LayOutCharacter(char c) {
  return ((c == ' ') || (c == '\t') || (c == '\n'));
}

bool isalpha(char c){
  return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

TokenType getToken(void) {
  /* holds current token to be returned */
  TokenType currentToken;
  /* current state - always begins at START */
  StateType state = START;
  /* next character */
  char c;

  do {
    c = cin.get();
  } while (LayOutCharacter(c));

  while (state != DONE) {
    switch (state) {
      case START:
        while(LayOutCharacter(c)){
          c = cin.get();
        }
        if (isdigit(c)){
          state = INNUM;

        }
        else if (isalpha(c)) {
      	  switch(c){
            case 'i':
              state = INT1;
              break;
            case 'v':
              state = VOD1;
              break;
            case 'w':
              state = WHILE1;
              break;
            case 'e':
              state = ELSE1;
              break;
            case 'r':
              state = RETURN1;
              break;
            case 'b':
              state = BREAK1;
              break;
            default:
              state = INID;
          }
        }
        else if (c == '/' || c == '=' || c == '>' || c == '<' || c== '!'){ 
          // Handles symbols(non-numerical /alphabetical) which can have more than one characters
          switch(c){
            case '/':
              state = SLASH;
              break;
            case '=':
              state = EQUAL;
              break;
            case '>':
              state = GT;
              break;
            case '<':
              state = LT;
              break;
            case '!':
              state = NOT;
              break;
          }
        }
        else {
      	  state = DONE;
      	  switch (c){ 
            case EOF:
              currentToken.TokenString = EOF;
              currentToken.TokenClass = ENDFILE;
              break;
            case '+':
              currentToken.TokenString = '+';
              currentToken.TokenClass = PLUS;
              break;
            case '-':
              currentToken.TokenString = '-';
              currentToken.TokenClass = SUB;
              break;
            case '*':
              currentToken.TokenString = '*';
              currentToken.TokenClass = TIMES;
              break;
            case '(':
              currentToken.TokenString = '(';
              currentToken.TokenClass = LPAREN;
              break;
            case ')':
              currentToken.TokenString = ')';
              currentToken.TokenClass = RPAREN;
              break;
            case '{':
              currentToken.TokenString = '{';
              currentToken.TokenClass = LCUR;
              break;
            case '}':
              currentToken.TokenString = '}';
              currentToken.TokenClass = RCUR;
              break;
            case '[':
              currentToken.TokenString = '[';
              currentToken.TokenClass = LBRAC;
              break;
            case ']':
              currentToken.TokenString = ']';
              currentToken.TokenClass = RBRAC;
              break;
            case ';':
              currentToken.TokenString = ';';
              currentToken.TokenClass = SEMI;
              break;
            case '$':
              currentToken.TokenString = '$';
              currentToken.TokenClass = OUT;
              break;
            case ',':
              currentToken.TokenString = ',';
              currentToken.TokenClass = COMMA;
              break;
            default:
              currentToken.TokenString = c;
              currentToken.TokenClass = ERROR;
              break;
          }
        }
        break;

      // Handling the number interals
      case INNUM:
        if (!isdigit(c)){ /* backup in the input */
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = NUM;
        }
        break;

      // Handling the IDs, where subsequent chars can be either alphabetical or numerical
      case INID:
        if (!isdigit(c) && !isalpha(c)){ /* backup in the input */
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;

      // Handling keyword while
      case WHILE1:
        if (c == 'h'){
          state = WHILE2;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case WHILE2:
        if (c == 'i'){
          state = WHILE3;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case WHILE3:
        if (c == 'l'){
          state = WHILE4;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case WHILE4:
        if (c == 'e'){
          state = WHILE5;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case WHILE5:
        if (!isdigit(c) && !isalpha(c)){
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = WHILE;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;

      // Handles key word break
      case BREAK1:
        if (c == 'r'){
          state = BREAK2;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case BREAK2:
        if (c == 'e'){
          state = BREAK3;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case BREAK3:
        if (c == 'a'){
          state = BREAK4;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case BREAK4:
        if (c == 'k'){
          state = BREAK5;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case BREAK5:
        if (!isdigit(c) && !isalpha(c)){
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = BREAK;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;

      // Handling keyword if and int, where the first stage can just to either if/int
      case INT1:
        if (c == 'f'){
          state = IF1;
        } else if (c == 'n'){
          state = INT2;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case INT2:
        if (c == 't'){
          state = INT3;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case INT3:
        if (!isdigit(c) && !isalpha(c)){
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = INTE;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case IF1:
        if (isdigit(c) || isalpha(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = IF;
        }
        break;

      // Handling keyword return
      case RETURN1:
        if (c == 'e'){
          state = RETURN2;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case RETURN2:
        if (c == 't'){
          state = RETURN3;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case RETURN3:
        if (c == 'u'){
          state = RETURN4;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case RETURN4:
        if (c == 'r'){
          state = RETURN5;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case RETURN5:
        if (c == 'n'){
          state = RETURN6;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case RETURN6:
        if (!isdigit(c) && !isalpha(c)){
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = RETURN;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;

      // Handling keyword void
      case VOD1:
        if (c == 'o'){
          state = VOD2;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case VOD2:
        if (c == 'i'){
          state = VOD3;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case VOD3:
        if (c == 'd'){
          state = VOD4;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case VOD4:
        if (!isdigit(c) && !isalpha(c)){
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = VOID;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;

      // Handling keyword else
      case ELSE1:
        if (c == 'l'){
          state = ELSE2;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case ELSE2:
        if (c == 's'){
          state = ELSE3;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case ELSE3:
        if (c == 'e'){
          state = ELSE4;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;
      case ELSE4:
        if (!isdigit(c) && !isalpha(c)){
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ELSE;
        } else if (isalpha(c) || isdigit(c)){
          state = INID;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = ID;
        }
        break;

      // Distinguish between = and == operator
      case EQUAL:
        state = DONE;
        if (c == '='){
          currentToken.TokenClass = EQ;
          currentToken.TokenString += c;
        } else{
          cin.putback(c);
          currentToken.TokenClass = ASSIGN;
        }
      break;

      // Distinguish between > and >= operator
      case GT:
        state = DONE;
        if (c == '='){
          currentToken.TokenClass = MOREEQ;
          currentToken.TokenString += c;
        } else{
          cin.putback(c);
          currentToken.TokenClass = MORE;
        }
      break;

      // Distinguish between < and <= operator
      case LT:
        state = DONE;
        if (c == '='){
          currentToken.TokenClass = LESSEQ;
          currentToken.TokenString += c;
        } else{
          cin.putback(c);
          currentToken.TokenClass = LESS;
        }
      break;

      // Distiguish between / and /*(start of comment)
      case SLASH:
        if (c == '*'){
          state = CMT;
        } else{
          cin.putback(c);
          state = DONE;
          currentToken.TokenClass = DIV;
        }
        break;

      // The stage when comment has been started
      case CMT:
        if (c == '*'){
          state = CMTSATR;
        }
        break;

      // The stage when second star has been detected
      case CMTSATR:
        if (c == '/'){
          currentToken.TokenString = "";
          state = START;
        } else if (c != '*'){
          state = CMT;
        }
        break;

      case NOT:
        state = DONE;
        if (c == '='){
          currentToken.TokenString += c;
          currentToken.TokenClass = NOTEQ;
        }
        else{
          cin.putback(c);
          currentToken.TokenClass = NEGATE;
        }
        break;

      case DONE:
        break;
      default: /* should never happen */
        state = DONE;
        currentToken.TokenClass = ERROR;
        break;
    }
    if (state != DONE && state != START){
      currentToken.TokenString += c;
    }
    if (state != DONE){
      c = cin.get();
    }
  }
  return currentToken;
} /* end getToken */
/*
int main(int argc, char* argv[]){
  TokenType current;
  while ((current = getToken()).TokenClass != ENDFILE){
    cout << current.TokenString;
    cout << " ";
    cout << current.TokenClass;
    cout << "\n";
  }
}
*/