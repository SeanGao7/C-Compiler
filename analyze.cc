#include <iostream>
#include "globals.h"

#define A_ROOT	-1
#define A_VAR	0
#define A_FUNC	1
/*
To compile:
g++ scan.cc parse.cc analyze.cc -std=c++11
As stoi is used which is a c++11 function
*/

ListNode* newListNode(int nodeType){
	ListNode* res;
	res = new ListNode();
	res->nodeType = nodeType;
}

bool localSearch(string name, ListNode* start){
	ListNode* ptr = start;
	while (ptr != NULL && ptr->nodeType != A_ROOT){
		if(name.compare(ptr->name) == 0){
			return true;
		}
		ptr = ptr->next;
	}
	return false;
}

bool globalSearch(string name, ListNode* start){
	ListNode* ptr = start;
	while (ptr != NULL){
		if(name.compare(ptr->name) == 0){
			return true;
		}
		ptr = ptr -> next;
	}
	return false;
}

ListNode* findFunc(ListNode* store, string name){
	ListNode* ptr = store;
	while(ptr != NULL){
		if (ptr->nodeType == A_FUNC && name.compare(ptr->name) == 0){
			return ptr;
		}
		ptr = ptr -> next;
	}
	return NULL;
}


int checkVar(ListNode* store, string name, int expectedType){
	ListNode* ptr = store;
	while (ptr != NULL){
		if(ptr->nodeType == A_VAR && name.compare(ptr->name) == 0){
			if (expectedType == 0 || expectedType == ptr->symbolType){
				return ptr->symbolType;
			}else{
				return -1;
			}
		}
		ptr = ptr -> next;
	}
	return -1;
}

// Utility Functions borrowed from parse.cc
void indent(int level);
string getType(int type);

ListNode* analyze(TreeNode* node, ListNode* parent, bool loop, int level, int expectedType){
	switch(node->nodeType){

		case PROGRAM:
		{
			TreeNode* ptr = node->child[0];
			ListNode* last = parent;
			int counter = 0;
			while(ptr != NULL){
				last = analyze(ptr, last, loop, level, 0);
				ptr = node->child[++counter];
			}
			cout << "Analysis Complete with No errors\n";
			return last;
		}


		case VA_DE:
		{
			if (localSearch(node->id, parent)){
				cout <<"Name " << node->id << " has been defined in the scope.\n";
				exit(-1);
			}
			ListNode* res = newListNode(A_VAR);
			res -> name = node->id;
			res -> symbolType = node -> varType;
			res -> next = parent;
			indent(level);
			cout << "Variable decalration: " << node->id << " (Type: " << getType(node->varType) << ") \n";
			return res;
		}

		case FU_DE:
		{
			if (globalSearch(node->id, parent)){
				cout << "Name " << node->id << " has been defined in the scope.\n";
				exit(-1);
			}
			ListNode* res = newListNode(A_FUNC);
			res->name = node->id;
			res->symbolType = node-> varType;
			res -> next = parent;
			ListNode* last = newListNode(A_ROOT);
			last -> next = res;

			TreeNode* ptr = node->child[0];

			int counter = 0;

			indent(level);
      		cout << "Function decalration: " << res->name << " (returnType: " << getType(res->symbolType) << ")\n";

			while(ptr->nodeType != BLOCK){
				// Add the param to function param list
				res->param[counter] = ptr -> varType;

				// Decalre them as local variables
				if (localSearch(ptr->id, last)){
					cout << "Name " << ptr->id << " has been defined in the scope.\n";
					exit(-1);
				}
				ListNode* child = newListNode(A_VAR);
				child -> name = ptr->id;
				child -> symbolType = ptr -> varType;
				child -> next = last;
				indent(level + 1);
				cout << "Parameter Variable decalration: " << child->name << " (Type: " << getType(child->symbolType) << ") \n";
				last = child;
				ptr = node->child[++counter];
			}

			counter = 0;

			// Handling decalrations inside the bock
			while(ptr->child[counter] != NULL){
				last = analyze(ptr->child[counter], last, loop, level + 1, 0);
				counter++;
			}
			return res;
		}

		case BLOCK:
		{
			// Insert a stop node to mark the boundary between inner / outter scope
			ListNode* res = newListNode(A_ROOT);
			res -> next = parent;
			int counter = 0;
			ListNode* last = res;
			while(node->child[counter] != NULL){
				last = analyze(node->child[counter], last, loop, level + 1, 0);
				counter++;
			}
			// Since the analyze has finished, discard inner scope
			return parent;
		}
		
		case VAR:
		{
			int match = -1;
			// Is an array index access
			if (node -> child[0] != NULL){
				// Check if the access part is numerical
				analyze(node -> child[0], parent, loop, level, INTE);
				match = checkVar(parent, node->id, expectedType == 0 ? 0 : expectedType + 2);
			} else{
				match = checkVar(parent, node->id, expectedType);
			}
			if (match == -1){
				cout << "Unmatched type for Variable " << node->id << "\n";
				exit(-1);
			}
			return parent;
		}

		case CONST:
			if (expectedType != 0 && expectedType != INTE){
				cout << "Unexpected numerical expression. \n";
				exit(-1);
			}
			return parent;

		case BOOLOP:
		{
			for (int i = 0; node->child[i] != NULL; i++){
				analyze(node->child[i], parent, loop, level, INTE);
			}
			return parent;
		}

		case ASSIGNTP:
		{
			// An assign always has and only has a left hand side and a right hand side
			// Match represent the type of LHS expression
			int match = -1;
			// Is an array index access
			if (node -> child[0] ->child[0] != NULL){
				// Check if the access part is numerical
				analyze(node -> child[0] -> child[0], parent, loop, level, INTE);
				// Looking for array
				match = checkVar(parent, node->child[0]->id, expectedType == 0 ? 0 : expectedType + 2);
				match -= 2;
			} else{
				match = checkVar(parent, node->child[0]->id, expectedType);
			}
			if (match < 0){
				cout << "Unmatched type for Variable " << node->child[0]->id << "\n";
				cout << expectedType;
				exit(-1);
			}
			analyze(node->child[1], parent, loop, level, match);

			return parent;
		}

		case OP:
		{
			if (expectedType != 0 && expectedType != INTE){
				cout << "Unexpected numerical expression. \n";
				exit(-1);
			}
			// Arithmatic operations are always binary so no need for loops
			analyze(node->child[0], parent, loop, level, INTE);
			analyze(node->child[1], parent, loop, level, INTE);
			return parent;
		}

		case WHILETP:
		{
			for (int i = 0; node->child[i] != NULL; i++){
				analyze(node->child[i], parent, true, level, 0);
			}
			return parent;
		}

		case IFTP:
		{
			for (int i = 0; node->child[i] != NULL; i++){
				analyze(node->child[i], parent, loop, level, 0);
			}
			return parent;
		}

		case BREAK:
			if (!loop){
				cout << "Break occur outside loop\n";
				exit(-1);
			}
			return parent;

		case OUT:
		{
			for (int i = 0; node->child[i] != NULL; i++){
				analyze(node->child[i], parent, loop, level, 0);
			}
			return parent;
		}

		case CALL:
		{
			ListNode* func = findFunc(parent, node->id);
			// Check exisitence
			if (func == NULL){
				cout << "Cannot find function named " << node->id << "\n";
				exit(-1);
			}
			// Check if had expercted return type
			if (expectedType != 0 && func->symbolType != expectedType){
				cout << "Function " << node->id << " with Unexpected return type " << getType(func -> symbolType) << " found.\n";
				exit(-1);
			}
			// Check parameter list
			int i = 0;
			for (; func->param[i] != 0; i++){
				if (node->child[i] == NULL){
					cout << "Inconsistent function list between function decalration and function call in " << func->name << "\n";
					exit(-1);
				}
				analyze(node->child[i], parent, loop, level, func->param[i]);
			}
			if (node->child[i] != NULL && node->child[i]->nodeType != BLOCK){
				cout << "Inconsistent function list between function decalration and function call in " << func->name << "\n";
				exit(-1);
			}
			return parent;
		}

		case RETTP:
		{
			for (int i = 0; node->child[i] != NULL; i++){
				analyze(node->child[i], parent, loop, level, 0);
			}
			return parent;
		}

		default:
			return parent;
	}
}
