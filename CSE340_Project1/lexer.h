

/*
*
* Arsalan Nawabi
*      and
* Junichi koizomi
*
*

*/








#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    IF, WHILE, DO, THEN, PRINT,
    PLUS, MINUS, DIV, MULT,
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ,
    DOT, NUM, ID, ERROR , REALNUM, BASE08NUM, BASE16NUM// TODO: Add labels for new token types here
} TokenType;

class Token {
  public:                                              // Class token and make it public for the access to  the user.
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;                                             //    make the token type . 
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

  private:                                 
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;                                                   // Gett all the Private labels bools, token and string 
    bool CheckInput(std::string);
    bool SkipSpace();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
    std::string ScanNum();
    std::string ScanBase08Num();
    std::string ScanBase16Num();
    std::string ScanRealNum();
     


};

#endif  //__LEXER__H__
