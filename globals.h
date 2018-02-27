#include <stdlib.h>

#ifndef _GLOBALS_H
#define _GLOBALS_H

#define ENDFILE 256
#define ERROR   257
#define NUM     258
#define PLUS    259
#define TIMES   260
#define LPAREN  261
#define RPAREN  262
#define SUB     263
#define DIV     264
#define ID      265
#define ASSIGN  266
#define LCUR    267
#define RCUR    268
#define LESS    269
#define MORE    270
#define LESSEQ  271
#define MOREEQ  272
#define NOTEQ   273
#define EQ      274
#define LBRAC   275
#define RBRAC   276
#define INTE    277
#define VOID    278
#define IF      279
#define ELSE    280
#define RETURN  281
#define WHILE   282
#define SEMI    283
// Out can also be a node ytpe
#define OUT     284
#define NEGATE  285
#define COMMA   286
// BREAK is also a node type
#define BREAK   287

// Node types
#define PROGRAM   0
#define FU_DE     1
#define VA_DE     2
#define CONST     3
#define WHILETP   4
#define IFTP      5
#define ASSIGNTP  6
#define CALL      7
#define FORMAL    8
#define BLOCK     9
#define RETTP     10
#define VAR       11
#define OP        12
#define BOOLOP    13

// types
#define INTEAR  279
#define VOIDAR  280

#define MAX_PAR 64

// Register
#define PC      7
#define SP      6
#define FP      5

using namespace std;

typedef struct {
  int TokenClass;  /* one of the above */
  string TokenString;
} TokenType;

#define MAXCHILDREN 128

typedef struct treeNode {
    struct treeNode * child[MAXCHILDREN];
    int op;
    int val;
    int nodeType;
    int varType;
    string id;
} TreeNode;

typedef struct storeNode {
    struct storeNode * next;
    int nodeType;
    int address;
    int isArray;
    string name;
} StoreNode;


typedef struct listNode {
    struct listNode * next;
    int nodeType;
    int symbolType;
    int param[MAX_PAR];
    string name;
} ListNode;

#define ST_SIZE 26

#define CODESIZE 1024

typedef enum {RO,RM} OpCodeType;

typedef struct {
  string opcode;
  OpCodeType ctype;
  int rand1;
  int rand2;
  int rand3;
} CodeType;

/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void);
TreeNode * statements(void);
ListNode* analyze(TreeNode* node, ListNode* parent, bool loop, int level, int expectedType);
void codeGenStmt(TreeNode*);
void printCode(void);

#endif
