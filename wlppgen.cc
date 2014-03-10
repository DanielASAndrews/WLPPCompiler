#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cassert>

using std::map;
using std::pair;
using std::make_pair;

using namespace std;

const char *terminals[] = {
  "BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE", "GT", "ID",
  "IF", "INT", "LBRACE", "LE", "LPAREN", "LT", "MINUS", "NE", "NUM",
  "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN", "RPAREN", "SEMI",
  "SLASH", "STAR", "WAIN", "WHILE", "AMP", "LBRACK", "RBRACK", "NEW",
  "DELETE", "NULL"
};

typedef pair<const string , int> hash_pair;

map<const string, hash_pair> hashMap;


int isTerminal(const string &sym) {
  int idx;
  for(idx=0; idx<sizeof(terminals)/sizeof(char*); idx++)
    if(terminals[idx] == sym) return 1;
  return 0;
}

// Data structure for storing the parse tree.
class tree {
  public:
    string rule;
    string type;
    vector<string> tokens;
    vector<tree*> children;
    ~tree() { for(int i=0; i<children.size(); i++) delete children[i]; }
};

// Call this to display an error message and exit the program.
void bail(const string &msg) {
  // You can also simply throw a string instead of using this function.
  throw string(msg);
}

// Read and return wlppi parse tree.
tree *readParse(const string &lhs) {
  // Read a line from standard input.
  string line;
  getline(cin, line);


 if(cin.fail())
{
    bail("ERROR: Unexpected end of file.");
}
 tree *ret = new tree();

  // Tokenize the line.
  stringstream ss;
  ss << line;
  while(!ss.eof()) {
 
    string token;
    ss >> token;
    if(token == "") continue;
    ret->tokens.push_back(token);
  }
  // Ensure that the rule is separated by single spaces.
  for(int idx=0; idx < ret->tokens.size(); idx++) {
    if(idx>0) ret->rule += " ";
    ret->rule += ret->tokens[idx];
  }

  // Recurse if lhs is a nonterminal.
  if(!isTerminal(lhs)) {
    for(int idx=1/*skip the lhs*/; idx < ret->tokens.size(); idx++) {
      ret->children.push_back(readParse(ret->tokens[idx]));
    }
  }
  return ret;
}

//Globals

tree *parseTree;

int dclSeen = 0;
int offset = -4;
int loopNumber = 0;
string type = "";
string strInt = "int";
string strIntStar = "int*";

// Compute symbols defined in t.
void genSymbols(tree *t) {

	if (t->tokens[0] == "dcl")
	{
		dclSeen = 1;
	}

	if (t->tokens.size() > 2)
	{
		if (t->tokens[1] == "INT" && t->tokens[2] == "STAR")
                {
			type = strIntStar; 
                        t->type = strIntStar;
                }
	}
	else if (t->tokens.size() > 1)
	{
		if (t->tokens[1] == "INT")
		{
			type = strInt;
			t->type = strInt;
		}
	}

	if (t->tokens[0] == "ID")
	{

		map<const string, hash_pair>::iterator idExists = hashMap.find(t->tokens[1]);

		if (idExists == hashMap.end() && dclSeen == 1)
		{
			hashMap.insert( make_pair( t->tokens[1], make_pair(type,offset) ) );
			dclSeen = 0;
			type = "";
			offset -= 4;
		}
		else if (idExists == hashMap.end() && dclSeen == 0)
		{
			string problem = "ERROR: " + t->tokens[1] + " was not declared";
                	bail(problem);
		}

		if (idExists != hashMap.end() && dclSeen == 1)
		{
			string problem = "ERROR: " + t->tokens[1] + " was already declared";
                        bail(problem);
		}
         
	}

       // Recurse if children still exist.
  	if(t->children.size() != 0) {

		for(int idx=0; idx < t->children.size(); ++idx) 
                {
                        genSymbols(t->children[idx]);
                }
    	}
  }

string retu = "";

void printTree(tree *t)
{
	retu = retu + t->rule + "\n";

	if(!isTerminal(t->tokens[0]))
	{
    		for(int idx=0; idx < t->children.size(); idx++)
		{
      			printTree(t->children[idx]);
    		}
	}
}

//Basic Code Generators

void add(int reg1, int reg2, int reg3)
{ cout << "add $" << reg1  << ", $"<< reg2 << ", $" << reg3 << endl; }

void sub(int reg1, int reg2, int reg3)
{ cout << "sub $" << reg1  << ", $"<< reg2 << ", $" << reg3 << endl; }

void lis(int reg)
{ cout << "lis $" << reg << endl; }

void word(int val)
{ cout << ".word " << val << endl; }

void word(string label)
{ cout << ".word " << label << endl; }

void lw(int reg1, int offset, int reg2)
{ cout << "lw $" << reg1 << ", " << offset << "($" << reg2 << ")" << endl; }

void sw(int reg1, int offset, int reg2)
{ cout << "sw $" << reg1 << ", " << offset << "($" << reg2 << ")" << endl; }

void jr(int reg)
{ cout << "jr $" << reg << endl; }

void pop(int reg)
{
	cout << ";Pop reg " << reg << endl;
    add(30, 30, 4);
    lw(reg, -4, 30);
}

void push(int reg)
{
	cout << ";Push reg " << reg << endl;
	sw(reg, -4, 30);
    sub(30, 30, 4);
}

void mult(int reg1, int reg2)
{ cout << "mult $" << reg1 << ", $" << reg2 << endl; }

void divi(int reg1, int reg2)
{ cout << "div $" << reg1 << ", $" << reg2 << endl; }

void mflo (int reg)
{ cout << "mflo $" << reg << endl; }

void mfhi (int reg)
{ cout << "mfhi $" << reg << endl; }

void jalr(int reg)
{ cout << "jalr $" << reg << endl; }

void slt(int reg1, int reg2, int reg3)
{ cout << "slt $" << reg1 << ", $" << reg2 << ", $" << reg3 << endl; }

void sltu(int reg1, int reg2, int reg3)
{ cout << "sltu $" << reg1 << ", $" << reg2 << ", $" << reg3 << endl; }

void lt(int reg1, int reg2, int reg3, string type)
{ cout << ";" << reg2 << " < " << reg3 << " ?" << endl;
	if (type == strInt)
	{
		slt(reg1, reg2, reg3);
	}
	else
	{
		sltu(reg1, reg2, reg3);
	}
}

void gt(int reg1, int reg2, int reg3, string type)
{ cout << ";" << reg2 << " > " << reg3 << " ?" << endl;
  cout << "; So check negation of lt" << endl;
  lt(reg1, reg3, reg2, type);
}

void lte(int reg1, int reg2, int reg3, string type)
{ // Less than or Equal to is the negation of greater than
  cout << ";" << reg2 << " =< " << reg3 << " ?" << endl;
  cout << "; So check negation of gt" << endl;
  gt(reg1, reg2, reg3, type);
  sub(3, 11, 3);
}

void gte(int reg1, int reg2, int reg3, string type)
{ // Greater than or Equal to is the negation of less than
  cout << ";" << reg2 << " >= " << reg3 << " ?" << endl;
  cout << "; So check negation of lt" << endl;
  lt(reg1, reg2, reg3, type);
  sub(3, 11, 3);
}

void ne(int reg1, int reg2, string type)
{ cout << ";" << reg1 << " != " << reg2 << " ?" << endl;
	if(type == strInt)
	{
		slt(6, reg1, reg2);
  		slt(7, reg2, reg1);
	}
	else
	{
		sltu(6, reg1, reg2);
                sltu(7, reg2, reg1);
	}
  	//Will $3 will be 1 if NE, 0 if EQ
  	add(3, 6, 7);
}

void eq(int reg1, int reg2, string type)
{ cout << ";" << reg1 << " == " << reg2 << " ?" << endl;
  cout << "; So check negation of ne" << endl;
  ne(reg1, reg2, type);
 //Negate ne by subtracting 1, so if EQ $3 will be -1, 0 if NE
  sub(3, 11, 3);
}

void beq(int reg1, int reg2, string lbl)
{ cout << "beq $" << reg1 << ", $" << reg2 << ", " << lbl << endl; }

//Type Generators

//predeclarations

string termType(tree* t);
string lValueType(tree* t);
string idType(tree* t);

string exprType(tree* t)
{
	if (t->children.size() == 1)
	{
		string type = termType(t->children[0]);
		t->type = type;
		return type;
	}
	else if (t->children.size() == 3)
	{
		string exprType1 = exprType(t->children[0]);
		string termType1 = termType(t->children[2]);

			if (t->tokens[2] == "PLUS")
			{
				if (exprType1 == strInt && termType1 == strInt)
				{
					t->type = strInt;
					return strInt;
				}
				else if (exprType1 == strIntStar && termType1 == strInt)
				{
					t->type = strIntStar;
					return strIntStar;
				}
				else if (exprType1 == strInt && termType1 == strIntStar)
				{
					t->type = strIntStar;
					return strIntStar;
				}
			}
			else if (t->tokens[2] == "MINUS")
			{
				if (exprType1 == strInt && termType1 == strInt)
				{
					t->type = strInt;
					return strInt;
				}
				else if(exprType1 == strIntStar && termType1 == strInt)
				{
					t->type = strIntStar;
					return strIntStar;
				}
				else if (exprType1 == strIntStar && termType1 == strIntStar)
				{
					t->type = strInt;
					return strInt;
				}
			}

	}
	string tmp = "";
    	return tmp;
}

string factorType(tree* t)
{
    	if (t->children.size() == 1)
	{
    		if (t->tokens[1] == "ID")
		{
    			string type = idType(t->children[0]);
    			t->type = type;
    			return type;
    		}
    		else if (t->tokens[1] == "NUM")
		{
    			t->type = strInt;
    			return strInt;
    		}
    		else if (t->tokens[1] == "NULL")
		{
    			t->type = strIntStar;
    			return strIntStar;

    		}
    	}
    	else if (t->children.size() == 2)
	{
    		if (t->tokens[1] == "AMP")
		{
    			t->type = strIntStar;
    			string type = lValueType(t->children[1]);
    			if (type == strInt)
			{
				return strIntStar;
			}
    		}
    		else if (t->tokens[1] == "STAR")
		{
    			t->type = strInt;
    			string type = factorType(t->children[1]);
    			if (type == strIntStar)
			{
    				return strInt;
			}
    		}
    	}
    	else if (t->children.size() == 3)
	{
    		string type =  exprType(t->children[1]);
    		t->type = type;
    		return type;
    	}
    	else if (t->children.size() == 5)
	{
    		string type = exprType(t->children[3]);
    		t->type = strIntStar;
    		if (type == strInt)
		{
    			return strIntStar;
    		}

    	}
	string tmp = "";
    	return tmp;
}

string termType(tree* t)
{
	if (t->children.size() == 1)
	{
    		string type = factorType(t->children[0]);
    		t->type = type;
    		return type;
    	}
    	else if (t->children.size() == 3)
	{
		string termType1 = termType(t->children[0]);
    		string factorType1 = factorType(t->children[2]);
		t->type = strInt;

    		if (termType1 == strInt && factorType1 == strInt)
		{
    			return strInt;
		}
    	}
	string tmp = "";
    	return tmp;
}

string lValueType(tree* t)
{
	if (t->children.size() == 1)
	{
		string type = idType(t->children[0]);
		t->type = type;
		return type;
	}
	else if (t->children.size() == 2)
	{
		string type = factorType(t->children[1]);
		t->type = strInt;

		if (type == strIntStar)
		{
			return strInt;
		}
	}
	else if (t->children.size() == 3)
	{
		string type = lValueType(t->children[1]);
		t->type = type;
    		return type;
	}
    	string tmp = "";
	return tmp;
}

string idType(tree* t)
{
	 map<const string, hash_pair>::iterator it;
         it = hashMap.find(t->tokens[1]);
         string key = it->first;
         string type = hashMap[key].first;
         return type;
}

//Sub-Generators

int genID(tree* t)
{
	 map<const string, hash_pair>::iterator it;
         it = hashMap.find(t->tokens[1]);
         string key = it->first;
         int offset = hashMap[key].second;
         return offset;
}

void genDcl(tree* t, int reg)
{
	//dcl -> type ID
                if(t->tokens[2] == "ID")
                {
			int offset = genID(t->children[1]);
			sw(reg, offset, 30);
			return;
                }
}


//pre-declarations
void genExpr(tree* t);
int genLValue(tree* t);

void genFactor(tree* t)
{
        //factor -> terminal
        if (t->children.size() == 1)
        {
                //factor -> ID
                if (t->tokens[1] == "ID")
                {
	
                        int offset = genID(t->children[0]);
                        lw(3, offset, 29);
                        return;
                }
		//factor -> NUM
		else if (t->tokens[1] == "NUM")
		{
	
			string strValue = t->children[0]->tokens[1];
			int value = atoi(strValue.c_str());
			lis(3);
			word(value);
			return;
		}
		//factor -> NULL
		else if (t->tokens[1] == "NULL")
		{
	
			add(3, 0, 0);
			return;
		}
        }
	//factor -> STAR expr
	//factor -> AMP lvalue
	else if (t->children.size() == 2)
	{
		 //factor -> STAR expr
		if (t->tokens[1] == "STAR")
		{
			cout << ";factor STAR factor" << endl;
			genFactor(t->children[1]);
			lw(3, 0, 3);
			return;
		}
		//factor -> AMP lvalue
		else if (t->tokens[1] == "AMP")
		{
			if (t->children[1]->children.size() == 2)
			{
				genFactor(t->children[1]->children[1]);
			}
			else
			{
			int offset = genLValue(t->children[1]);
			lis(3);
			word(offset);
			add (3,29,3);
			return;
			}
		}
	}
	//factor -> LPAREN expr RPAREN
	else if (t->children.size() == 3)
	{
		//expr
		if (t->tokens[2] == "expr")
		{
			genExpr(t->children[1]);
			return;
		}
	}
	//factor -> NEW INT LBRACK expr RBRACK
	else if (t->children.size() == 5)
	{
		genExpr(t->children[3]);
		add(1, 3, 0);
		lis(12);
		word("new");
		jalr(12);
		return;
	}

}

void genTerm(tree* t)
{
        //term -> factor
        if (t->children.size() == 1)
        {
                cout << ";term to factor" << endl;
		genFactor(t->children[0]);
                return;
        }
	else if (t->children.size() == 3)
	{
		genTerm(t->children[0]);
		push(3);
		genFactor(t->children[2]);
		pop(5);

		if (t->tokens[2] == "STAR"){
			cout << "; STAR" << endl;
    			mult(5,3);
    			mflo (3);
    		}
    		else if (t->tokens[2] == "SLASH"){
    			cout << "; SLASH" << endl;
			divi (5,3);
    			mflo (3);
    		}
    		else if (t->tokens[2] == "PCT"){
    			cout << "; PCT" << endl;
			divi (5,3);
    			mfhi(3);
    		}
	return;
	}
}

void genExpr(tree* t)
{
        //expr -> term
        if (t->children.size() == 1)
        {
		cout << ";expr to term" << endl;
                genTerm(t->children[0]);
                return;
        }
	else if (t->children.size() == 3)
	{
		genExpr(t->children[0]);
                push(3);
                genTerm(t->children[2]);
                pop(5);

		string typeExpr = exprType(t->children[0]);
		string typeTerm = termType(t->children[2]);

		if (t->tokens[2] == "PLUS")
		{
			if (typeExpr == strIntStar)
			{
				mult(3, 4);
				mflo(3);
			}
			else if (typeTerm == strIntStar)
			{
				mult(5, 4);
				mflo(5);
			}
			cout << "; PLUS" << endl;
			add(3, 5, 3);
		}
		else if (t->tokens[2] == "MINUS")
                {
			if (typeExpr == strIntStar && typeTerm == strInt)
			{
				cout << "; int* - int" << endl;
				mult(3, 4);
				mflo(3);
			}
			cout << "; MINUS" << endl;
			sub(3, 5, 3);

			if (typeExpr == strIntStar && typeTerm == strIntStar)
			{
				cout << "; int* - int*" << endl;
				divi(3, 4);
				mflo(3);
			}
		}
	return;
	}
}

void genReturn()
{
    lw(31, (hashMap.size() * -4) - 4, 29);
    jr(31);
}

int genLValue(tree* t)
{
	if(t->children.size() == 3)
	{
		genLValue(t->children[1]);
	}
	else if (t->children.size() == 1)
	{
		int offset = genID(t->children[0]);
		return offset;
	}
	//lvalue -> STAR factor
	else if (t->children.size() == 2)
	{
		cout << "; lvalue -> STAR factor" << endl;
		genFactor(t->children[1]);
		return 0;
	}
}

//predeclaration

void genStatements(tree* t);
void genTest(tree* t);

void genStatement(tree* t)
{
        //statement -> PRINTLN LPAREN expr RPAREN SEMI
        if (t->tokens[1] == "PRINTLN")
        {
                genExpr(t->children[2]);
	//	push(1);    //Originally had this to store reg1, but it may not be neccessary since we stored it in the beginning
                add(1, 3, 0);
                push(31);
                cout << "; Load print program" << endl;
		lis(10);
                word("print");
                jalr(10);
		pop(31);
	//	pop(1);
                return;
        }
	else if (t->tokens[2] == "BECOMES")
	{
		//int* BECOMES expr SEMI
		if (t->children[0]->children.size() == 2)
		{
			cout << "; we reach int* becomes expr semi" << endl;
			int offset = genLValue(t->children[0]); 
			push(3);
			genExpr(t->children[2]);
			pop(5);
			sw(3, 0, 5);
			return;
		}
		//int BECOMES expr SEMI
		else
		{
			genExpr(t->children[2]);
                	int offset = genLValue(t->children[0]); 
                	sw(3, offset, 29);
                	return;
		}
	}
	else if (t->tokens[1] == "WHILE")
	{
		string strOutput;
    		int currentLoop = loopNumber;
    		loopNumber++;
		stringstream ss;
		// loopX:
		ss << "loop" << currentLoop << ":";
		ss >> strOutput;
		cout << strOutput << endl;
		ss.str("");
		ss.clear();
		strOutput = "";
        	genTest(t->children[2]);
		// doneX
		ss << "done" << currentLoop;
		ss >> strOutput;
        	beq(3, 0, strOutput);
		ss.str("");
                ss.clear();
		strOutput = "";
        	genStatements(t->children[5]);
		// loopX
		ss << "loop" << currentLoop;
                ss >> strOutput;
        	beq(0, 0, strOutput);
		ss.str("");
                ss.clear();
		strOutput = "";
        	// doneX:
		ss << "done" << currentLoop << ":";
                ss >> strOutput;
        	cout << strOutput << endl;
		return;
	}
	else if (t->tokens[1] == "IF")
        {
                string strOutput;
                int currentLoop = loopNumber;
                loopNumber++;
                stringstream ss;
                // elseX
                ss << "else" << currentLoop;
                ss >> strOutput;
                genTest(t->children[2]);
                beq(3, 0, strOutput);
		ss.str("");
                ss.clear();
                strOutput = "";
		genStatements(t->children[5]);
		// endifX
                ss << "endif" << currentLoop;
                ss >> strOutput;
		beq(0, 0, strOutput);
		ss.str("");
                ss.clear();
                strOutput = "";
		// elseX:
                ss << "else" << currentLoop << ":";
                ss >> strOutput;
		cout << strOutput << endl;
		genStatements(t->children[9]);
		ss.str("");
                ss.clear();
                strOutput = "";
		// endifX:
                ss << "endif" << currentLoop << ":";
                ss >> strOutput;
                cout << strOutput << endl;
                return;
        }
	//statement -> DELETE LBRACK RBRACK expr SEMI
	else if (t->children.size() == 5)
	{
		genExpr(t->children[3]);
		add(1, 3, 0);
		lis(12);
		word("delete");
		jalr(12);
		return;
	}
}


void genStatements(tree* t)
{
	//statements -> statements statement
	if (t->children.size() == 2)
	{
		genStatements(t->children[0]);
		genStatement(t->children[1]);
		return;
	}
	return;
}

void genDcls(tree* t)
{
	if (t->children.size() == 5)
	{
		genDcls(t->children[0]);

		//dcl BECOMES NUM SEMI
		if (t->tokens[4] == "NUM")
		{
			//cout << ";dcl BECOMES NUM SEMI" << endl;
                        string strValue = t->children[3]->tokens[1];
			//cout << "This is strValue: " << strValue << endl;
                        int value = atoi(strValue.c_str());
                        lis(5);
                        word(value);
			genDcl(t->children[1], 5);
		}
		else if (t->tokens[4] == "NULL")
		{
			cout << ";dcl BECOMES NULL SEMI" << endl;
			add(5, 0, 0);
			genDcl(t->children[1], 5);
		}
		return;
	}
	return;
}

void genTest(tree* t)
{
	genExpr(t->children[0]);
	push(3);
	genExpr(t->children[2]);
	pop(5);

	string typeExpr = exprType(t->children[0]);

		// 5 is the first expression, 3 is the second expression.
		if(t->tokens[2] == "LT")
		{
			lt(3, 5, 3, typeExpr);
		}
		else if(t->tokens[2] == "GT")
		{
			gt(3, 5, 3, typeExpr);
		}
		else if(t->tokens[2] == "GE")
		{
			gte(3, 5, 3, typeExpr);
		}
		else if(t->tokens[2] == "LE")
		{
			lte(3, 5, 3, typeExpr);
		}
		else if(t->tokens[2] == "NE")
		{
			ne(3, 5, typeExpr);
		}
		else if(t->tokens[2] == "EQ")
		{
			eq(3, 5, typeExpr);
		}
	return;
}

// Generate the code for the parse tree t.
void genCode(tree *t)
{

//Conventions:

	//$1 1st param
	//$2 2nd param
	//$3 output of a subexpression
	//$4 contains 4
	//$5 is used as a temp register for binary calculations
	//$11 is 1
	//$10 has address for print
	//$12 has address for init/new/delete when neccessary
	//$29 is used as a frame pointer
	//all vars (incl params) are placed on stack, and their offsets computed w.r.t frame pointer

	//imports
	cout << ";imports" << endl;
	cout << ".import print" << endl;
	cout << ".import init" << endl;
	cout << ".import new" << endl;
	cout << ".import delete" << endl;

	lis(4);
    	word(4);

	//store print label
	lis(10);
	word("print");

    	lis(11);
    	word (1);
	//initialize stack frame pointer
    	add (29,30,0);

	// rule: factor -> id
	//if (t->token[0] == "factor" && t->children.size() == 0){add

	tree* ret = parseTree->children[1];

	genDcl(ret->children[3], 1);
	genDcl(ret->children[5], 2);
	genDcls(ret->children[8]);

	// Sutract and extra -4, because we sw 31 earlier
	int totalOffset = (hashMap.size() * -4) - 4;
	sw (31, totalOffset, 30);
	lis(5);
	word(totalOffset);
	add(30,30,5);

	lw(1, -4, 29);
	lw(2, -8, 29);

	//prep for init, $2 must be 0
	if (ret->children[3]->children[0]->children.size() == 1)
	{
		add(2, 0, 0);
    	}

	//init
	lis(12);
	word("init");
	jalr(12);

	genStatements(ret->children[9]);
	genExpr(ret->children[11]);
	genReturn();
}


int main()
{

  try {
    parseTree = readParse("S");

    genSymbols(parseTree);

    genCode(parseTree);
  } catch(string msg) {
    cerr << msg << endl;
  }
  if (parseTree) delete parseTree;

return 0;

}
