#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;
#include "globals.h"
/*
To compile:
g++ scan.cc parse.cc -std=c++11
As stoi is used which is a c++11 function
*/

/* Syntax Analyzer for the calculator language */
static string TYPE[] = {"int", "void", "int array", "void array"};
static TokenType token; /* holds current token */
TokenType getToken(void);

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newNode(int type) {
  TreeNode * t = new TreeNode();
  int i;
  if (t==NULL) {
    cerr << "Out of memory error at line %d\n";
    exit(1);
  }
  else {
    for (i=0;i < MAXCHILDREN;i++){
      t->child[i] = NULL;
    }
    t->nodeType = type;
    t->val = 0;
  }
  return t;
}

static void advance(bool condition) {
  if (condition) token = getToken();
  else {
    cerr << "unexpected token -> " << token.TokenString << endl; 
    exit(-1);
  }
}

// Utility function mapping the type index to type string
string getType(int type){
  switch(type){
    case INTE:
      return "int";

    case VOID:
      return "void";

    case INTEAR:
      return "int array";

    case VOIDAR:
      return "void array";

    default:
      return "error"; 
  }
}

// add parameter list to node.params
int formals(TreeNode * node){
  advance(token.TokenClass == LPAREN);
  int counter = 0;
  bool first = true;
  // check for conditions where formals are only void
  if (token.TokenClass == VOID){
    first = false;
    TreeNode * temp = newNode(FORMAL);
    advance(true);
    temp -> varType = VOID;
    if (token.TokenClass != RPAREN){
      // This is a normal void typed variable
      temp -> id = token.TokenString.c_str();
      advance(token.TokenClass == ID);

      // check if is an array
      if (token.TokenClass == LBRAC){
        advance(true);
        advance(token.TokenClass == RBRAC);
        temp -> varType = temp->varType + 2;
      }
      node -> child[counter++] = temp;
    }
  }
  while(token.TokenClass != RPAREN){
    if (!first){
      advance(token.TokenClass == COMMA);
    }
    first = false;
    TreeNode * temp = newNode(FORMAL);
    temp -> varType = token.TokenClass;

    advance(token.TokenClass == INTE || token.TokenClass == VOID);

    temp -> id = token.TokenString.c_str();
    advance(token.TokenClass == ID);

    // check if is an array
    if (token.TokenClass == LBRAC){
      advance(true);
      advance(token.TokenClass == RBRAC);
      temp -> varType = temp->varType + 2;
    }

    node -> child[counter++] = temp;


  }
  advance(token.TokenClass == RPAREN);
  return counter;
}

TreeNode* declare(bool varOnly);
TreeNode* statement(void);
TreeNode* parseProduct(void);
TreeNode* atirhFactor(void);
TreeNode* parseSum(void);
TreeNode* expression(void);
TreeNode* comparison(void);

// Add the actuals to params attribute in node 
void actuals(TreeNode * node){
  advance(token.TokenClass == LPAREN);
  int counter = 0;
  bool first = true;
  while (token.TokenClass != RPAREN){
    if (!first){
      advance(token.TokenClass == COMMA);
    }
    first = false;
    node -> child[counter ++] = expression();
  }
  advance(token.TokenClass == RPAREN);
}

// Return a TreeNode typed Block containing all the statements 
// decalrations in such a block
TreeNode* block(void){
  TreeNode * res = newNode(BLOCK);
  advance(token.TokenClass == LCUR);
  int counter = 0;
  while(token.TokenClass != RCUR){
    if (token.TokenClass == INTE || token.TokenClass == VOID){
      res ->child[counter++] = declare(true);
    } else{
      res ->child[counter++] = statement();
      //advance(token.TokenClass == SEMI);
    }
  }
  advance(token.TokenClass == RCUR);
  return res;
}

TreeNode* parseSum(void){
  TreeNode* left = parseProduct();
  TreeNode* res = NULL;
  while (token.TokenClass == PLUS || token.TokenClass == SUB){
    TreeNode * old = res == NULL ? left : res;
    res = newNode(OP);
    res->op = token.TokenClass;
    res->id = token.TokenString;
    advance(true);
    TreeNode * right = parseProduct();
    res->child[0] = old;
    res->child[1] = right;
  }
  return res == NULL ? left : res;
}

TreeNode* parseProduct(void){
  TreeNode* left = atirhFactor();
  TreeNode* res = NULL;
  while (token.TokenClass == TIMES || token.TokenClass == DIV){
    TreeNode * old = res == NULL ? left : res;
    res = newNode(OP);
    res->op = token.TokenClass;
    res->id = token.TokenString;
    advance(true);
    TreeNode * right = atirhFactor();
    res->child[0] = old;
    res->child[1] = right;
  }
  return res == NULL ? left : res;
}

TreeNode* atirhFactor(void){
  TreeNode * res;
  switch(token.TokenClass){
    case NUM:
      res = newNode(CONST);
      res -> val = stoi(token.TokenString);
      advance(true);
      break;

    case ID:
      res = newNode(VAR);
      res -> id = token.TokenString;
      advance(true);
      if (token.TokenClass == LBRAC){
        advance(true);
        res -> child[0] = expression();
        advance(token.TokenClass == RBRAC);
      } else if (token.TokenClass == LPAREN){
        res -> nodeType = CALL;
        actuals(res);
      }
      break;

    case LPAREN:
      advance(true);
      res = expression();
      advance(token.TokenClass == RPAREN);
      break;

    default:
      cerr << "unexpected token -> " << token.TokenString << "\n";
      exit(-1);
  }
  return res;
}

// Handles a boolean experssion inside parenthesis
TreeNode* comparison(void){
  TreeNode * res;
  res = newNode(BOOLOP);
  advance(token.TokenClass == LPAREN);
  if (token.TokenClass == NEGATE){
    res -> op = NEGATE;
    res->id = token.TokenString;
    advance(true);
    res -> child[0] = comparison();
  } else{
    res -> child[0] = parseSum();
    res -> op = token.TokenClass;
    res->id = token.TokenString;
    advance(token.TokenClass == LESS 
      || token.TokenClass == MORE 
      || token.TokenClass == LESSEQ
      || token.TokenClass == MOREEQ
      || token.TokenClass == NOTEQ
      || token.TokenClass == EQ
    );
    res -> child[1] = parseSum();
  }
  advance(token.TokenClass == RPAREN);
  return res;
}

TreeNode* expression(void){
  TreeNode* LHS = parseSum();
  // returns LHS but not advance
  if (token.TokenClass == RPAREN || token.TokenClass == RBRAC || token.TokenClass == COMMA){
    return LHS;
  }
  if (token.TokenClass == SEMI){
    advance(true);
    return LHS;
  } else if (token.TokenClass == ASSIGN){
    if (LHS -> nodeType != VAR){
      cerr << "Cannot assign to non-variable term\n";
      exit(-1);
    }
    TreeNode* res = newNode(ASSIGNTP);
    res->child[0] = LHS;
    advance(token.TokenClass == ASSIGN);
    res->child[1] = expression();
    return res;
  } else{
    cerr << "unexpected token -> " << token.TokenString << "\n";
    exit(-1);
  }

}

TreeNode* select(void){
  advance(token.TokenClass == IF);
  TreeNode * res = newNode(IFTP);
  res -> child[0] = comparison();
  res -> child[1] = statement();
  if (token.TokenClass == ELSE){
    advance(token.TokenClass == ELSE);
    res -> child[2] = statement();
  }
  return res;
}

TreeNode* repetition(void){
  advance(token.TokenClass == WHILE);
  TreeNode * res = newNode(WHILETP);
  res -> child[0] = comparison();
  res -> child[1] = statement();
  return res;
}

TreeNode* returnStmt(void){
  advance(token.TokenClass == RETURN);
  TreeNode* res = newNode(RETTP);
  if (token.TokenClass != SEMI){
    res->child[0] = expression();
  } else{
    advance(token.TokenClass == SEMI);
  }
  return res;

}

TreeNode* output(void){
  advance(token.TokenClass == OUT);
  TreeNode* res = newNode(OUT);
  res->child[0] = expression();
  return res;
}

TreeNode* breakStmt(void){
  advance(token.TokenClass == BREAK);
  TreeNode* res = newNode(BREAK);
  advance(token.TokenClass == SEMI);
  return res;
}

TreeNode* statement(void){
  switch(token.TokenClass){
    case RETURN:
      return returnStmt();

    case WHILE:
      return repetition();

    case IF:
      return select();

    case OUT:
      return output();

    case LCUR:
      return block();

    case BREAK:
      return breakStmt();

    case SEMI:
      advance(true);
      return NULL;

    default:
      return expression();
  }
}

// Handles the function and variable declaration
TreeNode * declare(bool varOnly) {
  int returnType = token.TokenClass;
  advance(token.TokenClass == INTE || token.TokenClass == VOID);
  string name = token.TokenString;
  advance(token.TokenClass == ID);

  // This is bascially decalre'
  TreeNode * res;
  switch(token.TokenClass){
    case LBRAC:
      advance(true);
      if (token.TokenClass != NUM){
        cerr << "Number expected";
        exit(-1);
      }
      int len;
      len = stoi(token.TokenString);
      advance(token.TokenClass == NUM);
      advance(token.TokenClass == RBRAC);
      advance(token.TokenClass == SEMI);
      returnType += 2;
      // This is a variable decalration(array)
      res = newNode(VA_DE);
      res -> val = len;
      break;
    case SEMI:
      // This is a variable decalration
      res = newNode(VA_DE);
      advance(true);
      break;
    case LPAREN:
      // This is a function
      if (varOnly){
        cerr << "Function decalration not allowed\n";
        exit(-1);
      }
      res = newNode(FU_DE);
      int pos;
      pos = formals(res);
      res->child[pos] = block();
      break;

    default:
      cerr << token.TokenClass;
      cerr << "unexpected token -> " << token.TokenString;
  }
  res -> varType = returnType;
  res -> id = name;
  return res;
}

// Main function being called 
TreeNode * program(void) {
  TreeNode *head = newNode(PROGRAM);
  token = getToken();
  int counter = 0;
  while (token.TokenClass != ENDFILE){
    TreeNode * decl = declare(false);
    head -> child[counter++] = decl;
  }
  return head;
}

// Utility function for showing levels
void indent(int level){
  for (int j = 0; j < level; j++){
    cout << "\t";
  }
}

// Prints out each node
void traverse(TreeNode * node, int level){
  if (node == NULL){
    return;
  }
  switch(node->nodeType){
    case PROGRAM:
      cout << "Program begins\n";
      cout << "Note:\n";
      cout << "For if statement: 1st child cond, 2nd child stmt, 3rd child else\n";
      cout << "For while statement: 1st child cond, 2nd child stmt\n";
      cout << "For variables: variable containing child are array access\n";
      break;

    case FU_DE:
      indent(level);
      cout << "Function decalration: " << node->id << " (returnType: " << getType(node->varType) << ")\n";
      break;

    case VA_DE:
      indent(level);
      cout << "Variable decalration: " << node->id << " (Type: " << getType(node->varType) << ") ";
      if (node->val != 0){
        cout << "Size: " << node -> val;
      }
      cout << "\n";
      break;

    case FORMAL:
      indent(level);
      cout << "Parameter " << node->id << ": " << getType(node->varType) << "\n";
      break;

    case BLOCK:
      indent(level);
      cout << "Compound statement\n";
      break;

    case OP:
      indent(level);
      cout << "OP: " << node -> id << "\n";
      break;

    case CONST:
      indent(level);
      cout << "Number: " << node ->val << "\n";
      break;

    case VAR:
      indent(level);
      cout << "Variable: " << node -> id << "\n";
      break;

    case ASSIGNTP:
      indent(level);
      cout << "Assign" << "\n";
      break;

    case CALL:
      indent(level);
      cout << "Call: " << node-> id << "\n";
      break;

    case OUT:
      indent(level);
      cout << "Output: \n";
      break;

    case RETTP:
      indent(level);
      cout << "Return: \n";
      break;

    case BOOLOP:
      indent(level);
      cout << "Boolean Operator: " << node -> id << "\n";
      break;

    case IFTP:
      indent(level);
      cout << "If statement: \n";
      break;

    case WHILETP:
      indent(level);
      cout << "While statement: \n";
      break;

    case BREAK:
      indent(level);
      cout << "Break statement: \n";
      break;

  }
  for (int i = 0; node->child[i] != NULL; i++){
    traverse(node->child[i], level + 1);
  }
}

