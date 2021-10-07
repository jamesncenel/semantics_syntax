#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "blank.h"
#include <string>
#include <sstream>

using namespace std;

std::string returnExpression1;
std::string returnExpression2;
int maximumLineNumberReached;
int pOrder = 0;

blank l;
std::string errorMessage = "Syntax Error\n";
Token checker;
bool parse_input(); //NOT DONE?
bool parse_program(); //DONE
bool parse_scope(); //DONE
bool parse_scope_list(); //DONE
bool parse_var_decl(); //DONE
bool parse_id_list(); //DONE
bool parse_type_name(); //DONE
bool parse_smt(); //NOT DONE
bool parse_assign_smt(); //DONE
bool parse_expr(); //DONE
bool parse_arithmetic_operator(); //DONE
bool parse_binary_boolean_operator(); //DONE
bool parse_relational_operator(); //DONE
bool parse_primary(); //DONE
bool parse_bool_const(); //DONE
bool parse_while_smt(); //DONE
bool parse_condition(); //DONE
bool parse_smt_list(); //DONE

int seenASemanticError = 0;
int semanticErrorType = -1;
int numOfUnitializedRef = 0;
int currentScope = 0;
std::string stringDump;
std::string nameOfVarDeclaredMoreThanOnce;
std::string nameOfUndeclaredVariable;
std::string nameOfVariableDeclaredButNotUsed;
int lineOnWhichTheErrorTookPlace = 0;
int intDump;
//ERROR TYPES:
//

struct eNode
{
    int value;
    std::string badVar;
    int lineNumber;
    eNode *next;
};
class errorList
{
public:
    eNode *head;
    eNode *tail;
    errorList()
    {
        head = NULL;
        tail = NULL;
    }
    void addE(int erNum, int lineOn, std::string theOne)
    {
        eNode *temp = new eNode;
        temp->value = erNum;
        temp->lineNumber = lineOn;
        temp->badVar = theOne;
        temp->next = NULL;
        if(head == NULL)
        {
            head = temp;
            tail = temp;
        }
        else{
            tail->next = temp;
            tail = tail->next;
        }
    }
};


struct varName
{
    std::string name;
    varName *next;
};

class tempVarList
{
public:
    varName *head;
    varName *tail;
    tempVarList()
    {
        head = NULL;
        tail = NULL;
    }
    void addVar(std::string c)
    {
        varName *temp = new varName;
        temp->name = c;
        temp->next = NULL;
        if(head == NULL)
        {
            head = temp;
            tail = temp;
        }
        else{
            tail->next = temp;
            tail = tail->next;
        }
    }
};

struct scope
{
    int number;
    int isDone = 0;
    scope *next;
};

class scopeList
{
public:
    scope *head;
    scope *tail;
    int maxScope;
    scopeList()
    {
        head = NULL;
        tail = NULL;
    }
    void addScope(int n)
    {
        scope *temp = new scope;
        temp->number = n;
        temp->isDone = 0;
        temp->next = NULL;
        if(head == NULL)
        {
            head = temp;
            tail = temp;
        }
        else{
            tail->next = temp;
            tail = tail->next;
        }
    }
    void deleteItem(int n)
    {
        scope *temp = head;
        while(temp != NULL)
        {
            if(temp->number == n && temp->isDone == 0)
            {
                temp->isDone = 1;
                break;
            }
            temp = temp->next;
        }
    }

};


struct lineNum
{
    int value;
    int higherOrder;
    lineNum *next;
};

class lineNoList
{
public:
    lineNum *head;
    lineNum *tail;
    lineNoList()
    {
        head = NULL;
        tail = NULL;
    }
    void add_line(int n, int higherOrderTemp)
    {
        lineNum *temp = new lineNum;
        temp->value = n;
        temp->higherOrder = higherOrderTemp;
        temp->next = NULL;
        if(head == NULL)
        {
            head = temp;
            tail = temp;
        }
        else{
            tail->next = temp;
            tail = tail->next;
        }
    }
    void deleteList()
    {
        lineNum *temp = head;
        lineNum *otherTemp;
        while (temp != NULL)
        {
            otherTemp = temp->next;
            delete temp;
            temp = otherTemp;
        }

    }
};

struct node
{
    int scopeNum;
    int declarationLineNum;
    int isInitialized;
    int isReferenced;
    std::string type;
    std::string name;
    node *next;
    lineNoList *lineList;
    int outOfTheRunning;
};

class varList
{
public:
    node *head;
    node *tail;
    varList()
    {
        head = NULL;
        tail = NULL;
    }

    void add_Var(int scope, int line, std::string typeD, std::string givenName)
    {
        node *temp = new node;
        temp->scopeNum = scope;
        temp->declarationLineNum = line;
        temp->type = typeD;
        temp->name = givenName;
        temp->next = NULL;
        temp->isInitialized = 0;
        temp->isReferenced = 0;
        temp->lineList = new lineNoList;
        temp->outOfTheRunning = 0;
        if (head == NULL)
        {
            head = temp;
            tail = temp;
        }
        else {
            tail->next = temp;
            tail = tail->next;
        }
    }

    void updateRunning(int scopeNum)
    {
        node *temp = head;
        while(temp != NULL)
        {
            if(temp->scopeNum == scopeNum)
            {
                temp->outOfTheRunning = 1;
            }
            temp = temp->next;
        }
    }

    void deleteList()
    {
        node *temp = head;
        node *otherTemp;
        while (temp != NULL)
        {
            otherTemp = temp->next;
            delete temp;
            temp = otherTemp;
        }

    }
};


struct lineNumDet
{
    int value;
    int higherOrder;
    std::string name;
    int decl;
    lineNumDet *next;
};

class lineNoDetList
{
public:
    lineNumDet *head;
    lineNumDet *tail;
    lineNoDetList()
    {
        head = NULL;
        tail = NULL;
    }
    void add_line(int n, int higherOrderTemp, std::string gName, int d)
    {
        lineNumDet *temp = new lineNumDet;
        temp->value = n;
        temp->higherOrder = higherOrderTemp;
        temp->name = gName;
        temp->decl = d;
        temp->next = NULL;
        if(head == NULL)
        {
            head = temp;
            tail = temp;
        }
        else{
            tail->next = temp;
            tail = tail->next;
        }
    }
    void deleteList()
    {
        lineNumDet *temp = head;
        lineNumDet *otherTemp;
        while (temp != NULL)
        {
            otherTemp = temp->next;
            delete temp;
            temp = otherTemp;
        }

    }
};


//error type:
//Code 1.1 -> variable declared more than once DONE
//Code 1.2 -> Undeclared variable DONE
//Code 1.3 -> Variable declared, but not used DONE

//C1 -> if LHS of an assign statement is INT BOOL or String, the right side needs to match Working
//C2 -> if LHS of an assign statement is REAL, then the right needs to be REAL or INT
//C3 -> Operands of the arithmetic operator need to be REAL or INT DONE
//C4 -> operands of Binary Boolean Operator need to be bOOLEAN DONE
//C5 -> If operands of relational operators are neither INT nor String, then they should be both be Boolean or both be String DONE
//C6 -> If one of the operands is an INT or REAL, the other must also be an INT or REAL DONE
//C7 -> The type of a condition must be a boolean
//C8 -> The type of the operand for a NOT must be Boolean

//UNIT -> List uninitialized variables that are used like they have been initialized


void printSemanticError(eNode *chosenErr)
{
    if(chosenErr->value == 1)
    {
        cout<<"ERROR CODE 1.1 "<<chosenErr->badVar<<"\n";
        exit(1);
    }
    else if(chosenErr->value == 2)
    {
        cout<<"ERROR CODE 1.2 "<<chosenErr->badVar<<"\n";
        exit(1);
    }
    else if(chosenErr->value == 3)
    {
        cout<<"ERROR CODE 1.3 "<<chosenErr->badVar<<"\n";
        exit(1);
    }
    else if(chosenErr->value == 4)
    {
        cout<<"TYPE MISMATCH "<<chosenErr->lineNumber<<" C1\n";
        exit(1);
    }
    else if(chosenErr->value == 5)
    {
        cout<<"TYPE MISMATCH "<<chosenErr->lineNumber<<" C2\n";
        exit(1);
    }
    else if(chosenErr->value == 6)
    {
        cout<<"TYPE MISMATCH "<<chosenErr->lineNumber<<" C3\n";
        exit(1);
    }
    else if(chosenErr->value == 7)
    {
        cout<<"TYPE MISMATCH "<<chosenErr->lineNumber<<" C4\n";
        exit(1);
    }
    else if(chosenErr->value == 8)
    {
        cout<<"TYPE MISMATCH "<<chosenErr->lineNumber<<" C5\n";
        exit(1);
    }
    else if(chosenErr->value == 9)
    {
        cout<<"TYPE MISMATCH "<<chosenErr->lineNumber<<" C6\n";
        exit(1);
    }
    else if(chosenErr->value == 10)
    {
        cout<<"TYPE MISMATCH "<<chosenErr->lineNumber<<" C7\n";
        exit(1);
    }
    else if(chosenErr->value == 11)
    {
        cout<<"TYPE MISMATCH "<<chosenErr->lineNumber<<" C8\n";
        exit(1);
    }
    else{
        exit(1);
    }
}


varList vList;
scopeList sList;
tempVarList dumpList;
varName *chaser;
errorList eList;
node *tempTracker;


int main() {
    //cout << "starting parsing\n";
    scopeList semanticOrder;
    semanticOrder.addScope(1);
    semanticOrder.addScope(2);
    semanticOrder.addScope(3);
    semanticOrder.addScope(6);
    semanticOrder.addScope(7);
    semanticOrder.addScope(8);
    semanticOrder.addScope(9);
    semanticOrder.addScope(11);
    semanticOrder.addScope(4);
    semanticOrder.addScope(5);
    semanticOrder.addScope(10);
    semanticOrder.addScope(12);
    Token token;
    token = l.gt();
    if(token.token_type == LBRACE)
    {
        //cout << "In first scope\n";
        l.gut(token);
        parse_program();
        //cout<<"\naway\n";
        //cout<<"scope num: " << currentScope;
        tempTracker = vList.head;
        while(tempTracker != NULL)
        {
            if(tempTracker->isReferenced == 0)
            {
                eList.addE(3, 0, tempTracker->name);
                break;
            }
            tempTracker = tempTracker->next;
        }
        lineNoDetList dumperList;
        lineNoDetList clearer;
        lineNumDet *linePointDet;
        if(eList.head == NULL)
        {
            lineNum *linePoint;
            tempTracker = vList.head;
            for(int w = 1; w <= maximumLineNumberReached; w++)
            {
                dumperList = clearer;
                tempTracker = vList.head;
                while(tempTracker != NULL)
                {
                    linePoint = tempTracker->lineList->head;
                    while(linePoint != NULL)
                    {
                        if(linePoint->value == w)
                        {
                            //cout<<tempTracker->name<<" "<<linePoint->value<<" "<<tempTracker->declarationLineNum<<"\n";
                            dumperList.add_line(linePoint->value, linePoint->higherOrder, tempTracker->name, tempTracker->declarationLineNum);
                        }
                        linePoint = linePoint->next;
                    }
                    tempTracker = tempTracker->next;
                }
                for(int y = 1; y <= pOrder; y++)
                {
                    linePointDet = dumperList.head;
                    while(linePointDet != NULL)
                    {
                        if(linePointDet->higherOrder == y)
                        {
                            cout<<linePointDet->name<<" "<<linePointDet->value<<" "<<linePointDet->decl<<"\n";
                        }
                        linePointDet = linePointDet->next;
                    }
                }
            }




            exit(1);
        }

        //cout<<"\nthe enemy\n";

        int errorTypeFirst = -1;
        eNode *theChosenOne;
        int theDecider = 0;
        eNode *eChaser = eList.head;
        scope *orderEval = semanticOrder.head;
        while(orderEval != NULL)
        {
            eChaser = eList.head;
            while(eChaser != NULL)
            {
                if(eChaser->value == orderEval->number)
                {
                    theDecider = 1;
                    theChosenOne = eChaser;
                    break;
                }
                eChaser = eChaser->next;
            }
            if(theDecider == 1)
            {
                break;
            }
            orderEval = orderEval->next;
        }


//        for(int i = 1; i < 13; i++)
//        {
//            eChaser = eList.head;
//            while(eChaser != NULL)
//            {
//                if(eChaser->value == i)
//                {
//                    theDecider = 1;
//                    theChosenOne = eChaser;
//                    break;
//                }
//                eChaser = eChaser->next;
//            }
//            if(theDecider == 1)
//            {
//                break;
//            }
//        }
        if(theChosenOne->value != 12)
        {
            //cout<<"\nwoah\n";
            printSemanticError(theChosenOne);
        }
        else{
            //cout<<"\ntn\n";
            eChaser = eList.head;
            while(eChaser != NULL)
            {
                //cout<<"\nthats right!\n";
                if(eChaser->value == 12)
                {
                    cout<<"UNINITIALIZED "<<eChaser->badVar<<" "<<eChaser->lineNumber<<"\n";
                }
                eChaser = eChaser->next;
            }
            exit(1);
        }

        return true;
    }
    else{
        cout << errorMessage;
        exit(1);
    }

    return 0;
}

bool parse_program(){
    //cout<<"in parse_program\n";
    Token token;
    token = l.gt();

    if(token.token_type == LBRACE)
    {
        //cout << "seen an L BRace\n";
        l.gut(token);
        parse_scope();
        token = l.gt();
        if(token.token_type == END_OF_FILE)
        {
            return true;
        }
            //cout<<"\nthis is the end of a scope\n";
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_scope(){
    currentScope++;
    int scopeHere = currentScope;
    sList.addScope(currentScope);
    //cout << "in parse scope\n";
    Token token;
    token = l.gt();

    if(token.token_type == LBRACE)
    {
        //cout << "seen another L Brace\n";
        token = l.gt();
        if(token.token_type == LBRACE || token.token_type == ID || token.token_type == WHILE)
        {
            //cout << "scope list about to start\n";
            l.gut(token);
            parse_scope_list();
            //cout <<"\nda\n";
            token = l.gt();
            //cout<<"\no ";
            //token.Print();
            //cout<<"\n";
            if(token.token_type == RBRACE)
            {
                maximumLineNumberReached = token.line_no;
                sList.deleteItem(currentScope);
                vList.updateRunning(currentScope);
                currentScope = currentScope - 1;
                return true;
            }
            else{
                //cout<<"\nthis one?\n";
                cout<<errorMessage;
                exit(1);
            }
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_scope_list(){
    //cout << "In scope list\n";
    Token token;
    token = l.gt();

    if(token.token_type == LBRACE || token.token_type == ID || token.token_type == WHILE)
    {
        //cout << "scope list pass 1\n";
        if(token.token_type == LBRACE)
        {
            l.gut(token);
            parse_scope();
            token = l.gt();
            if(token.token_type == LBRACE || token.token_type == ID || token.token_type == WHILE)
            {
                l.gut(token);
                parse_scope_list();
                return true;
            }
            else if(token.token_type == RBRACE)
            {
                l.gut(token);
                return true;
            }
            else{
                cout<<errorMessage;
                exit(1);
            }

        }
        else if(token.token_type == ID)
        {
            Token token2;
            token2 = l.gt();
            if(token2.token_type == COMMA || token2.token_type == COLON)
            {
                l.gut(token2);
                l.gut(token);
                tempVarList clear;
                dumpList = clear;
                parse_var_decl();
                token = l.gt();
                if(token.token_type == LBRACE || token.token_type == ID || token.token_type == WHILE)
                {
                    l.gut(token);
                    parse_scope_list();
                    return true;
                }
                else if(token.token_type == RBRACE)
                {
                    l.gut(token);
                    return true;
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }
            }
            else if(token2.token_type == EQUAL)
            {
                //cout<<"found assign statement\n";
                l.gut(token2);
                l.gut(token);
                parse_smt();
                token = l.gt();
                if(token.token_type == LBRACE || token.token_type == ID || token.token_type == WHILE)
                {
                    l.gut(token);
                    parse_scope_list();
                    return true;
                }
                else if(token.token_type == RBRACE)
                {
                    l.gut(token);
                    return true;
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }
            }
            else{
                cout<<errorMessage;
                exit(1);
            }

        }
        else if(token.token_type == WHILE)
        {
            //cout <<"\nHH\n";
            l.gut(token);
            parse_smt();
            //cout<<"\nthat's right\n";
            token = l.gt();
            if(token.token_type == LBRACE || token.token_type == ID || token.token_type == WHILE)
            {
                l.gut(token);
                parse_scope_list();
                return true;
            }
            else if(token.token_type == RBRACE)
            {
                //cout<<"\nthe end of while upper\n";
                l.gut(token);
                return true;
            }
            else{
                //cout<<"\nthis one....\n";
                cout<<errorMessage;
                exit(1);
            }
        }
        else{
            cout<<errorMessage;
            exit(1);
        }

    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_var_decl()
{
    Token token;
    token = l.gt();
    if(token.token_type == ID)
    {
        l.gut(token);
        parse_id_list();
        token = l.gt();
        if(token.token_type == COLON)
        {
            token = l.gt();
            if(token.token_type == REAL || token.token_type == INT || token.token_type == BOOLEAN || token.token_type == STRING)
            {
                chaser = dumpList.head;
                while(chaser != NULL)
                {
                    intDump = 0;
                    tempTracker = vList.head;

                    while(tempTracker != NULL)
                    {
                        if(chaser->name == tempTracker->name)
                        {
                            //cout<<"yes\n";
                            if(tempTracker->scopeNum == currentScope)
                            {
                                //cout<<"yes!\n";
                                if(tempTracker->outOfTheRunning == 0)
                                {
                                    //cout<<"YES!\n";
                                    eList.addE(1, token.line_no, chaser->name);
                                    break;
                                }
                            }
                        }
                        tempTracker = tempTracker->next;
                    }
                    vList.add_Var(currentScope, token.line_no, token.lexeme, chaser->name);
                    chaser = chaser->next;
                }
                l.gut(token);
                parse_type_name();
                token = l.gt();
                if(token.token_type == SEMICOLON)
                {
                    return true;
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }
            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_id_list()
{
    Token token;
    token = l.gt();
    if(token.token_type == ID)
    {
        dumpList.addVar(token.lexeme);
        token = l.gt();
        if(token.token_type == COMMA)
        {
            token = l.gt();
            if(token.token_type == ID)
            {
                l.gut(token);
                parse_id_list();
                return true;
            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else if(token.token_type == COLON)
        {
            l.gut(token);
            return true;
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_type_name()
{
    Token token;
    token = l.gt();

    if(token.token_type == REAL || token.token_type == INT || token.token_type == BOOLEAN || token.token_type == STRING)
    {
        return true;
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_smt(){
    Token token;
    token = l.gt();
    if(token.token_type == ID)
    {
        //cout << "going to assign statement\n";
        l.gut(token);
        parse_assign_smt();
        return true;
    }
    else if(token.token_type == WHILE)
    {
        l.gut(token);
        parse_while_smt();
        //cout<<"\nabstraction\n";
        return true;
    }
    else{
        cout<<errorMessage;
        exit(1);
    }

}

bool parse_assign_smt()
{
    //cout << "in assign statement\n";
    Token token;
    token = l.gt();
    if(token.token_type == ID)
    {
        tempTracker = vList.head;
        intDump = 0;
        int reversal7;
        reversal7 = currentScope;
        while(reversal7 > 0)
        {
            tempTracker = vList.head;
            while(tempTracker != NULL)
            {
                if(tempTracker->name == token.lexeme)
                {
                    if(tempTracker->scopeNum == reversal7)
                    {
                        if(tempTracker->outOfTheRunning == 0)
                        {
                            stringDump = tempTracker->type;
                            intDump = 1;
                            break;
                        }
                    }
                }
                tempTracker = tempTracker->next;
            }
            if(intDump == 1)
            {
                break;
            }
            reversal7 = reversal7 - 1;
        }


        tempTracker = vList.head;
        intDump = 0;
        while(tempTracker != NULL)
        {
            if(tempTracker->name == token.lexeme)
            {
                if(tempTracker->outOfTheRunning == 0)
                {
                    intDump = 1;
                }
            }
            tempTracker = tempTracker->next;
        }
        if(intDump == 0)
        {
            eList.addE(2, token.line_no, token.lexeme);
        }
        tempTracker = vList.head;
        intDump = 0;
        int reversal1;
        reversal1 = currentScope;
        std::string storeNameTemp;
        storeNameTemp = token.lexeme;
        while(reversal1 > 0)
        {
            tempTracker = vList.head;
            while(tempTracker != NULL)
            {
                if(tempTracker->name == token.lexeme)
                {
                    if(tempTracker->scopeNum == reversal1)
                    {
                        if(tempTracker->outOfTheRunning == 0)
                        {
                            tempTracker->isReferenced = 1;
                            pOrder = pOrder + 1;
                            tempTracker->lineList->add_line(token.line_no, pOrder);
                            intDump = 1;
                            break;
                        }
                    }
                }
                tempTracker = tempTracker->next;
            }
            if(intDump == 1)
            {
                break;
            }
            reversal1 = reversal1 - 1;
        }

        //cout << "first pass\n";
        token = l.gt();
        if(token.token_type == EQUAL)
        {
            //cout << "second pass\n";
            token = l.gt();
            if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
            {
                //cout << "third pass\n";
                l.gut(token);
                if(stringDump == "INT" || stringDump == "REAL")
                {
                    if(token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT)
                    {
                        if(stringDump == "INT")
                        {
                            eList.addE(4, token.line_no, token.lexeme);
                        }
                        else{
                            eList.addE(5, token.line_no, token.lexeme);
                        }
                    }

                }
                else if(stringDump == "STRING")
                {
                    if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT)
                    {
                        eList.addE(4, token.line_no, token.lexeme);
                    }
                }
                else if(stringDump == "BOOLEAN")
                {
                    if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV)
                    {
                        eList.addE(4, token.line_no, token.lexeme);
                    }
                }
                parse_expr();
                token = l.gt();
                if(token.token_type == SEMICOLON)
                {
                    int reversal20;
                    reversal20 = currentScope;
                    tempTracker = vList.head;
                    while(reversal20 > 0)
                    {
                        tempTracker = vList.head;
                        while(tempTracker != NULL)
                        {
                            if(tempTracker->name == storeNameTemp)
                            {
                                if(tempTracker->scopeNum == reversal20)
                                {
                                    if(tempTracker->outOfTheRunning == 0)
                                    {
                                        tempTracker->isInitialized = 1;
                                        intDump = 1;
                                        break;
                                    }
                                }
                            }
                            tempTracker = tempTracker->next;
                        }
                        if(intDump == 1)
                        {
                            break;
                        }
                        reversal20 = reversal20 - 1;
                    }
                    //cout << "fourth pass\n";
                    return true;
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }
            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_expr()
{
    Token token;
    token = l.gt();
    if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
    {
        if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV)
        {
            l.gut(token);
            parse_arithmetic_operator();
            token = l.gt();
            if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
            {
                if(token.token_type == ID)
                {
                    std::string d;
                    tempTracker = vList.head;
                    intDump = 0;
                    int reversal3;
                    reversal3 = currentScope;
                    while(reversal3 > 0)
                    {
                        tempTracker = vList.head;
                        while(tempTracker != NULL)
                        {
                            if(tempTracker->name == token.lexeme)
                            {
                                if(tempTracker->scopeNum == reversal3)
                                {
                                    if(tempTracker->outOfTheRunning == 0)
                                    {
                                        d = tempTracker->type;
                                        intDump = 1;
                                        break;
                                    }
                                }
                            }
                            tempTracker = tempTracker->next;
                        }
                        if(intDump == 1)
                        {
                            break;
                        }
                        reversal3 = reversal3 - 1;
                    }
                    if(d != "INT" && d != "REAL")
                    {
                        eList.addE(6, token.line_no, token.lexeme);
                    }
                }
                else if(token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
                {
                    eList.addE(6, token.line_no, token.lexeme);
                }
                l.gut(token);
                parse_expr();
                token = l.gt();
                if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID  || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
                {
                    if(token.token_type == ID)
                    {
                        std::string d;
                        tempTracker = vList.head;
                        intDump = 0;
                        int reversal4;
                        reversal4 = currentScope;
                        while(reversal4 > 0)
                        {
                            tempTracker = vList.head;
                            while(tempTracker != NULL)
                            {
                                if(tempTracker->name == token.lexeme)
                                {
                                    if(tempTracker->scopeNum == reversal4)
                                    {
                                        if(tempTracker->outOfTheRunning == 0)
                                        {
                                            d = tempTracker->type;
                                            intDump = 1;
                                            break;
                                        }
                                    }
                                }
                                tempTracker = tempTracker->next;
                            }
                            if(intDump == 1)
                            {
                                break;
                            }
                            reversal4 = reversal4 - 1;
                        }

                        if(d != "INT" && d != "REAL")
                        {
                            eList.addE(6, token.line_no, token.lexeme);
                        }
                    }
                    else if(token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
                    {
                        eList.addE(6, token.line_no, token.lexeme);
                    }
                    l.gut(token);
                    parse_expr();
                    return true;
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }
            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else if(token.token_type == AND || token.token_type == OR || token.token_type == XOR)
        {

            l.gut(token);
            parse_binary_boolean_operator();
            token = l.gt();
            if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
            {
                if(token.token_type == ID)
                {
                    std::string d;
                    tempTracker = vList.head;
                    int reversal8;
                    reversal8 = currentScope;
                    intDump = 0;
                    while(reversal8 > 0)
                    {
                        tempTracker = vList.head;
                        while(tempTracker != NULL)
                        {
                            if(tempTracker->name == token.lexeme)
                            {
                                if(tempTracker->scopeNum == reversal8)
                                {
                                    if(tempTracker->outOfTheRunning == 0)
                                    {
                                        d = tempTracker->type;
                                        intDump = 1;
                                        break;
                                    }
                                }
                            }
                            tempTracker = tempTracker->next;
                        }
                        if(intDump == 1)
                        {
                            break;
                        }
                        reversal8 = reversal8 - 1;
                    }

                    if(d != "BOOLEAN")
                    {
                        eList.addE(7, token.line_no, token.lexeme);
                    }
                }
                else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT)
                {
                    eList.addE(7, token.line_no, token.lexeme);
                }
                l.gut(token);
                parse_expr();
                token = l.gt();
                if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
                {
                    if(token.token_type == ID)
                    {
                        std::string d;
                        tempTracker = vList.head;
                        int reversal9;
                        reversal9 = currentScope;
                        intDump = 0;
                        while(reversal9 > 0)
                        {
                            tempTracker = vList.head;
                            while(tempTracker != NULL)
                            {
                                if(tempTracker->name == token.lexeme)
                                {
                                    if(tempTracker->scopeNum == reversal9)
                                    {
                                        if(tempTracker->outOfTheRunning == 0)
                                        {
                                            d = tempTracker->type;
                                            intDump = 1;
                                            break;
                                        }
                                    }
                                }
                                tempTracker = tempTracker->next;
                            }
                            if(intDump == 1)
                            {
                                break;
                            }
                            reversal9 = reversal9 - 1;
                        }

                        if(d != "BOOLEAN")
                        {
                            eList.addE(7, token.line_no, token.lexeme);
                        }
                    }
                    else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT)
                    {
                        eList.addE(7, token.line_no, token.lexeme);
                    }
                    l.gut(token);
                    parse_expr();
                    return true;
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }
            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else if(token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ)
        {
            l.gut(token);
            parse_relational_operator();
            token = l.gt();
            std::string relationalPlaceHolder1;
            std::string relationalPlaceHolder2;
            if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
            {
                if(token.token_type == ID)
                {
                    tempTracker = vList.head;
                    intDump = 0;
                    int reversal5;
                    reversal5 = currentScope;
                    while(reversal5 > 0)
                    {
                        tempTracker = vList.head;
                        while(tempTracker != NULL)
                        {
                            if(tempTracker->name == token.lexeme)
                            {
                                if(tempTracker->scopeNum == reversal5)
                                {
                                    if(tempTracker->outOfTheRunning == 0)
                                    {
                                        relationalPlaceHolder1 = tempTracker->type;
                                        intDump = 1;
                                        break;
                                    }
                                }
                            }
                            tempTracker = tempTracker->next;
                        }
                        if(intDump == 1)
                        {
                            break;
                        }
                        reversal5 = reversal5 - 1;
                    }

                }
                else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
                {
                    if(token.token_type == NUM || token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == REALNUM)
                    {
                        relationalPlaceHolder1 = "INT";
                    }
                    else if(token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == TRUE || token.token_type == FALSE)
                    {
                        relationalPlaceHolder1 = "BOOLEAN";
                    }
                    else if(token.token_type == STRING_CONSTANT)
                    {
                        relationalPlaceHolder1 = "STRING";
                    }
                }
                l.gut(token);
                parse_expr();
                token = l.gt();
                if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
                {
                    if(token.token_type == ID)
                    {
                        tempTracker = vList.head;
                        intDump = 0;
                        int reversal6;
                        reversal6 = currentScope;
                        while(reversal6 > 0)
                        {
                            tempTracker = vList.head;
                            while(tempTracker != NULL)
                            {
                                if(tempTracker->name == token.lexeme)
                                {
                                    if(tempTracker->scopeNum == reversal6)
                                    {
                                        if(tempTracker->outOfTheRunning == 0)
                                        {
                                            relationalPlaceHolder2 = tempTracker->type;
                                            intDump = 1;
                                            break;
                                        }
                                    }
                                }
                                tempTracker = tempTracker->next;
                            }
                            if(intDump == 1)
                            {
                                break;
                            }
                            reversal6 = reversal6 - 1;
                        }

                    }
                    else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
                    {
                        if(token.token_type == NUM || token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == REALNUM)
                        {
                            relationalPlaceHolder2 = "INT";
                        }
                        else if(token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == TRUE || token.token_type == FALSE)
                        {
                            relationalPlaceHolder2 = "BOOLEAN";
                        }
                        else if(token.token_type == STRING_CONSTANT)
                        {
                            relationalPlaceHolder2 = "STRING";
                        }
                    }
                    if(relationalPlaceHolder1 == "BOOLEAN" || relationalPlaceHolder1 == "STRING")
                    {
                        if(relationalPlaceHolder1 != relationalPlaceHolder2)
                        {
                            eList.addE(8, token.line_no, token.lexeme);
                        }
                    }
                    else{
                        if(relationalPlaceHolder1 == "INT" || relationalPlaceHolder1 == "REAL")
                        {
                            if(relationalPlaceHolder2 != "INT" && relationalPlaceHolder2 != "REAL")
                            {
                                eList.addE(9, token.line_no, token.lexeme);
                            }
                        }
                    }
                    l.gut(token);
                    parse_expr();
                    return true;
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }
            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
        {
            std::string stringDumpTemp;
            if(token.token_type == ID)
            {
                tempTracker = vList.head;
                intDump = 0;
                int reversal27;
                reversal27 = currentScope;
                while(reversal27 > 0)
                {
                    tempTracker = vList.head;
                    while(tempTracker != NULL)
                    {
                        if(tempTracker->name == token.lexeme)
                        {
                            if(tempTracker->scopeNum == reversal27)
                            {
                                if(tempTracker->outOfTheRunning == 0)
                                {
                                    if(tempTracker->isInitialized == 0)
                                    {
                                        eList.addE(12, token.line_no, token.lexeme);
                                        intDump = 1;
                                        break;
                                    }
                                    else{
                                        intDump = 1;
                                        break;
                                    }
                                }
                            }
                        }
                        tempTracker = tempTracker->next;
                    }
                    if(intDump == 1)
                    {
                        break;
                    }
                    reversal27 = reversal27 - 1;
                }

                stringDumpTemp = stringDump;
                int reversal10;
                reversal10 = currentScope;
                tempTracker = vList.head;
                intDump = 0;
                while(reversal10 > 0)
                {
                    tempTracker = vList.head;
                    while(tempTracker != NULL)
                    {
                        if(tempTracker->name == token.lexeme)
                        {
                            if(tempTracker->scopeNum == currentScope)
                            {
                                if(tempTracker->outOfTheRunning == 0)
                                {
                                    stringDumpTemp = tempTracker->type;
                                    intDump = 1;
                                    break;
                                }
                            }
                        }
                        tempTracker = tempTracker->next;
                    }
                    if(intDump == 1)
                    {
                        break;
                    }
                    reversal10 = reversal10 - 1;
                }

                if(stringDump != "REAL")
                {
                    if(stringDumpTemp != stringDump)
                    {
                        eList.addE(4, token.line_no, token.lexeme);
                    }
                }
                else{
                    if(stringDumpTemp != "INT" && stringDumpTemp != "REAL")
                    {
                        eList.addE(5, token.line_no, token.lexeme);
                    }
                }

                tempTracker = vList.head;
                intDump = 0;
                while(tempTracker != NULL)
                {
                    if(tempTracker->name == token.lexeme)
                    {
                        if(tempTracker->outOfTheRunning == 0)
                        {
                            intDump = 1;
                        }
                    }
                    tempTracker = tempTracker->next;
                }
                if(intDump == 0)
                {
                    eList.addE(2, token.line_no, token.lexeme);
                }
                tempTracker = vList.head;
                intDump = 0;
                int reversal2;
                reversal2 = currentScope;
                while(reversal2 > 0)
                {
                    tempTracker = vList.head;
                    while(tempTracker != NULL)
                    {
                        if(tempTracker->name == token.lexeme)
                        {
                            if(tempTracker->scopeNum == reversal2)
                            {
                                if(tempTracker->outOfTheRunning == 0)
                                {
                                    tempTracker->isReferenced = 1;
                                    pOrder = pOrder + 1;
                                    tempTracker->lineList->add_line(token.line_no, pOrder);
                                    intDump = 1;
                                    break;
                                }
                            }
                        }
                        tempTracker = tempTracker->next;
                    }
                    if(intDump == 1)
                    {
                        break;
                    }
                    reversal2 = reversal2 - 1;
                }

            }
            else{
                if(token.token_type == NUM || token.token_type == REALNUM)
                {
                    stringDumpTemp = "INT";
                }
                else if(token.token_type == STRING_CONSTANT)
                {
                    stringDumpTemp = "STRING";
                }
                else if(token.token_type == TRUE || token.token_type == FALSE)
                {
                    stringDumpTemp = "BOOLEAN";
                }

                if(stringDump != "REAL")
                {
                    if(stringDumpTemp != stringDump)
                    {
                        eList.addE(4, token.line_no, token.lexeme);
                    }
                }
                else{
                    if(stringDumpTemp != "INT" && stringDumpTemp != "REAL")
                    {
                        eList.addE(5, token.line_no, token.lexeme);
                    }
                }


            }
            l.gut(token);
            parse_primary();
            return true;
        }
        else if(token.token_type == NOT)
        {
            token = l.gt();
            if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
            {
                if(token.token_type == ID)
                {
                    std::string typeHolder;
                    tempTracker = vList.head;
                    intDump = 0;
                    int reversal11;
                    reversal11 = currentScope;
                    while(reversal11 > 0)
                    {
                        tempTracker = vList.head;
                        while(tempTracker != NULL)
                        {
                            if(tempTracker->name == token.lexeme)
                            {
                                if(tempTracker->scopeNum == reversal11)
                                {
                                    if(tempTracker->outOfTheRunning == 0)
                                    {
                                        typeHolder = tempTracker->type;
                                        intDump = 1;
                                        break;
                                    }
                                }
                            }
                            tempTracker = tempTracker->next;
                        }
                        if(intDump == 1)
                        {
                            break;
                        }
                        reversal11 = reversal11 - 1;
                    }

                    if(typeHolder != "BOOLEAN")
                    {
                        eList.addE(11, token.line_no, token.lexeme);
                    }
                }
                else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT)
                {
                    eList.addE(11, token.line_no, token.lexeme);
                }
                l.gut(token);
                parse_expr();
                return true;
            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_arithmetic_operator()
{
    Token token;
    token = l.gt();
    if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV)
    {
        return true;
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_binary_boolean_operator()
{
    Token token;
    token = l.gt();
    if(token.token_type == AND || token.token_type == OR || token.token_type == XOR)
    {
        return true;
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_relational_operator()
{
    Token token;
    token = l.gt();
    if(token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ)
    {
        return true;
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_primary()
{
    Token token;
    token = l.gt();
    if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
    {
        if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT)
        {
            return true;
        }
        else if(token.token_type == TRUE || token.token_type == FALSE)
        {
            l.gut(token);
            parse_bool_const();
            return true;
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_bool_const()
{
    Token token;
    token = l.gt();
    if(token.token_type == TRUE || token.token_type == FALSE)
    {
        return true;
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_while_smt()
{
    //cout << "\nEntering while statement\n";
    Token token;
    token = l.gt();
    if(token.token_type == WHILE)
    {
        //cout<<"first while pass\n";
        token = l.gt();
        if(token.token_type == LPAREN)
        {
            l.gut(token);
            parse_condition();
            token = l.gt();
            if(token.token_type == LBRACE)
            {
                currentScope++;
                sList.addScope(currentScope);
                //spoops
                token = l.gt();
                if(token.token_type == ID || token.token_type == WHILE)
                {
                    l.gut(token);
                    parse_smt_list();
                    token = l.gt();
                    if(token.token_type == RBRACE)
                    {
                        sList.deleteItem(currentScope);
                        vList.updateRunning(currentScope);
                        currentScope = currentScope - 1;
                        return true;
                    }
                    else{
                        cout<<errorMessage;
                        exit(1);
                    }
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }
            }
            else if(token.token_type == ID || token.token_type == WHILE)
            {
                l.gut(token);
                parse_smt();
                token = l.gt();
                Token token2;
                token2 = l.gt();
                l.gut(token2);
                l.gut(token);
                if(token.token_type != ID && token2.token_type != EQUAL || token.token_type != WHILE)
                {
                    return true;
                }
                else{
                    cout<<errorMessage;
                    exit(1);
                }

            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}//check

bool parse_condition()
{
    Token token;
    token = l.gt();
    if(token.token_type == LPAREN)
    {
        token = l.gt();
        if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == AND || token.token_type == OR || token.token_type == XOR || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LESS || token.token_type == NOTEQUAL || token.token_type == LTEQ || token.token_type == NOT || token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT || token.token_type == TRUE || token.token_type == FALSE)
        {
            if(token.token_type == ID)
            {
                std::string typeHolder;
                tempTracker = vList.head;
                int reversal12;
                reversal12 = currentScope;
                intDump = 0;
                while(reversal12 > 0)
                {
                    tempTracker = vList.head;
                    while(tempTracker != NULL)
                    {
                        if(tempTracker->name == token.lexeme)
                        {
                            if(tempTracker->scopeNum == reversal12)
                            {
                                if(tempTracker->outOfTheRunning == 0)
                                {
                                    typeHolder = tempTracker->type;
                                    intDump = 1;
                                    break;
                                }
                            }
                        }
                        tempTracker = tempTracker->next;
                    }
                    if(intDump == 1)
                    {
                        break;
                    }
                    reversal12 = reversal12 - 1;
                }

                if(typeHolder != "BOOLEAN")
                {
                    eList.addE(10, token.line_no, token.lexeme);
                }
            }
            else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == NUM || token.token_type == REALNUM || token.token_type == STRING_CONSTANT)
            {
                eList.addE(10, token.line_no, token.lexeme);
            }
            l.gut(token);
            parse_expr();
            token = l.gt();
            if(token.token_type == RPAREN)
            {
                return true;
            }
            else{
                cout<<errorMessage;
                exit(1);
            }
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}

bool parse_smt_list()
{
    Token token;
    token = l.gt();
    if(token.token_type == ID || token.token_type == WHILE)
    {
        l.gut(token);
        parse_smt();
        token = l.gt();
        Token token2;
        token2 = l.gt();
        l.gut(token2);
        l.gut(token);
        if(token.token_type == RBRACE)
        {
            return true;
        }
        else if(token.token_type == ID && token2.token_type == EQUAL || token.token_type == WHILE)
        {
            parse_smt_list();
            return true;
        }
        else{
            cout<<errorMessage;
            exit(1);
        }
    }
    else{
        cout<<errorMessage;
        exit(1);
    }
}





