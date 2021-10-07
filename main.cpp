#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "lexer.h"
#include "compiler.h"
#include <string>
#include <sstream>

using namespace std;

int trueIndex = 0;
LexicalAnalyzer lexer;
bool trueBranchParseIf(int index);
bool trueBranchParseSmtList(int index);
bool trueBranchParseSmt(int index);
bool trueBranchParseAssign(int index);
bool trueBranchParsePrint(int index);
bool trueBranchParseWhile(int index);
bool trueBranchParseBody(int index);
bool parse_program(); //DONE
bool parse_var_section(); //DONE
bool parse_body(); //DONE
bool parse_id_list(); //DONE
bool parse_smt_list(); //DONE
bool parse_smt(); //DONE
bool parse_assign_smt(); //DONE
bool parse_primary(); //DONE
bool parse_expr(); //DONE
bool parse_op(); //DONE
bool parse_print_smt(); //DONE
bool parse_while_smt(); //DONE
bool parse_condition(); //DONE
bool parse_relop(); //DONE
bool parse_if_smt(); //DONE
bool parse_switch_smt(); //DONE
bool parse_case_list(); //DONE
bool parse_case(); //DONE
bool parse_default_case(); //DONE
bool parse_for_smt(); //DONE

StatementNode *node = new StatementNode;
StatementNode *nodeTracker;

struct trueBranch{
    StatementNode *firstStatement;
    trueBranch *next;
};

class trueBranches{
public:
    trueBranch *head;
    trueBranch *tail;
    trueBranches(){
        head = NULL;
        tail = NULL;
    }
    void addBranch()
    {
        trueBranch *temp = new trueBranch;
        StatementNode *tempStatement = new StatementNode;
        temp->firstStatement = tempStatement;
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
    void addToBranch(int index, StatementNode *nodeToAdd)
    {
        trueBranch *temp = head;
        int tempIndex = 1;
        while(true)
        {
            if(tempIndex == index)
            {
                //cout<<"\nfound index "<<tempIndex<<"\n";
                StatementNode *tempTracker = temp->firstStatement;
                if(tempTracker->type == NULL)
                {
                    //cout<<"\yes\n";
                    temp->firstStatement = nodeToAdd;
                }
                else{
                    while(tempTracker != NULL)
                    {
                        //cout<<"dbs\n";
                        if(tempTracker->next == NULL)
                        {
                            tempTracker->next = nodeToAdd;
                            break;
                        }
                        tempTracker = tempTracker->next;
                    }
                }
                break;
            }
            temp = temp->next;
            tempIndex = tempIndex + 1;
        }
    }
    StatementNode * getBranch(int index)
    {
        trueBranch *temp = head;
        int tempIndex = 1;
        while(true)
        {
            if(tempIndex == index)
            {
                return temp->firstStatement;
                break;
            }
            temp = temp->next;
            tempIndex = tempIndex + 1;
        }
    }
    void printBranches()
    {
        int index = 0;
        trueBranch *temp = head;
        StatementNode *tracker;
        while(temp != NULL)
        {
            index = index + 1;
            tracker = temp->firstStatement;
            cout<<"At index "<<index<<": "<<"\n";
            while(tracker != NULL)
            {
                cout<<tracker->type << "\n";
                tracker = tracker->next;
            }
            temp = temp->next;
        }

    }

};

struct vNode{
    ValueNode *storedV;
    vNode *next;
};

class theVarList{
public:
    vNode *head;
    vNode *tail;
    theVarList(){
        head = NULL;
        tail = NULL;
    }
    void addVar(std::string inString, int inValue)
    {
        vNode *temp = new vNode;
        temp->next = NULL;
        ValueNode *tempValueNode = new ValueNode;
        tempValueNode->name = inString;
        tempValueNode->value = inValue;
        temp->storedV = tempValueNode;
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
    void updateVar(std::string finderString, int replaceVal)
    {
        vNode *tracker = head;
        while(tracker != NULL)
        {
            if(finderString == tracker->storedV->name)
            {
                tracker->storedV->value = replaceVal;
                break;
            }
            tracker = tracker->next;
        }
    }
    bool isAVar(std::string trackerString)
    {
        vNode *tracker = head;
        bool retBool = false;
        while(tracker != NULL)
        {
            if(tracker->storedV->name == trackerString)
            {
                retBool = true;
                break;
            }
            tracker = tracker->next;
        }
        return retBool;
    }
    int getVal(std::string trackerString)
    {
        vNode *tracker = head;
        int retInt = -1;
        while(tracker != NULL)
        {
            if(tracker->storedV->name == trackerString)
            {
                retInt = tracker->storedV->value;
                break;
            }
            tracker = tracker->next;
        }
        return retInt;
    }
    void printList()
    {
        //cout<<"\n print\n";
        vNode *tracker = head;
        //cout<<"\n do it\n";
        while(tracker != NULL)
        {
            //cout<<"\nfvdv\n";
            cout<<"\nVarName: "<<tracker->storedV->name<<"\nVarValue: "<<tracker->storedV->value<<"\n";
            //cout<<"\nyeah\n";
            tracker = tracker->next;
        }
    }

    ValueNode * getValueNode(std::string finderString)
    {
        vNode *tracker = head;
        //ValueNode *retValueNode;
        while(tracker != NULL)
        {
            if(tracker->storedV->name == finderString)
            {
                return tracker->storedV;
                //break;
            }
            tracker = tracker->next;
        }
        //return retValueNode;
    }

};

theVarList varList;
trueBranches branchList;


void addToNode(StatementNode *nodeToAdd)
{
    if(node->type == NULL)
    {
        node = nodeToAdd;
        //node->next = NULL;
    }
    else{
        nodeTracker = node;
        while(true)
        {
            if(nodeTracker->next == NULL)
            {
                nodeTracker->next = nodeToAdd;
                //nodeTracker->next->next = NULL;
                break;
            }
            nodeTracker = nodeTracker->next;
        }
    }
}


struct StatementNode * parse_generate_intermediate_representation(){
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        lexer.UngetToken(token);
        //cout<<"\nabout to parse\n";
        parse_program();
    }
    else{
        //cout<<"ig";
        parse_program();
    }
    //cout<<"\nyu\n";
    StatementNode *returnNode = node;
    //varList.printList();
    return returnNode;
}

bool parse_program()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
       lexer.UngetToken(token);
       parse_var_section();
       token = lexer.GetToken();
       if(token.token_type == LBRACE)
       {
           lexer.UngetToken(token);
           parse_body();
           return true;
       }
       else{
           exit(1);
       }
    }
    else{
        exit(1);
    }
    cout<<"simple";
}

bool parse_var_section()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        lexer.UngetToken(token);
        parse_id_list();
        token = lexer.GetToken();
        if(token.token_type == SEMICOLON)
        {
            return true;
        }
        else{
            exit(1);
        }
    }
    else{
        exit(1);
    }
}

bool parse_id_list()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        //VARS ADDED
        //cout<<"'\nabout to add var "<<token.lexeme<<"\n";
        varList.addVar(token.lexeme, 0);
        //cout<<"\nvar added\n";
        //varList.printList();
        token = lexer.GetToken();
        if(token.token_type == COMMA)
        {
            parse_id_list();
            token = lexer.GetToken();
            if(token.token_type == SEMICOLON)
            {
                lexer.UngetToken(token);
                return true;
            }
        }
        else if(token.token_type == SEMICOLON)
        {
            lexer.UngetToken(token);
            return true;
        }
        else{
            exit(1);
        }
    }
    else{
        exit(1);
    }
}

bool parse_body()
{
    //StatementNode *deliveryStatements = new StatementNode;
    Token token;
    token = lexer.GetToken();
    if(token.token_type == LBRACE)
    {
        parse_smt_list();
        token = lexer.GetToken();
        if(token.token_type == RBRACE)
        {
            return true;
        }
        else{
            cout<<"\nparse_body_Err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_body_Err1\n";
        exit(1);
    }
}

bool parse_smt_list()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID || token.token_type == PRINT || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH || token.token_type == FOR)
    {
        lexer.UngetToken(token);
        parse_smt();
        token = lexer.GetToken();
        if(token.token_type == ID || token.token_type == PRINT || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH || token.token_type == FOR)
        {
            lexer.UngetToken(token);
            parse_smt_list();
            token = lexer.GetToken();
            if(token.token_type == RBRACE)
            {
                lexer.UngetToken(token);
                return true;
            }
            else{
                cout<<"\nsmt_list_err3\n";
                exit(1);
            }
        }
        else if(token.token_type == RBRACE)
        {
            lexer.UngetToken(token);
            return true;
        }
        else{
            cout<<"\nsmt_list_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nsmt_list_err1\n";
        exit(1);
    }
}

bool parse_smt()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID || token.token_type == PRINT || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH || token.token_type == FOR)
    {
        if(token.token_type == ID)
        {
            lexer.UngetToken(token);
            parse_assign_smt();
            return true;
        }
        else if(token.token_type == PRINT)
        {
            lexer.UngetToken(token);
            parse_print_smt();
            return true;
        }
        else if(token.token_type == IF)
        {
            lexer.UngetToken(token);
            parse_if_smt();
            return true;
        }
        else if(token.token_type == WHILE)
        {
            lexer.UngetToken(token);
            parse_while_smt();
            return true;
        }
        else if(token.token_type == SWITCH)
        {
            lexer.UngetToken(token);
            parse_switch_smt();
            return true;
        }
        else if(token.token_type == FOR)
        {
            lexer.UngetToken(token);
            parse_for_smt();
            return true;
        }
        else{
            cout<<"\nparse_smt_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_smt_err1\n";
        exit(1);
    }
}


bool parse_assign_smt()
{
    StatementNode *newStatement = new StatementNode;
    AssignmentStatement *newAssign = new AssignmentStatement;
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        std::string assignString = token.lexeme;
        token = lexer.GetToken();
        if(token.token_type == EQUAL)
        {
            token = lexer.GetToken();
            if(token.token_type == ID || token.token_type == NUM)
            {
                Token token2;
                token2 = lexer.GetToken();
                if(token2.token_type == SEMICOLON)
                {
                    lexer.UngetToken(token2);
                    lexer.UngetToken(token);
                    //ADDED SINGLE VAL ASSIGN
                    if(token.token_type == NUM)
                    {
                        ValueNode *tempoValueNode = new ValueNode;
                        tempoValueNode->name = token.lexeme;
                        tempoValueNode->value = std::atoi(token.lexeme.c_str());

                        newAssign->left_hand_side = varList.getValueNode(assignString);
                        newAssign->operand1 = tempoValueNode;
                        newAssign->op = OPERATOR_NONE;
                        newAssign->operand2 = NULL;

                        newStatement->assign_stmt = newAssign;
                        newStatement->type = ASSIGN_STMT;
                        newStatement->next = NULL;
                        addToNode(newStatement);
                        //varList.updateVar(assignString, std::atoi(token.lexeme.c_str()));
                    }
                    else if(token.token_type == ID)
                    {
                        newAssign->left_hand_side = varList.getValueNode(assignString);
                        newAssign->operand1 = varList.getValueNode(token.lexeme);
                        newAssign->op = OPERATOR_NONE;
                        newAssign->operand2 = NULL;

                        newStatement->assign_stmt = newAssign;
                        newStatement->type = ASSIGN_STMT;
                        newStatement->next = NULL;
                        addToNode(newStatement);
                        //varList.updateVar(assignString, varList.getVal(token.lexeme));
                    }
                    parse_primary();
                    token = lexer.GetToken();
                    if(token.token_type == SEMICOLON)
                    {
                        return true;
                    }
                    else{
                        cout<<"\nparse_Assign_err4\n";
                        exit(1);
                    }
                }
                else if(token2.token_type == PLUS || token2.token_type == MINUS || token2.token_type == MULT || token2.token_type == DIV)
                {
                    //assigning operator
                    newAssign->left_hand_side = varList.getValueNode(assignString);
                    if(token2.token_type == PLUS)
                    {
                        newAssign->op = OPERATOR_PLUS;
                    }
                    else if(token2.token_type == MINUS)
                    {
                        newAssign->op = OPERATOR_MINUS;
                    }
                    else if(token2.token_type == MULT)
                    {
                        newAssign->op = OPERATOR_MULT;
                    }
                    else if(token2.token_type == DIV)
                    {
                        newAssign->op = OPERATOR_DIV;
                    }
                    //assigning first operand
                    if(token.token_type == ID)
                    {
                       newAssign->operand1 = varList.getValueNode(token.lexeme);
                    }
                    else if(token.token_type == NUM)
                    {
                        ValueNode *tempoValueNode = new ValueNode;
                        tempoValueNode->name = token.lexeme;
                        tempoValueNode->value = std::atoi(token.lexeme.c_str());
                        newAssign->operand1 = tempoValueNode;
                    }
                    //assigning second operand
                    Token token3 = lexer.GetToken();
                    lexer.UngetToken(token3);
                    lexer.UngetToken(token2);
                    lexer.UngetToken(token);
                    if(token3.token_type == ID)
                    {
                        newAssign->operand2 = varList.getValueNode(token3.lexeme);
                    }
                    else if(token3.token_type == NUM)
                    {
                        ValueNode *tempoValueNodeOp2 = new ValueNode;
                        tempoValueNodeOp2->name = token3.lexeme;
                        tempoValueNodeOp2->value = std::atoi(token3.lexeme.c_str());
                        newAssign->operand2 = tempoValueNodeOp2;
                    }
                    newStatement->assign_stmt = newAssign;
                    newStatement->type = ASSIGN_STMT;
                    newStatement->next = NULL;
                    addToNode(newStatement);
                    parse_expr();
                    token = lexer.GetToken();
                    if(token.token_type == SEMICOLON)
                    {
                        return true;
                    }
                    else{
                        cout<<"\nparse_Assign_err5\n";
                        exit(1);
                    }
                }
                else{
                    cout<<"\nparse_assign_smt_err6\n";
                }
            }
            else{
                cout<<"\nparse_assign_err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nparse_assign_err2 "<<token.line_no<<" "<<token.lexeme<< "\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_assign_Err1\n";
        exit(1);
    }
}

bool parse_primary()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        return true;
    }
    else if(token.token_type == NUM)
    {
        return true;
    }
    else{
        cout<<"\nparse_primary_err\n";
        exit(1);
    }
}

bool parse_expr()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID || token.token_type == NUM)
    {
        lexer.UngetToken(token);
        parse_primary();
        token = lexer.GetToken();
        if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV)
        {
            lexer.UngetToken(token);
            parse_op();
            token = lexer.GetToken();
            if(token.token_type == ID || token.token_type == NUM)
            {
                lexer.UngetToken(token);
                parse_primary();
                return true;
            }
            else{
                cout<<"\nparse_Exp_err\n";
                exit(1);
            }
        }
        else{
            cout<<"\nparse_expr_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_expr_err3\n";
        exit(1);
    }
}

bool parse_op()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV)
    {
        return true;
    }
    else{
        cout<<"\nparse_op_err\n";
        exit(1);
    }
}



bool parse_print_smt()
{
    StatementNode *newNode = new StatementNode;
    PrintStatement *printSmt = new PrintStatement;
    Token token;
    token = lexer.GetToken();
    if(token.token_type == PRINT)
    {
        token = lexer.GetToken();
        if(token.token_type == ID)
        {
            printSmt->id = varList.getValueNode(token.lexeme);
            newNode->type = PRINT_STMT;
            newNode->next = NULL;
            newNode->print_stmt = printSmt;
            addToNode(newNode);
            token = lexer.GetToken();
            if(token.token_type == SEMICOLON)
            {
                return true;
            }
            else{
                cout<<"\nprint_err1\n";
                exit(1);
            }
        }
        else{
            cout<<"\nprinterr2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nprint_err3\n";
        exit(1);
    }
}

bool parse_if_smt()
{
    StatementNode *newNode = new StatementNode;
    newNode->type = IF_STMT;
    StatementNode *buffer = new StatementNode;
    buffer->next = NULL;
    buffer->type = NOOP_STMT;
    newNode->next = buffer;
    IfStatement *newIf = new IfStatement;
    Token token;
    token = lexer.GetToken();
    if(token.token_type == IF)
    {
        token = lexer.GetToken();
        if(token.token_type == ID || token.token_type == NUM)
        {
            //GETS OPERAND 1
            if(token.token_type == ID)
            {
                newIf->condition_operand1 = varList.getValueNode(token.lexeme);
            }
            else if(token.token_type == NUM)
            {
                ValueNode *tempOperand1 = new ValueNode;
                tempOperand1->value = std::atoi(token.lexeme.c_str());
                tempOperand1->name = token.lexeme;
                newIf->condition_operand1 = tempOperand1;
            }
            Token token2 = lexer.GetToken();
            Token token3 = lexer.GetToken();

            //GET RELOP
            if(token2.token_type == GREATER)
            {
                newIf->condition_op = CONDITION_GREATER;
            }
            else if(token2.token_type == LESS)
            {
                newIf->condition_op = CONDITION_LESS;
            }
            else if(token2.token_type == NOTEQUAL)
            {
                newIf->condition_op = CONDITION_NOTEQUAL;
            }

            //GET OPERAND 2
            if(token3.token_type == ID)
            {
                newIf->condition_operand2 = varList.getValueNode(token3.lexeme);
            }
            else if(token3.token_type == NUM)
            {
                ValueNode *tempOperand2 = new ValueNode;
                tempOperand2->value = std::atoi(token3.lexeme.c_str());
                tempOperand2->name = token3.lexeme;
                newIf->condition_operand2 = tempOperand2;
            }
            lexer.UngetToken(token3);
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_condition();
            token = lexer.GetToken();
            if(token.token_type == LBRACE)
            {
                //ADDED BRANCHES:
                newIf->false_branch = buffer;
                //StatementNode *tempTrueBranch = new StatementNode;
                lexer.UngetToken(token);
                int returnIndex = trueIndex + 1;
                trueIndex = trueIndex + 1;
                branchList.addBranch();
                trueBranchParseBody(returnIndex);
                branchList.addToBranch(returnIndex, buffer);
                newIf->true_branch = branchList.getBranch(returnIndex);
                newNode->if_stmt = newIf;
                addToNode(newNode);
                //branchList.printBranches();
                return true;
            }
            else{
                cout<<"\nif_smt_Err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nif_smt_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nif_smt_err1\n";
        exit(1);
    }
}

bool trueBranchParseBody(int index)
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == LBRACE)
    {
        trueBranchParseSmtList(index);
        token = lexer.GetToken();
        if(token.token_type == RBRACE)
        {
            return true;
        }
        else{
            cout<<"\nparse_body_Err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_body_Err1\n";
        exit(1);
    }

    //return returnTrueBranch;
}

bool trueBranchParseSmtList(int index)
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID || token.token_type == PRINT || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH || token.token_type == FOR)
    {
        lexer.UngetToken(token);
        trueBranchParseSmt(index);
        token = lexer.GetToken();
        if(token.token_type == ID || token.token_type == PRINT || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH || token.token_type == FOR)
        {
            lexer.UngetToken(token);
            trueBranchParseSmtList(index);
            token = lexer.GetToken();
            if(token.token_type == RBRACE)
            {
                lexer.UngetToken(token);
                return true;
            }
            else{
                cout<<"\nsmt_list_err3\n";
                exit(1);
            }
        }
        else if(token.token_type == RBRACE)
        {
            lexer.UngetToken(token);
            return true;
        }
        else{
            cout<<"\nsmt_list_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nsmt_list_err1\n";
        exit(1);
    }
}

bool trueBranchParseSmt(int index)
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID || token.token_type == PRINT || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH || token.token_type == FOR)
    {
        if(token.token_type == ID)
        {
            lexer.UngetToken(token);
            trueBranchParseAssign(index);
            return true;
        }
        else if(token.token_type == PRINT)
        {
            lexer.UngetToken(token);
            trueBranchParsePrint(index);
            return true;
        }
        else if(token.token_type == IF)
        {
            lexer.UngetToken(token);
            trueBranchParseIf(index);
        }
        else if(token.token_type == WHILE)
        {
            lexer.UngetToken(token);
            trueBranchParseWhile(index);
        }
        else{
            cout<<"\nparse_smt_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_smt_err1\n";
        exit(1);
    }
}

bool trueBranchParseAssign(int index)
{
    StatementNode *newStatement = new StatementNode;
    AssignmentStatement *newAssign = new AssignmentStatement;
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        std::string assignString = token.lexeme;
        token = lexer.GetToken();
        if(token.token_type == EQUAL)
        {
            token = lexer.GetToken();
            if(token.token_type == ID || token.token_type == NUM)
            {
                Token token2;
                token2 = lexer.GetToken();
                if(token2.token_type == SEMICOLON)
                {
                    lexer.UngetToken(token2);
                    lexer.UngetToken(token);
                    //ADDED SINGLE VAL ASSIGN
                    if(token.token_type == NUM)
                    {
                        ValueNode *tempoValueNode = new ValueNode;
                        tempoValueNode->name = token.lexeme;
                        tempoValueNode->value = std::atoi(token.lexeme.c_str());

                        newAssign->left_hand_side = varList.getValueNode(assignString);
                        newAssign->operand1 = tempoValueNode;
                        newAssign->op = OPERATOR_NONE;
                        newAssign->operand2 = NULL;

                        newStatement->assign_stmt = newAssign;
                        newStatement->type = ASSIGN_STMT;
                        newStatement->next = NULL;
                        //STATEMENT ADDED:
                        branchList.addToBranch(index, newStatement);
                        //varList.updateVar(assignString, std::atoi(token.lexeme.c_str()));
                    }
                    else if(token.token_type == ID)
                    {
                        newAssign->left_hand_side = varList.getValueNode(assignString);
                        newAssign->operand1 = varList.getValueNode(token.lexeme);
                        newAssign->op = OPERATOR_NONE;
                        newAssign->operand2 = NULL;

                        newStatement->assign_stmt = newAssign;
                        newStatement->type = ASSIGN_STMT;
                        newStatement->next = NULL;
                        //STATEMENT ADDED:
                        branchList.addToBranch(index, newStatement);
                        //varList.updateVar(assignString, varList.getVal(token.lexeme));
                    }
                    parse_primary();
                    token = lexer.GetToken();
                    if(token.token_type == SEMICOLON)
                    {
                        return true;
                    }
                    else{
                        cout<<"\nparse_Assign_err4\n";
                        exit(1);
                    }
                }
                else if(token2.token_type == PLUS || token2.token_type == MINUS || token2.token_type == MULT || token2.token_type == DIV)
                {
                    //assigning operator
                    newAssign->left_hand_side = varList.getValueNode(assignString);
                    if(token2.token_type == PLUS)
                    {
                        newAssign->op = OPERATOR_PLUS;
                    }
                    else if(token2.token_type == MINUS)
                    {
                        newAssign->op = OPERATOR_MINUS;
                    }
                    else if(token2.token_type == MULT)
                    {
                        newAssign->op = OPERATOR_MULT;
                    }
                    else if(token2.token_type == DIV)
                    {
                        newAssign->op = OPERATOR_DIV;
                    }
                    //assigning first operand
                    if(token.token_type == ID)
                    {
                        newAssign->operand1 = varList.getValueNode(token.lexeme);
                    }
                    else if(token.token_type == NUM)
                    {
                        ValueNode *tempoValueNode = new ValueNode;
                        tempoValueNode->name = token.lexeme;
                        tempoValueNode->value = std::atoi(token.lexeme.c_str());
                        newAssign->operand1 = tempoValueNode;
                    }
                    //assigning second operand
                    Token token3 = lexer.GetToken();
                    lexer.UngetToken(token3);
                    lexer.UngetToken(token2);
                    lexer.UngetToken(token);
                    if(token3.token_type == ID)
                    {
                        newAssign->operand2 = varList.getValueNode(token3.lexeme);
                    }
                    else if(token3.token_type == NUM)
                    {
                        ValueNode *tempoValueNodeOp2 = new ValueNode;
                        tempoValueNodeOp2->name = token3.lexeme;
                        tempoValueNodeOp2->value = std::atoi(token3.lexeme.c_str());
                        newAssign->operand2 = tempoValueNodeOp2;
                    }
                    newStatement->assign_stmt = newAssign;
                    newStatement->type = ASSIGN_STMT;
                    newStatement->next = NULL;
                    branchList.addToBranch(index, newStatement);
                    parse_expr();
                    token = lexer.GetToken();
                    if(token.token_type == SEMICOLON)
                    {
                        return true;
                    }
                    else{
                        cout<<"\nparse_Assign_err5\n";
                        exit(1);
                    }
                }
                else{
                    cout<<"\nparse_assign_smt_err6\n";
                }
            }
            else{
                cout<<"\nparse_assign_err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nparse_assign_err2 "<<token.line_no<<" "<<token.lexeme<< "\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_assign_Err1\n";
        exit(1);
    }
}

bool trueBranchParsePrint(int index)
{
    StatementNode *newNode = new StatementNode;
    PrintStatement *printSmt = new PrintStatement;
    Token token;
    token = lexer.GetToken();
    if(token.token_type == PRINT)
    {
        token = lexer.GetToken();
        if(token.token_type == ID)
        {
            printSmt->id = varList.getValueNode(token.lexeme);
            newNode->type = PRINT_STMT;
            newNode->next = NULL;
            newNode->print_stmt = printSmt;
            branchList.addToBranch(index, newNode);
            //addToNode(newNode);
            token = lexer.GetToken();
            if(token.token_type == SEMICOLON)
            {
                return true;
            }
            else{
                cout<<"\nprint_err1\n";
                exit(1);
            }
        }
        else{
            cout<<"\nprinterr2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nprint_err3\n";
        exit(1);
    }
}

bool trueBranchParseIf(int index)
{
    StatementNode *newNode = new StatementNode;
    newNode->type = IF_STMT;
    StatementNode *buffer = new StatementNode;
    buffer->next = NULL;
    buffer->type = NOOP_STMT;
    newNode->next = buffer;
    IfStatement *newIf = new IfStatement;
    Token token;
    token = lexer.GetToken();
    if(token.token_type == IF)
    {
        token = lexer.GetToken();
        if(token.token_type == ID || token.token_type == NUM)
        {
            //GETS OPERAND 1
            if(token.token_type == ID)
            {
                newIf->condition_operand1 = varList.getValueNode(token.lexeme);
            }
            else if(token.token_type == NUM)
            {
                ValueNode *tempOperand1 = new ValueNode;
                tempOperand1->value = std::atoi(token.lexeme.c_str());
                tempOperand1->name = token.lexeme;
                newIf->condition_operand1 = tempOperand1;
            }
            Token token2 = lexer.GetToken();
            Token token3 = lexer.GetToken();

            //GET RELOP
            if(token2.token_type == GREATER)
            {
                newIf->condition_op = CONDITION_GREATER;
            }
            else if(token2.token_type == LESS)
            {
                newIf->condition_op = CONDITION_LESS;
            }
            else if(token2.token_type == NOTEQUAL)
            {
                newIf->condition_op = CONDITION_NOTEQUAL;
            }

            //GET OPERAND 2
            if(token3.token_type == ID)
            {
                newIf->condition_operand2 = varList.getValueNode(token3.lexeme);
            }
            else if(token3.token_type == NUM)
            {
                ValueNode *tempOperand2 = new ValueNode;
                tempOperand2->value = std::atoi(token3.lexeme.c_str());
                tempOperand2->name = token3.lexeme;
                newIf->condition_operand2 = tempOperand2;
            }
            lexer.UngetToken(token3);
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_condition();
            token = lexer.GetToken();
            if(token.token_type == LBRACE)
            {
                //ADDED BRANCHES:
                newIf->false_branch = buffer;
                //StatementNode *tempTrueBranch = new StatementNode;
                lexer.UngetToken(token);
                int returnIndex = trueIndex + 1;
                trueIndex = trueIndex + 1;
                branchList.addBranch();
                trueBranchParseBody(returnIndex);
                branchList.addToBranch(returnIndex, buffer);
                newIf->true_branch = branchList.getBranch(returnIndex);
                newNode->if_stmt = newIf;
                branchList.addToBranch(index, newNode);
                //branchList.printBranches();
                return true;
            }
            else{
                cout<<"\nif_smt_Err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nif_smt_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nif_smt_err1\n";
        exit(1);
    }
}


bool parse_condition()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == ID || token.token_type == NUM)
    {
        lexer.UngetToken(token);
        parse_primary();
        token = lexer.GetToken();
        if(token.token_type == GREATER || token.token_type == LESS || token.token_type == NOTEQUAL)
        {
            lexer.UngetToken(token);
            parse_relop();
            token = lexer.GetToken();
            if(token.token_type == ID || token.token_type == NUM)
            {
                lexer.UngetToken(token);
                parse_primary();
                return true;
            }
            else{
                cout<<"\nparse_condition_err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nparse_condition_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_condition_err\n";
        exit(1);
    }
}

bool parse_relop()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == GREATER || token.token_type == LESS || token.token_type == NOTEQUAL)
    {
        return true;
    }
    else{
        cout<<"\nparse_relop_err\n";
        exit(1);
    }
}

bool parse_while_smt()
{
    //SETUP:
    StatementNode *newNode = new StatementNode;
    IfStatement *newIf = new IfStatement;
    StatementNode *buffer = new StatementNode;
    buffer->type = NOOP_STMT;
    buffer->next = NULL;
    newNode->next = buffer;
    newNode->type = IF_STMT;
    newIf->false_branch = buffer;

    Token token;
    token = lexer.GetToken();
    if(token.token_type == WHILE)
    {
        token = lexer.GetToken();
        if(token.token_type == ID || token.token_type == NUM)
        {
            if(token.token_type == ID)
            {
                newIf->condition_operand1 = varList.getValueNode(token.lexeme);
            }
            else if(token.token_type == NUM)
            {
                ValueNode *tempNum = new ValueNode;
                tempNum->value = std::atoi(token.lexeme.c_str());
                tempNum->name = token.lexeme;
                newIf->condition_operand1 = tempNum;
            }
            Token token2 = lexer.GetToken();
            Token token3 = lexer.GetToken();

            //GET RELOP
            if(token2.token_type == GREATER)
            {
                newIf->condition_op = CONDITION_GREATER;
            }
            else if(token2.token_type == LESS)
            {
                newIf->condition_op = CONDITION_LESS;
            }
            else if(token2.token_type == NOTEQUAL)
            {
                newIf->condition_op = CONDITION_NOTEQUAL;
            }
            //operand2
            if(token3.token_type == ID)
            {
                newIf->condition_operand2 = varList.getValueNode(token3.lexeme);
            }
            else if(token3.token_type == NUM)
            {
                ValueNode *tempNum2 = new ValueNode;
                tempNum2->value = std::atoi(token3.lexeme.c_str());
                tempNum2->name = token3.lexeme;
                newIf->condition_operand2 = tempNum2;
            }
            lexer.UngetToken(token3);
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_condition();
            token = lexer.GetToken();
            if(token.token_type == LBRACE)
            {
                int returnIndex = trueIndex + 1;
                trueIndex = trueIndex + 1;
                branchList.addBranch();
                lexer.UngetToken(token);
                trueBranchParseBody(returnIndex);
                StatementNode *goToThere = new StatementNode;
                goToThere->type = GOTO_STMT;
                goToThere->next = NULL;
                GotoStatement *youGoToThere = new GotoStatement;
                youGoToThere->target = newNode;
                goToThere->goto_stmt = youGoToThere;
                branchList.addToBranch(returnIndex, goToThere);
                branchList.addToBranch(returnIndex, buffer);
                newIf->true_branch = branchList.getBranch(returnIndex);
                newNode->if_stmt = newIf;
                addToNode(newNode);
                //branchList.printBranches();
                //parse_body();
                return true;
            }
            else{
                cout<<"\nwhile_Err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nwhile_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nWhile_err1\n";
        exit(1);
    }
}

bool trueBranchParseWhile(int index)
{
    //SETUP:
    StatementNode *newNode = new StatementNode;
    IfStatement *newIf = new IfStatement;
    StatementNode *buffer = new StatementNode;
    buffer->type = NOOP_STMT;
    buffer->next = NULL;
    newNode->next = buffer;
    newNode->type = IF_STMT;
    newIf->false_branch = buffer;

    Token token;
    token = lexer.GetToken();
    if(token.token_type == WHILE)
    {
        token = lexer.GetToken();
        if(token.token_type == ID || token.token_type == NUM)
        {
            if(token.token_type == ID)
            {
                newIf->condition_operand1 = varList.getValueNode(token.lexeme);
            }
            else if(token.token_type == NUM)
            {
                ValueNode *tempNum = new ValueNode;
                tempNum->value = std::atoi(token.lexeme.c_str());
                tempNum->name = token.lexeme;
                newIf->condition_operand1 = tempNum;
            }
            Token token2 = lexer.GetToken();
            Token token3 = lexer.GetToken();

            //GET RELOP
            if(token2.token_type == GREATER)
            {
                newIf->condition_op = CONDITION_GREATER;
            }
            else if(token2.token_type == LESS)
            {
                newIf->condition_op = CONDITION_LESS;
            }
            else if(token2.token_type == NOTEQUAL)
            {
                newIf->condition_op = CONDITION_NOTEQUAL;
            }
            //operand2
            if(token3.token_type == ID)
            {
                newIf->condition_operand2 = varList.getValueNode(token3.lexeme);
            }
            else if(token3.token_type == NUM)
            {
                ValueNode *tempNum2 = new ValueNode;
                tempNum2->value = std::atoi(token3.lexeme.c_str());
                tempNum2->name = token3.lexeme;
                newIf->condition_operand2 = tempNum2;
            }
            lexer.UngetToken(token3);
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_condition();
            token = lexer.GetToken();
            if(token.token_type == LBRACE)
            {
                int returnIndex = trueIndex + 1;
                trueIndex = trueIndex + 1;
                branchList.addBranch();
                lexer.UngetToken(token);
                trueBranchParseBody(returnIndex);
                StatementNode *goToThere = new StatementNode;
                goToThere->type = GOTO_STMT;
                goToThere->next = NULL;
                GotoStatement *youGoToThere = new GotoStatement;
                youGoToThere->target = newNode;
                goToThere->goto_stmt = youGoToThere;
                branchList.addToBranch(returnIndex, goToThere);
                branchList.addToBranch(returnIndex, buffer);
                newIf->true_branch = branchList.getBranch(returnIndex);
                newNode->if_stmt = newIf;
                branchList.addToBranch(index, newNode);
                //branchList.printBranches();
                //parse_body();
                return true;
            }
            else{
                cout<<"\nwhile_Err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nwhile_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nWhile_err1\n";
        exit(1);
    }
}

bool parse_switch_smt()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == SWITCH)
    {
        token = lexer.GetToken();
        if(token.token_type == ID)
        {
            token = lexer.GetToken();
            if(token.token_type == LBRACE)
            {
                token = lexer.GetToken();
                if(token.token_type == CASE)
                {
                    lexer.UngetToken(token);
                    parse_case_list();
                    token = lexer.GetToken();
                    if(token.token_type == RBRACE)
                    {
                        return true;
                    }
                    else if(token.token_type == DEFAULT)
                    {
                        lexer.UngetToken(token);
                        parse_default_case();
                        token = lexer.GetToken();
                        if(token.token_type == RBRACE)
                        {
                            return true;
                        }
                        else{
                            cout<<"\nswitch_err6\n";
                            exit(1);
                        }
                    }
                    else{
                        cout<<"\nswitch_err5\n";
                        exit(1);
                    }
                }
                else{
                    cout<<"\nswitch_err4\n";
                    exit(1);
                }
            }
            else{
                cout<<"\nswitch_err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nswitch_err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nswitch-err1\n";
        exit(1);
    }
}

bool parse_case_list()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == CASE)
    {
        lexer.UngetToken(token);
        parse_case();
        token = lexer.GetToken();
        if(token.token_type == CASE)
        {
            lexer.UngetToken(token);
            parse_case_list();
            token = lexer.GetToken();
            if(token.token_type == RBRACE || token.token_type == DEFAULT)
            {
                lexer.UngetToken(token);
                return true;
            }
            else{
                cout<<"\ncase_list_err3\n";
                exit(1);
            }
        }
        else if(token.token_type == RBRACE || token.token_type == DEFAULT)
        {
            lexer.UngetToken(token);
            return true;
        }
        else{
            cout<<"\ncase_list_Err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nparse_case_listerr\n";
        exit(1);
    }
}

bool parse_case()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == CASE)
    {
        token = lexer.GetToken();
        if(token.token_type == NUM)
        {
            token = lexer.GetToken();
            if(token.token_type == COLON)
            {
                token = lexer.GetToken();
                if(token.token_type == LBRACE)
                {
                    lexer.UngetToken(token);
                    parse_body();
                    return true;
                }
                else{
                    cout<<"\ncase__err4\n";
                }
            }
            else{
                cout<<"\ncase__err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\ncase err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\ncase__Err\n";
        exit(1);
    }
}


bool parse_default_case()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == DEFAULT)
    {
        token = lexer.GetToken();
        if(token.token_type == COLON)
        {
            token = lexer.GetToken();
            if(token.token_type == LBRACE)
            {
                lexer.UngetToken(token);
                parse_body();
                return true;
            }
            else{
                cout<<"\nswitch_err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nswitch_Err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nswitch_Err1\n";
        exit(1);
    }

}

bool parse_for_smt()
{
    Token token;
    token = lexer.GetToken();
    if(token.token_type == FOR)
    {
        token = lexer.GetToken();
        if(token.token_type == LPAREN)
        {
            token = lexer.GetToken();
            if(token.token_type == ID)
            {
                lexer.UngetToken(token);
                parse_assign_smt();
                token = lexer.GetToken();
                if(token.token_type == ID || token.token_type == NUM)
                {
                    lexer.UngetToken(token);
                    parse_condition();
                    token = lexer.GetToken();
                    if(token.token_type == SEMICOLON)
                    {
                        token = lexer.GetToken();
                        if(token.token_type == ID || token.token_type == NUM)
                        {
                            lexer.UngetToken(token);
                            parse_assign_smt();
                            token = lexer.GetToken();
                            if(token.token_type == RPAREN)
                            {
                                token = lexer.GetToken();
                                if(token.token_type == LBRACE)
                                {
                                    lexer.UngetToken(token);
                                    parse_body();
                                    return true;
                                }
                                else{
                                    cout<<"\nfor_err8\n";
                                    exit(1);
                                }
                            }
                            else{
                                cout<<"\nfor_err7\n";
                                exit(1);
                            }
                        }
                        else{
                            cout<<"\nfor_err6\n";
                            exit(1);
                        }
                    }
                    else{
                        cout<<"\nfor_err5\n";
                        exit(1);
                    }

                }
                else{
                    cout<<"\nfor_err4\n";
                    exit(1);
                }
            }
            else{
                cout<<"\nfor_err3\n";
                exit(1);
            }
        }
        else{
            cout<<"\nfor_Err2\n";
            exit(1);
        }
    }
    else{
        cout<<"\nfor_err1\n";
        exit(1);
    }
}




