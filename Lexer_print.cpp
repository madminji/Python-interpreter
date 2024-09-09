#include "Lexer_print.hpp"
#include <cstring>
#include <cctype>
#include <iostream>

// Constructor initializes the Lexer with the input code
Lexer_print::Lexer_print(const char* input) : input(input), currentIndex(0) {}

// Function to retrieve the next token from the input code
Token_lex Lexer_print::getNextToken() {
    // Check if the end of the input code has been reached
    if (currentIndex >= strlen(input)) {
        return { EndOfFile, nullptr };
    }

    // Get the current character from the input code
    char currentChar = input[currentIndex++];

    // Handle whitespace
    if (currentChar == ' ' || currentChar == '\t') {
        return getNextToken(); // Skip whitespace
    }

    // Handle newlines
    if (currentChar == '\n' || currentChar == '\r') {
        return { Newline, nullptr };
    }

    // Handle identifiers and keywords
    if (isalpha(currentChar) || currentChar == '_') {
        const char* start = input + currentIndex - 1;
        while (isalnum(input[currentIndex]) || input[currentIndex] == '_') {
            currentIndex++;
        }
        const char* lexeme = subString(start, currentIndex - 1);
        TokenType type = getIdentifierType(lexeme);
        return { type, lexeme };
    }

    // Handle numeric literals
    if (isdigit(currentChar) || currentChar == '.') {
        const char* start = input + currentIndex - 1;
        while (isdigit(input[currentIndex]) || input[currentIndex] == '.') {
            currentIndex++;
        }
        const char* lexeme = subString(start, currentIndex - 1);
        return { isFloat(lexeme) ? Float : Int, lexeme };
    }

    // Handle string literals enclosed in double quotes
    if (currentChar == '\"') {
        const char* start = input + currentIndex;
        while (input[currentIndex] != '\"' && input[currentIndex] != '\0') {
            currentIndex++;
        }
        if (input[currentIndex] == '\"') {
            currentIndex++; // Consume the closing quote
            const char* lexeme = subString(start, currentIndex - 2);
            return { String, lexeme };
        }
        else {
            return { Error, nullptr }; // Unterminated string literal
        }
    }

    // Handle string literals enclosed in single quotes
    if (currentChar == ('\'')) {
        const char* start = input + currentIndex;
        while (input[currentIndex] != ('\'') && input[currentIndex] != '\0') {
            currentIndex++;
        }
        if (input[currentIndex] == ('\'')) {
            currentIndex++; // Consume the closing quote
            const char* lexeme = subString(start, currentIndex - 2);
            return { String, lexeme };
        }
        else {
            return { Error, nullptr }; // Unterminated string literal
        }
    }

    // Handle operators and punctuators
    const char* start = input + currentIndex - 1;
    while (isOperatorOrPunctuator(input[currentIndex])) {
        currentIndex++;
    }
    const char* lexeme = subString(start, currentIndex - 1);
    TokenType type = getOperatorOrPunctuatorType(lexeme);
    return { type, lexeme };
}

const char* Lexer_print::subString(const char* start, int end) {
    // Calculate the length of the substring
    int length = end - (start - input) + 1;

    // Allocate memory for the substring plus null terminator
    char* substring = new char[length + 1];

    // Copy each character from the source to the destination
    for (int i = 0; i < length; ++i) {
        substring[i] = start[i];
    }

    // Null-terminate the string
    substring[length] = '\0';

    // Return the created substring
    return substring;
}

// Function to determine the type of an identifier (keyword or identifier)
TokenType Lexer_print::getIdentifierType(const char* identifier) {
    if (strcmp(identifier, "def") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "print") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "return") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "while") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "break") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "continue") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "if") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "elif") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "else") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "True") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "False") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "None") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "and") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "or") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "not") == 0) {
        return Keyword;
    }
    else if (strcmp(identifier, "pass") == 0) {
        return Keyword;
    }
    else {
        return Identifier;
    }
}

// Function to check if a string represents a floating-point number
bool Lexer_print::isFloat(const char* str) {
    return strchr(str, '.') != nullptr;
}

// Function to check if a character is an operator or punctuator
bool Lexer_print::isOperatorOrPunctuator(char ch) {
    return ch == '+';
    return ch == '-';
    return ch == '*';
    return ch == '/';
    return ch == '=';
    return ch == '<';
    return ch == '>';
    return ch == '!';
    return ch == '&';
    return ch == '|';
    return ch == '^';
    return ch == '~';
    return ch == '%';
    return ch == ':';
    return ch == ',';
    return ch == '(';
    return ch == ')';
    return ch == '{';
    return ch == '}';
    return ch == '[';
    return ch == ']';
    return ch == '.';
    return ch == '\n';
    return ch == '\r';
}

// Function to determine the type of an operator or punctuator
TokenType Lexer_print::getOperatorOrPunctuatorType(const char* op) {
    switch (op[0]) {
    case '+':
        return Plus;
    case '-':
        return Minus;
    case '*':
        return Star;
    case '/':
        return Slash;
    case '=':
        return op[1] == '=' ? EqualEqual : Equals;
    case '<':
        return op[1] == '=' ? LessEqual : Less;
    case '>':
        return op[1] == '=' ? GreaterEqual : Greater;
    case '!':
        return op[1] == '=' ? BangEqual : Bang;
    case '&':
        return op[1] == '=' ? AndEqual : Ampersand;
    case '|':
        return op[1] == '=' ? OrEqual : Pipe;
    case '^':
        return op[1] == '=' ? XorEqual : Caret;
    case '~':
        return Tilde;
    case '%':
        return op[1] == '=' ? ModEqual : Mod;
    case ':':
        return Colon;
    case ',':
        return Comma;
    case '(':
        return LeftParen;
    case ')':
        return RightParen;
    case '{':
        return LeftBrace;
    case '}':
        return RightBrace;
    case '[':
        return LeftBrace;
    case ']':
        return RightBrace;
    case '.':
        return Dot;
    case '\n':
    case '\r':
        return Newline;
    default:
        return Error;
    }
}


