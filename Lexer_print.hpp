// Lexer_print.hpp

#pragma once
#include "token.hpp"

class Lexer_print {
private:
    const char *input;
    int currentIndex;

public:
    Lexer_print(const char *input);
    Token_lex getNextToken();

private:
    const char *subString(const char *start, int end);
    TokenType getIdentifierType(const char *identifier);
    bool isFloat(const char *str);
    bool isOperatorOrPunctuator(char ch);
    TokenType getOperatorOrPunctuatorType(const char *op);
};
