
/* 
* 
* Arsalan Nawabi 
*      and 
* Junichi koizomi 
* 
* 

*/



#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR" ,"REALNUM", "BASE08NUM", "BASE16NUM" // Adding some labels to modify it later. 
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };





bool ispdigit(char c) {
    return c >= '1' && c <= '9';
}
                                                                       
bool ispdigit8(char c) {
    return c >= '1' && c <= '7';
}

bool ispdigit16(char c) {
    return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'F');
}

bool isdigit(char c) {
    return c == '0' ||  c >= '1' && c <= '9' ;
}

bool isdigit8(char c) {
    return c == '0' ||  c >= '1' && c <= '7' ;
}

bool isdigit16(char c) {
    return c == '0' ||  (c >= '1' && c <= '9') || (c >= 'A' && c <= 'F');
}

void Token::Print()
{                                                                     
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}



bool LexicalAnalyzer::CheckInput(string s)
{
    char c;

    string ungotChars;

    for (int i = 0; i < s.length(); i++) {
        input.GetChar(c);
        if (input.EndOfInput() || c != s[i]) {
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }

            for (int j = ungotChars.length() - 1; j >= 0; j--) {
                input.UngetChar(ungotChars[j]);
            }
            return false;
        }
        ungotChars += c;
    }

    return true;
}

Token LexicalAnalyzer::ScanNumber()
{
    string lexer;
    TokenType tokenType;

    lexer = ScanRealNum();
    if (lexer.length() > 0) {
        tokenType = REALNUM;
    }
    else {
        lexer = ScanBase08Num();
        if (lexer.length() > 0) {
            tokenType = BASE08NUM;
        }
        else {
            lexer = ScanBase16Num();
            if (lexer.length() > 0) {
                tokenType = BASE16NUM;
            }
            else {
                lexer = ScanNum();
                if (lexer.length() > 0) {
                    tokenType = NUM;
                }
                else {
                    tokenType = ERROR;
                }
            }
        }
    }

    tmp.token_type = tokenType;
    tmp.line_no = line_no;
    tmp.lexeme = lexer;
    return tmp;

}



string LexicalAnalyzer::ScanNum()
{
    char c;
    string temp = "";

    input.GetChar(c);
    temp += c;
    if (isdigit(c)) {
        if (c == '0') {
            return temp;
        }
        else {
            input.GetChar(c);
            while (!input.EndOfInput() && isdigit(c)) {
                temp += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            return temp;
        }
    }
    input.UngetString(temp);
    return "";
}





string LexicalAnalyzer::ScanRealNum()
{
    char currentChar;
    string scannedNumber = "";

    input.GetChar(currentChar);
    scannedNumber += currentChar;

    if (currentChar == '0' && CheckInput(".")) {
        scannedNumber += '.';
        input.GetChar(currentChar);
        while (!input.EndOfInput() && currentChar == '0') {
            scannedNumber += currentChar;
            input.GetChar(currentChar);
        }
        if (!input.EndOfInput() && ispdigit(currentChar)) {
            while (!input.EndOfInput() && isdigit(currentChar)) {
                scannedNumber += currentChar;
                input.GetChar(currentChar);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(currentChar);
            }
            return scannedNumber;
        }
    }
    else if (ispdigit(currentChar)) {
        input.GetChar(currentChar);
        while (!input.EndOfInput() && isdigit(currentChar)) {
            scannedNumber += currentChar;
            input.GetChar(currentChar);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(currentChar);
        }
        if (CheckInput(".")) {
            scannedNumber += '.';
            bool isScannedDigit = false;
            input.GetChar(currentChar);
            while (!input.EndOfInput() && isdigit(currentChar)) {
                isScannedDigit = true;
                scannedNumber += currentChar;
                input.GetChar(currentChar);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(currentChar);
            }
            if (isScannedDigit) {
                return scannedNumber;
            }
        }
    }
    input.UngetString(scannedNumber);
    return "";
}



string LexicalAnalyzer::ScanBase16Num() {
    char c;
    string scanned = "";

    input.GetChar(c);  
    scanned += c; 

    if (c == '0') {
        input.GetChar(c); 
        if (c == 'x') { 
            scanned += c;  
            input.GetChar(c);  
            while (!input.EndOfInput() && isdigit16(c)) {
                scanned += c;  
                input.GetChar(c);  
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);  
            }
            return scanned; 
        }
        else {
            input.UngetChar(c);  
        }
    }

    input.UngetString(scanned); 
    return "";  
}






string LexicalAnalyzer::ScanBase08Num()
{
    char currentChar;
    string scannedNumber = "";

    input.GetChar(currentChar);
    scannedNumber += currentChar;                                             
    if (currentChar == '0' && CheckInput("x08")) {                              
        scannedNumber += "x08";
        return scannedNumber;
    }
    else if (ispdigit8(currentChar)) {
        input.GetChar(currentChar);
        while (!input.EndOfInput() && isdigit8(currentChar)) {                    
            scannedNumber += currentChar;
            input.GetChar(currentChar);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(currentChar);
        }
        if (CheckInput("x08")) {
            scannedNumber += "x08";
            return scannedNumber;
        }
    }
    input.UngetString(scannedNumber);
    return "";
}










Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);                                         // Getting the char. 

    if (isalpha(c)) {                                           // need while loop and if statements to loop and ckeck the char and the input num. 
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {                                                  // All the switch cases for tokens 
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {                                 // if and else if statments for the token. 
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();                                     // returning the scanned numbers and checking the alph.
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }                                                                                 // end of the main 
}                                                                  // End of the class. 
