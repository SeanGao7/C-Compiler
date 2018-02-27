#include <iostream>
#include <cstring>
#include "globals.h"

static CodeType codeArray[CODESIZE];
static int ICounter = 0;

int lenOfVar = 0;
bool mainFound = 0;
int params = 0;
bool inMain = 0;

int * whileBranch;

/*
To compile:
g++ code-gen.cc scan.cc parse.cc analyze.cc -std=c++11
As stoi is used which is a c++11 function
*/

#define GLOBALVAR	0
#define FUNCTION 	1
#define LOCALVAR	3
#define ArrayLen	100

void markCode(string code, OpCodeType ctype, 
	  int operand1, int operand2, int operand3) {
  codeArray[ICounter].opcode = code;
  codeArray[ICounter].ctype = ctype;
  codeArray[ICounter].rand1 = operand1;
  codeArray[ICounter].rand2 = operand2;
  codeArray[ICounter++].rand3 = operand3;
}

StoreNode * addName(StoreNode* old, string name, int address, int type){
	StoreNode * res = new StoreNode();
	res -> name = name;
	res -> address = address;
	res -> nodeType = type;
	res -> next = old;
	return res;
}

int search(StoreNode * start, string name){
	StoreNode * ptr = start;
	while(ptr != NULL){
		if (ptr->name.compare(name) == 0){
			return ptr->address;
		}
		ptr = ptr->next;
	}
	return 0;
}

int typeSearch(StoreNode * start, string name){
	StoreNode * ptr = start;
	while(ptr != NULL){
		if (ptr->name.compare(name) == 0){
			return ptr->nodeType;
		}
		ptr = ptr->next;
	}
	return 0;
}

bool arraySearch(StoreNode * start, string name){
	StoreNode * ptr = start;
	while(ptr != NULL){
		if (ptr->name.compare(name) == 0){
			return ptr->isArray;
		}
		ptr = ptr->next;
	}
	return false;
}

int getParamNum(TreeNode * root){
	TreeNode * ptr = root->child[0];
	int count = 0;
	while(ptr->nodeType != BLOCK){
		ptr = root->child[++count];
	}
	return count;
}

string getBoolOp(string op, int negated){
	if (op.compare("==") == 0){
		return negated > 0 ? "JEQ" : "JNE";
	} else if (op.compare("!=") == 0){
		return negated > 0 ? "JNE" : "JEQ";
	} else if (op.compare(">=") == 0){
		return negated > 0 ? "JGE" : "JLT";
	} else if (op.compare("<=") == 0){
		return negated > 0 ? "JLE" : "JGT";
	} else if (op.compare(">") == 0){
		return negated > 0 ? "JGT" : "JLE";
	} else if (op.compare("<") == 0){
		return negated > 0 ? "JLT" : "JGE";
	} else{
		cout << "Unidentified bool op " << op << "\n";
		exit(-1);
	}
}

string getArthOp(string op){
	if (op.compare("+") == 0){
		return "ADD";
	} else if (op.compare("-") == 0){
		return "SUB";
	} else if (op.compare("*") == 0){
		return "MUL";
	} else if (op.compare("/") == 0){
		return "DIV";
	} else{
		cout << "Unidentified arithmatic op " << op << "\n";
		exit(-1);
	}
}
/*
	param last represent the value passed from ancestor nodes to child nodes
	Cases used in this implementation:
	1. ! operator to indicate whether or not is negated
	2. Distinguish between getting the address or the value of an array index

*/
StoreNode * genCode(TreeNode * root, int level, StoreNode * store, int last){
	switch(root->nodeType){
		case PROGRAM:
			// Write the first line, don't know where main is yet
			markCode("LDC", RM, PC, 0, 0);
			{
				TreeNode* ptr = root->child[0];
				StoreNode * temp = store;
				int counter = 0;
				while(ptr != NULL){
					temp = genCode(root->child[counter], 0, temp, last);
					ptr = root->child[++counter];
				}
			}
			break;

		case VA_DE:
			{
				StoreNode * newStore;
				if (level == 0){
					// This is a global Variable
					newStore = store;
				} else{
					// This is a local Variable
					int isArray = 0;
					if (root->varType == INTEAR || root->varType == VOIDAR){
						// This is an array
						isArray = 1;
						// Allocate space
						markCode("LDA", RM, SP, root->val, SP);
					} else{
						// This is an atomic var

						// Allocate space
						markCode("LDA", RM, SP, 1, SP);;
					}
					
					// Put in store
					newStore = addName(store, root->id, 2 + lenOfVar, LOCALVAR);
					if (isArray){
						newStore->isArray = true;
					}
					lenOfVar += isArray ? root->val : 1;
				}
				return newStore;
			}

		case FU_DE:
			if (root->id.compare("main") == 0){
				// Main function
				mainFound = true;

				// Fill in the address we didn't know
				codeArray[last].rand2 = ICounter;
				// Intial value of fp and sp
				markCode("LDC", RM, FP, 0, 0);
				markCode("LDC", RM, SP, 2, 0);

				int blcokPos = getParamNum(root);

				// Assume Main has no parameters
				lenOfVar = 0;
				params = 0;
				inMain = true;
				genCode(root->child[blcokPos], 1, store, last);
				lenOfVar = 0;
				inMain = false;
				params = 0;
				
				// End of Main
				markCode("LDA", RM, SP, -1 * lenOfVar, SP);
				markCode("HALT", RO, 0, 0, 0);

			} else{
				// Not Main function

				// Register function name
				StoreNode * newStore = addName(store, root->id, ICounter, FUNCTION);
				
				int paramNum = getParamNum(root);

				StoreNode * ptr = newStore;
				// Link the parameters
				for (int i = 0; i < paramNum; i++){
					ptr = addName(ptr, root->child[i]->id, -1 * (paramNum - i), LOCALVAR);
				}

				// Prelude
				markCode("ST", RM, FP, 0, SP);
				markCode("LDA", RM, FP, -1, SP);
				markCode("LDA", RM, SP, 1, SP);

				lenOfVar = 0;
				params = paramNum;
				genCode(root->child[paramNum], 1, ptr, last);
				lenOfVar = 0;
				params = 0;
				
				// Postlude
				markCode("LDA", RM, SP, -1 * paramNum, FP);
				markCode("LD", RM, PC, 0, FP);

				return newStore;
			}
			break;

		case BLOCK:
			{
				int counter = 0;
				StoreNode* temp = store;
				while(root->child[counter] != NULL){
					temp = genCode(root->child[counter], 1, temp, last);
					counter++;
				}

				// Since the analyze has finished, discard inner scope
				return store;
			}

		case VAR:
			if (root->child[0] != NULL){
				// This is an array

				if (typeSearch(store, root->id) == GLOBALVAR){
					// This is a global variable

				} else{
					// This is a local variable

					// Load the index value in R0
					genCode(root->child[0], 1, store, 1);

					// Load the array address in R1
					markCode("LDA", RM, 1, search(store, root->id), FP);

					// Load the address of element
					markCode("ADD", RO, 0, 0, 1);
				}

			} else{
				// This is an atomic variable

				if (typeSearch(store, root->id) == GLOBALVAR){
					// This is a global variable

				} else{
					// This is a local variable
					markCode("LDA", RM, 0, search(store, root->id), FP);
				}
			}
			if (last){
				// We need the value
				// R0 = dem[0 + R0]
				markCode("LD",RM, 0, 0, 0);
			}

			break;

		case CONST:
			markCode("LDC", RM, 0, root->val, 0);
			break;

		case BOOLOP:
			if (root->id.compare("!") == 0){
				// Flip bit negation
				genCode(root->child[0], 1, store, last == 1 ? 0 : 1);
			} else{

				// Put first comparator in r0 by value
				genCode(root->child[0], 1, store, 1);

				// Save first comparator in SP
				markCode("ST", RM, 0, 0, SP);
				markCode("LDA", RM, SP, 1, SP);

				// Put Second comparator in R0 by value
				genCode(root->child[1], 1, store, 1);

				// Load the first comparator back
				markCode("LD", RM, 1, -1, SP);
				markCode("LDA", RM, SP, -1, SP);

				// Subtract two compators
				markCode("SUB", RO, 0, 1, 0);

				// write jump, second rand needed to be rewrite
				markCode(getBoolOp(root->id, last), RO, 0, 0, 0);
			}

			break;

		case ASSIGNTP:
			// Load the address of LHS in R0
			genCode(root->child[0], 1, store, 0);

			// Save address in SP
			markCode("ST", RM, 0, 0, SP);
			markCode("LDA", RM, SP, 1, SP);

			// Load the value in RHS in R0
			genCode(root->child[1], 1, store, 1);

			// Load the first comparator back
			markCode("LD", RM, 1, -1, SP);
			markCode("LDA", RM, SP, -1, SP);	

			// Assign value
			markCode("ST", RM, 0, 0, 1);
			break;

		case OP:
			// Load LHS by value in R0
			genCode(root->child[0], 1, store, 1);

			// Save operand in SP
			markCode("ST", RM, 0, 0, SP);
			markCode("LDA", RM, SP, 1, SP);

			// Load the value in RHS in R0
			genCode(root->child[1], 1, store, 1);

			// Load the first comparator back
			markCode("LD", RM, 1, -1, SP);
			markCode("LDA", RM, SP, -1, SP);

			// Do the operation
			markCode(getArthOp(root->id), RO, 0, 1, 0);
			break;

		case WHILETP:
		{
			// Record where we should get back
			int back = ICounter;
			int * oldPtr = whileBranch;

			int branchArray[ArrayLen];
			std::memset(branchArray, 0, sizeof(branchArray));
			whileBranch = branchArray;

			// Generate code for conditions
			genCode(root->child[0], 1, store, last);

			// Record where we branch
			int branch = ICounter - 1;

			// Generate code for body
			if (root->child[1] != NULL){
				genCode(root->child[1], 1, store, last);
			}

			// Branch back to condition check
			markCode("LDC", RM, PC, back, 0);

			// Write back the branch locations:
			codeArray[branch].rand2 = ICounter;

			// Mark break points
			for (int i = 0; i < ArrayLen; i++){
				if (branchArray[i] == 0){
					break;
				}
				codeArray[branchArray[i]].rand2 = ICounter;
			}
			whileBranch = oldPtr;
			break;
		}
		case IFTP:
		{
			// Generate code for conditions
			genCode(root->child[0], 1, store, last);

			// Record where we branch
			int branch = ICounter - 1;

			if (root->child[1] != NULL){
				genCode(root->child[1], 1, store, last);
			}

			int jumpOut = ICounter;
			// Jump out of else
			markCode("jump out of else: LDC", RM, PC, 0, 0);

			// Jump to else
			codeArray[branch].rand2 = ICounter;
			if (root->child[2] != NULL){
				genCode(root->child[2], 1, store, last);
			}

			codeArray[jumpOut].rand2 = ICounter;
			break;
		}

		case BREAK:
			for (int i = 0; i < ArrayLen; i++){
				if (*(whileBranch + i) == 0){
					*(whileBranch + i) = ICounter;
					markCode("LDC", RM, PC, 0, 0);
					break;
				}
			}
			break;

		case OUT:
			// Load Result by value
			genCode(root->child[0], 1, store, 1);

			// Gnerate Code
			markCode("OUT", RO, 0, 0, 0);
			break;

		case CALL:
			// Push param variables
			for (int i = 0; root->child[i] != NULL; i++){
				markCode("Pass parameters", RM, 0, 0, SP);
				if (root->child[i]->nodeType == VAR && arraySearch(store, root->child[i]->id)){
					// If is array, pass by address
					genCode(root->child[i], 1, store, 0);
				} else {
					genCode(root->child[i], 1, store, 1);
				}

				markCode("ST", RM, 0, 0, SP);
				markCode("LDA", RM, SP, 1, SP);
			}

			// Save return address
			markCode("LDA", RM, 0, 3, PC);
			markCode("ST", RM, 0, 0, SP);
			markCode("LDA", RM, SP, 1, SP);

			// Give control
			markCode("LDC", RM, PC, search(store, root->id) ,0);

			// Rrestore Frame Pointer
			markCode("LD", RM, FP, 1, FP);
			break;

		case RETTP:
			// Load result by value
			genCode(root->child[0], 1, store, 1);


			// Postlude
			if (inMain){
				markCode("LDA", RM, SP, -1 * lenOfVar, SP);
				markCode("HALT", RO, 0, 0, 0);
			} else{
				markCode("LDA", RM, SP, -1 * params, FP);
				markCode("LD", RM, PC, 0, FP);
			}
			break;

		default:
      		cerr << "code generation error" << endl;
	}
	return store;
}

TreeNode * program(void);
void traverse(TreeNode * node, int level);

void printCode(void) {
  for (int i = 0; i < ICounter; i++) {
    cout << i << ": " 
	 << codeArray[i].opcode << " "
	 << codeArray[i].rand1 << ","
	 << codeArray[i].rand2 
	 << (codeArray[i].ctype == RO? "," : "(")
	 << codeArray[i].rand3;
    if (codeArray[i].ctype == RM) cout << ")";
    cout << endl;
  }
}

int main(int argc, char* argv[]){
  TreeNode * root = program();
  traverse(root, 0);
  analyze(root, NULL, false, 0, 0);
  genCode(root, 0, NULL, 0);
  if (!mainFound){
  	cout << "Cannot find Main function.\n";
  	exit(-1);
  }
  printCode();
}