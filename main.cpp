#include "Lexer_print.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "ByteCodeGenerator.hpp"
#include "Vm.hpp"
#include "max.hpp"

#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")

using namespace std;

inline void show_tokens(const vector<Token>& tokens) {
    for (const auto& token : tokens) {
        cout << token << '\n';
    }
    cout << flush;
}

void printToken(Token_lex token, int lineNumber) {
    const char* typeStrings[] = {
        "Keyword",
        "Class",
        "Print", "Def", "Return",
        "While", "Break", "Continue",
        "If", "Elif", "Else",
        "True", "False", "None",
        "And", "Or", "Not",
        "Pass",

        "Operator",
        "Plus", "Minus",
        "Star", "DoubleStar",
        "Slash", "DoubleSlash", "Mod",
        "Pipe", "Caret", "Ampersand", "Tilde",
        "LeftShift", "RightShift",
        "Less", "LessEqual",
        "Greater", "GreaterEqual",
        "Equals", "EqualEqual",
        "Bang", "BangEqual",
        "PlusEqual", "MinusEqual",
        "StarEqual", "SlashEqual", "ModEqual",
        "AndEqual", "OrEqual", "XorEqual",
        "LeftShiftEqual",
        "RightShiftEqual",
        "Dot",

        "Punctuator",
        "LeftParen", "RightParen",
        "LeftBrace", "RightBrace",
        "Comma", "Colon", "Semicolon",
        "Indent", "Dedent",
        "Newline",
        "EndOfFile",

        "Identifier",
        "Literal",
        "Int", "Float", "String",

        // Others
        "Error",
    };

    std::cout << "Line: " << lineNumber << ", Type: " << typeStrings[token.type] << ", Lexeme: ";
    if (token.lexeme != nullptr) {
        std::cout << token.lexeme;
    }
    else {
        std::cout << "NULL";
    }
    std::cout << std::endl;
}

int main() {
    // Read Python code from a file
    std::cout << "############Lexical Analysis############\n";

    //const char* const filename = "input1.py";
    //const char* const filename = "input2.py";
    const char* const filename = "input3.py";

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the input file." << std::endl;
        return 1;
    }

    std::string pythonCode((std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>());

    inputFile.close();

    // Tokenize and print tokens
    Lexer_print Lexer_print(pythonCode.c_str());
    Token_lex token;
    int lineNumber = 1;

    do {
        token = Lexer_print.getNextToken();
        printToken(token, lineNumber);
        if (token.type == Newline) {
            lineNumber++;
        }
    } while (token.type != EndOfFile);

    std::cout << "############Grammatical Analysis############\n";

    ifstream inputFile2(filename);

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    string source((istreambuf_iterator<char>(inputFile2)), {});

    try {
        Lexer lexer(source);
        vector<Token> tokens = lexer.scanTokens();
        Parser parser(tokens);

        ProgramNode* root = parser.parse();

        BytecodeGenerator Emitter = BytecodeGenerator();
        root->emit_bytecode(Emitter);
        VirtualMachine Vm;
        Vm.execute(Emitter.instructions);

        Interpreter interpreter;
        interpreter.interpret(root);
        std::cout << "############Running END############\n";

    } catch (const runtime_error& err) {
        cerr << err.what() << '\n';
        return EXIT_FAILURE;
    }


    return 0;
}


