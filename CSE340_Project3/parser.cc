#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include "compiler.h"
#include "lexer.h"
#include "parser.h"

using namespace std;

LexicalAnalyzer* lexer = new LexicalAnalyzer();

std::vector<InstructionNode*> instructionList;

std::string address[1000];


InstructionNode* activate;

InstructionNode* currentNode;




int indexOfToken(std::string str) {
    for (int i = 0; i < 1000; i++) {
        if (address[i] == str) {
            return i;
        }
    }
    return -1;
}


bool checkPrimary(int index) {
    TokenType type = lexer->peek(index).token_type;
    return (type == ID || type == NUM);
}

bool checkRelop(int index) {
    TokenType type = lexer->peek(index).token_type;
    return (type == GREATER || type == LESS || type == NOTEQUAL);
}

bool checkOp(int index) {
    TokenType type = lexer->peek(index).token_type;
    return (type == PLUS || type == MINUS || type == MULT || type == DIV);
}


bool checkCondition(int index) {

    bool isPrimary = checkPrimary(index);
    bool isRelop = checkRelop(index + 1);
    bool isPrimary2 = checkPrimary(index + 2);

    return isPrimary && isRelop && isPrimary2;
}

bool checkExpression(int index) {
    
    bool isPrimary = checkPrimary(index);
    bool isOp = checkOp(index + 1);
    bool isPrimary2 = checkPrimary(index + 2);

    return isPrimary && isOp && isPrimary2;
}



std::vector<Token> parseCondition() {
    std::vector<Token> con;

    if (checkCondition(1)) {
        for (int i = 0; i < 3; ++i) {
            con.push_back(lexer->GetToken());
        }
    }

    return con;
}


std::vector<Token> parseExpression() {
    std::vector<Token> ex;

    if (checkExpression(1)) {
        for (int i = 0; i < 3; ++i) {
            ex.push_back(lexer->GetToken());
        }
    }

    return ex;
}


std::vector<Token> parseAssignment() {
    std::vector<Token> assignment;

    InstructionNode* assignInstruction = new InstructionNode();
    assignInstruction->type = ASSIGN;

    for (int i = 0; i < 3; ++i) {
        Token token = lexer->GetToken();
        assignment.push_back(token);
    }

    int lhsIndex = indexOfToken(assignment[0].lexeme);
    assignInstruction->assign_inst.left_hand_side_index = lhsIndex;

    int op1Index;
    if (assignment[2].token_type != ID) {
        int op1Val = std::stoi(assignment[2].lexeme);

        mem[next_available] = op1Val;
        address[next_available] = assignment[2].lexeme;
        next_available++;

        op1Index = indexOfToken(assignment[2].lexeme);
    }
    else {
        op1Index = indexOfToken(assignment[2].lexeme);
    }

    assignInstruction->assign_inst.operand1_index = op1Index;

    if (lexer->peek(1).token_type != SEMICOLON) {
        for (int i = 0; i < 2; ++i) {
            Token token = lexer->GetToken();
            assignment.push_back(token);
        }

        ArithmeticOperatorType aop;
        if (assignment[3].token_type == PLUS) {
            aop = OPERATOR_PLUS;
        }
        else if (assignment[3].token_type == MINUS) {
            aop = OPERATOR_MINUS;
        }
        else if (assignment[3].token_type == MULT) {
            aop = OPERATOR_MULT;
        }
        else if (assignment[3].token_type == DIV) {
            aop = OPERATOR_DIV;
        }

        int op2Index;
        if (assignment[4].token_type != ID) {
            int op2Val = std::stoi(assignment[4].lexeme);
            mem[next_available] = op2Val;
            address[next_available] = assignment[4].lexeme;
            next_available++;
            op2Index = indexOfToken(assignment[4].lexeme);
        }
        else {
            op2Index = indexOfToken(assignment[4].lexeme);
        }

        assignInstruction->assign_inst.op = aop;
        assignInstruction->assign_inst.operand2_index = op2Index;
    }
    else {
        assignInstruction->assign_inst.op = OPERATOR_NONE;
    }

    lexer->GetToken();

    currentNode->next = assignInstruction;
    assignInstruction->next = NULL;
    currentNode = assignInstruction;

    return assignment;
}


void parseOutput() {
   
    lexer->GetToken();

    
    std::string varName = lexer->GetToken().lexeme;

   
    InstructionNode* output = new InstructionNode();
    output->type = OUT;


    int varIndex = indexOfToken(varName);


    output->output_inst.var_index = varIndex;

 
    currentNode->next = output;

   
    output->next = nullptr;

   
    currentNode = output;

  
    lexer->GetToken();
}



void parseInput() {
  
    lexer->GetToken();

   
    std::string varName = lexer->GetToken().lexeme;


    InstructionNode* input = new InstructionNode();
    input->type = IN;


    int varIndex = indexOfToken(varName);

  
    input->input_inst.var_index = varIndex;


    if (currentNode != nullptr) {
        currentNode->next = input;
    } else {
       
        currentNode = input;
    }

  
    input->next = nullptr;

   
    currentNode = input;

    
    lexer->GetToken();
}




void parseWhile() {
    if (lexer->peek(1).token_type != WHILE)
        return;

    lexer->GetToken();

    std::vector<Token> condition = parseCondition();

    if (condition.empty())
        return;

    InstructionNode* cJump = new InstructionNode();
    cJump->type = CJMP;
    cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);

    // Map the token types to conditional operator types
    ConditionalOperatorType cop;
    switch (condition[1].token_type) {
    case GREATER:
        cop = CONDITION_GREATER;
        break;
    case LESS:
        cop = CONDITION_LESS;
        break;
    default:
        cop = CONDITION_NOTEQUAL;
        break;
    }

    // Get the index of the second operand
    int secondIndex;
    if (condition[2].token_type != ID) {
        int cop1Val = std::stoi(condition[2].lexeme);
        mem[next_available] = cop1Val;
        address[next_available] = condition[2].lexeme;
        next_available++;
        secondIndex = indexOfToken(condition[2].lexeme);
    }
    else {
        secondIndex = indexOfToken(condition[2].lexeme);
    }

    // Set the conditional operator and the second operand index
    cJump->cjmp_inst.condition_op = cop;
    cJump->cjmp_inst.operand2_index = secondIndex;

    // Add the cJump instruction to the current node
    currentNode->next = cJump;
    cJump->next = NULL;
    currentNode = cJump;

    // Parse the body of the while loop
    parseBody();

    // Link back to the start of the loop
    currentNode->next = cJump;

    // Add a NOOP instruction as the target of the conditional jump
    InstructionNode* noop = new InstructionNode();
    noop->type = NOOP;
    cJump->cjmp_inst.target = noop;
    noop->next = NULL;

    // Move the current pointer to the NOOP instruction
    currentNode = noop;
}



void parseIf() {
    lexer->GetToken();

    std::vector<Token> condition = parseCondition();

    if (condition.empty())
        return;

    InstructionNode* cJump = new InstructionNode();
    cJump->type = CJMP;
    cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);

    // Map the token types to conditional operator types
    ConditionalOperatorType cot;
    switch (condition[1].token_type) {
    case GREATER:
        cot = CONDITION_GREATER;
        break;
    case LESS:
        cot = CONDITION_LESS;
        break;
    default:
        cot = CONDITION_NOTEQUAL;
        break;
    }

    // Set the conditional operator and the second operand index
    cJump->cjmp_inst.condition_op = cot;
    cJump->cjmp_inst.operand2_index = indexOfToken(condition[2].lexeme);

    // Add the cJump instruction to the current node
    cJump->next = NULL;
    currentNode->next = cJump;
    currentNode = cJump;

    // Parse the body of the if statement
    parseBody();

    // Add a NOOP instruction as the target of the conditional jump
    InstructionNode* noop = new InstructionNode();
    noop->type = NOOP;
    noop->next = NULL;
    currentNode->next = noop;
    cJump->cjmp_inst.target = noop;

    // Move the current pointer to the NOOP instruction
    currentNode = noop;
}



void parseStatementList() {
  
    while (lexer->peek(1).token_type != END_OF_FILE && lexer->peek(1).token_type != RBRACE) {
        
        parseStatement();
    }
}


void parseStatement() {
    
    TokenType nextTokenType = lexer->peek(1).token_type;

    switch (nextTokenType) {
    case WHILE:
        parseWhile();
        break;
    case IF:
        parseIf();
        break;
    case SWITCH:
        parseSwitch();
        break;
    case FOR:
        parseFor();
        break;
    case INPUT:
        parseInput();
        break;
    case OUTPUT:
        parseOutput();
        break;
    default:
        parseAssignment();
        break;
    }
}


void parseBody() {

    lexer->GetToken();
    parseStatementList();

    lexer->GetToken();
}

void parseCaseList(Token switchId) {

    InstructionNode* endSwitch = new InstructionNode();
    endSwitch->type = NOOP;

    do {

        if (lexer->peek(1).token_type == DEFAULT) {

            lexer->GetToken();

            lexer->GetToken();

            parseBody();
            currentNode->next = endSwitch;
        }
        else {

            lexer->GetToken();
            Token op2 = lexer->GetToken();

            int op2Index;

            int op2ValasInt = std::stoi(op2.lexeme);
            mem[next_available] = op2ValasInt;
            address[next_available] = op2.lexeme;
            next_available++;


            op2Index = indexOfToken(op2.lexeme);


            lexer->GetToken();

            InstructionNode* cJump = new InstructionNode();
            cJump->type = CJMP;

            cJump->cjmp_inst.operand1_index = indexOfToken(switchId.lexeme);
            cJump->cjmp_inst.operand2_index = op2Index;
            cJump->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

            currentNode->next = cJump;
            currentNode = cJump;

            parseBody();

            InstructionNode* noop = new InstructionNode();
            noop->type = NOOP;
            noop->next = NULL;

            cJump->cjmp_inst.target = cJump->next;
            cJump->next = noop;
            currentNode->next = endSwitch;
            currentNode = noop;

        }
    } while (lexer->peek(1).token_type != RBRACE);

    currentNode = endSwitch;

}


void parseSwitch() {
    // Check if the next token is SWITCH
    if (lexer->peek(1).token_type != SWITCH) {
        return; // Exit if the next token is not SWITCH
    }

   
    if (lexer->peek(2).token_type != ID || lexer->peek(3).token_type != LBRACE) {
        return; // Exit if the pattern is not matched
    }


    lexer->GetToken();


    Token switchId = lexer->GetToken();


    lexer->GetToken();

 
    parseCaseList(switchId);


    lexer->GetToken();
}





void parseFor() {
    // Consume 'for'
    lexer->GetToken();

    // Consume '('
    lexer->GetToken();

    // Parse the first assignment
    parseAssignment();
    InstructionNode* firstAssignment = currentNode;

    // Parse the condition
    std::vector<Token> condition = parseCondition();

    // Consume ';'
    lexer->GetToken();

    // Parse the second assignment
    parseAssignment();
    InstructionNode* secondAssignment = currentNode;

    // Consume ')'
    lexer->GetToken();

    // Create a conditional jump instruction
    InstructionNode* cJump = new InstructionNode();
    cJump->type = CJMP;
    cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);

    // Map the token types to conditional operator types
    ConditionalOperatorType cop;
    switch (condition[1].token_type) {
    case GREATER:
        cop = CONDITION_GREATER;
        break;
    case LESS:
        cop = CONDITION_LESS;
        break;
    default:
        cop = CONDITION_NOTEQUAL;
        break;
    }

    // Set the conditional operator
    cJump->cjmp_inst.condition_op = cop;

    // Get the index of the second operand
    int cop2Index;
    if (condition[2].token_type != ID) {
        // If constant, add to memory
        int cop2Val = std::stoi(condition[2].lexeme);
        mem[next_available] = cop2Val;
        address[next_available] = condition[2].lexeme;
        next_available++;
        cop2Index = indexOfToken(condition[2].lexeme);
    }
    else {
        // If ID, find its index
        cop2Index = indexOfToken(condition[2].lexeme);
    }

    // Set the index of the second operand
    cJump->cjmp_inst.operand2_index = cop2Index;

    // Link before the loop body
    firstAssignment->next = cJump;
    cJump->next = NULL;
    currentNode = cJump;

    // Parse the body of the loop
    parseBody();

    // Link after the loop body
    currentNode->next = secondAssignment;
    secondAssignment->next = cJump;

    // Create a NOOP and link it
    InstructionNode* noop = new InstructionNode();
    noop->type = NOOP;
    noop->next = NULL;

    // Set the target of the conditional jump
    cJump->cjmp_inst.target = noop;
    currentNode = noop;
}






void parseInputs() {

    while (lexer->peek(1).token_type != END_OF_FILE) {

        inputs.push_back(std::stoi(lexer->GetToken().lexeme));
    }
    lexer->GetToken();
}

void parseIdList() {
    next_available = 0;

    // Parse until a semicolon is encountered
    while (true) {
        // Check if the next token is a semicolon
        if (lexer->peek(1).token_type == SEMICOLON) {
            // Consume the semicolon and exit the loop
            lexer->GetToken();
            break;
        }

        // If the next token is not a comma, it's an identifier
        if (lexer->peek(1).token_type != COMMA) {
            // Store the identifier in the address array
            address[next_available] = lexer->GetToken().lexeme;
            // Initialize memory to 0
            mem[next_available] = 0;
            // Increment the index for the next available slot
            next_available++;
        }
        else {
            // If the next token is a comma, skip it
            lexer->GetToken();
        }
    }
}


void parseProgram() {

    InstructionNode* noop = new InstructionNode;
    noop->type = NOOP;
    currentNode = noop;
    activate = currentNode;

    parseIdList(); 

    parseBody();

    parseInputs();
}

struct InstructionNode* parse_generate_intermediate_representation()  {


    parseProgram(); 

    return activate; 

}