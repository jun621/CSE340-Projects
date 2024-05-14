#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "lexer.h"
#include "compiler.h"



int indexOfToken(std::string str);
bool checkPrimary(int index);
bool checkRelop(int index);
bool checkOp(int index);
bool checkCondition(int index);
bool checkExpression(int index);
std::vector<Token> parseCondition();
std::vector<Token> parseExpression();
std::vector<Token> parseAssignment();
void parseOutput();
void parseInput();
void parseWhile();
void parseIf();
void parseStatementList();
void parseStatement();
void parseBody();
void parseCaseList(Token switchId);
void parseSwitch();
void parseFor();
void parseInputs();
void parseIdList();
void parseProgram(); 






#endif 