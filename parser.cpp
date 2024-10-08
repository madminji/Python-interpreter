#include "parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

ProgramNode* Parser::parse() {
    return parseProgram();
}

ProgramNode* Parser::parseProgram() {
 
    std::vector<AstNode*> statements;
    while (!isAtEnd()) {
        if(match(TokenType::Newline)){
            continue;
        } else {
            statements.push_back(parseStmt());
        }
    }
    BlockNode* block = new BlockNode(statements);
    return new ProgramNode(block);
}

AstNode* Parser::parseStmt() {

    if (match(TokenType::If)){
        return parseIfStmt();

    } else if (match(TokenType::While)){
        return parseWhileStmt();

    } else if (match(TokenType::Def)){
        return parseFunctionDef();

    } else if (match(TokenType::Class)) {
        return parseClassDef();

    } else {
        std::vector<AstNode*> stmtList = parseStmtList();
        while(match(TokenType::Newline)) continue;
        return new BlockNode(stmtList);
    }
}

std::vector<AstNode*> Parser::parseParameterList() {
 
    std::vector<AstNode*> parameters;
    do {
        Token tk = consume(TokenType::Name);
        AstNode* pname = new NameNode(tk);
        parameters.push_back(pname);
    } while (match(TokenType::Comma));

    return parameters;
}

AstNode* Parser::parseFunctionDef(){

    Token fname = consume(TokenType::Name);
    consume(TokenType::LeftParen);

    std::vector<AstNode*> parameters;

    if (peek().type != TokenType::RightParen) {
       parameters = parseParameterList();
    }
    consume(TokenType::RightParen);
    consume(TokenType::Colon);

    isInsideFunc++;
    AstNode* body = parseSuite();
    isInsideFunc--;

    return new FunctionNode(fname, parameters, body);
}

AstNode* Parser::parseClassDef(){

    Token kname = consume(TokenType::Name);
    consume(TokenType::Colon);
    AstNode* body = parseSuite();

    return new ClassNode(kname, body);
}

std::vector<AstNode*> Parser::parseStmtList() {

    std::vector<AstNode*> stmts;

    stmts.push_back(parseSimpleStmt());

    while(match(TokenType::Semicolon)){
        if(peek().type == TokenType::Newline) break;
        stmts.push_back(parseSimpleStmt());
    }
    if(peek().type == TokenType::Semicolon) ++current;

    return stmts;
}

AstNode* Parser::parseSimpleStmt() {

     if (match(TokenType::Print)) {
        return parsePrintStmt();
    } else if(match(TokenType::Return)){
        return parseReturnStmt();
    } else if(match(TokenType::Break)){
        return parseBreakStmt();
    } else if(match(TokenType::Continue)){
        return parseContinueStmt();
    } else if(match(TokenType::Pass)) {
        return parsePassStmt();
    } else {
        return parseAssign();
    }
}

AstNode* Parser::parseReturnStmt() {
  
    if(!isInsideFunc) error("Return outside Function.");

    Token keyword = previous();
    AstNode* expr = match({TokenType::Newline, TokenType::Semicolon}) ?
                    nullptr : parseExpr();
    return new ReturnNode(keyword, expr);
}

AstNode* Parser::parseBreakStmt() {

    if(!isInsideLoop) error("Break outside Loop.");

    Token keyword = previous();
    return new BreakNode(keyword);
}

AstNode* Parser::parseContinueStmt() {
  
    if(!isInsideLoop) error("Continue outside Loop.");

    Token keyword = previous();
    return new ContinueNode(keyword);
}

AstNode* Parser::parsePassStmt() {
 
    Token keyword = previous();
    return new PassNode(keyword);
}

AstNode* Parser::parsePrintStmt() {

    consume(TokenType::LeftParen);
    AstNode* expr = nullptr;
    if(peek().type != TokenType::RightParen) {
        expr = parseExpr();
    } consume(TokenType::RightParen);

    return new PrintNode(expr);
}

AstNode* Parser::parseSuite() {

    if(match(TokenType::Newline)) {

        while(match(TokenType::Newline)) continue;

        consume(TokenType::Indent);

        std::vector<AstNode*> stmts;

        //stmts.push_back(parseStmt());

        while(peek().type != TokenType::Dedent){

             auto r = parseStmt();

             if(dynamic_cast<BlockNode*>(r) != nullptr){

                BlockNode* stmt_list =  dynamic_cast<BlockNode*>(r);

                for(auto stmt : stmt_list->statements)
                     stmts.push_back(stmt);

             } else {
                 stmts.push_back(r);
             }
        }
        consume(TokenType::Dedent);

        return new BlockNode(stmts);

    } else {
        std::vector<AstNode*> stmtList = parseStmtList();
        consume(TokenType::Newline);
        return new BlockNode(stmtList);
    }
}

AstNode* Parser::parseIfStmt() {

    AstNode* cond = parseExpr();
    consume(TokenType::Colon);
    AstNode* trueBranch = parseSuite();

    std::vector<std::pair<AstNode*, AstNode*>> elifBranches;
    while(match(TokenType::Elif)) {
        AstNode* elifCond = parseExpr();
        consume(TokenType::Colon);
        AstNode* elifSuite = parseSuite();
        elifBranches.emplace_back(elifCond, elifSuite);
    }

    AstNode* elseBranch = nullptr;
    if(match(TokenType::Else)) {
        consume(TokenType::Colon);
        elseBranch = parseSuite();
    }

    return new IfNode(cond, trueBranch, elifBranches, elseBranch);
}

AstNode* Parser::parseWhileStmt() {

    AstNode* cond = parseExpr();
    consume(TokenType::Colon);
    isInsideLoop++;
    AstNode* body = parseSuite();
    isInsideLoop--;

    return new WhileNode(cond, body);
}

AstNode* Parser::parseAssignSimple() {
 
    AstNode* left = parseExpr();

    if(match(TokenType::Equals)) {
        Token op = previous();
        AstNode* right = parseAssignSimple();
        return new AssignNode(left, right, op);
    }
    return left;
}

AstNode* Parser::parseAssign() {

    AstNode* left = parseAssignSimple();

    if(match({TokenType::PlusEqual, TokenType::MinusEqual,
              TokenType::StarEqual, TokenType::SlashEqual, TokenType::ModEqual,
              TokenType::AndEqual, TokenType::OrEqual, TokenType::XorEqual,
              TokenType::LeftShiftEqual, TokenType::RightShiftEqual})) {
        Token op = previous();
        AstNode* right = parseExpr();

        if(left->is_name_node() or left->is_property_node()) {
            return new AssignNode(left, right, op);
        } else {
            error("Invalid compound assignment.");
            return nullptr;
        }
    }
    return left;
}

AstNode* Parser::parseExpr() {

    return parseConditionalExpr();
}

AstNode* Parser::parseConditionalExpr() {

    AstNode* left = parseDisjunction();

    if(match(TokenType::If)) {
        AstNode* condition = parseDisjunction();
        consume(TokenType::Else);
        AstNode* right = parseExpr();
        left = new TernaryOpNode(condition, left, right);
    }
    return left;
}

AstNode* Parser::parseDisjunction() {

    AstNode* left = parseConjunction();

    while (match(TokenType::Or)) {
        Token op = previous();
        AstNode* right = parseConjunction();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseConjunction() {
 
    AstNode* left = parseInversion();

    while (match(TokenType::And)) {
        Token op = previous();
        AstNode* right = parseInversion();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseInversion() {
    /*
     *   inversion ::= "not" inversion
     *                | comparison
    */
    if (match(TokenType::Not)) {
        Token op = previous();
        AstNode* right = parseInversion();
        return new UnaryOpNode(op, right);
    } else {
        return parseComparison();
    }
}

AstNode* Parser::parseComparison() {
    /*
     *   comparison ::= factor (comp_operator factor)*
     *   comp_operator ::= "<" | ">" | "==" | ">=" | "<=" | "!="
    */
    AstNode* left = parseBitwiseOr();

    while (match({TokenType::Less, TokenType::Greater,
                  TokenType::EqualEqual, TokenType::GreaterEqual,
                  TokenType::LessEqual, TokenType::BangEqual})) {
        Token op = previous();
        AstNode* right = parseBitwiseOr();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseBitwiseOr() {
    /*
     *    bitwise_or ::= bitwise_xor ("|" bitwise_xor)*
    */
    AstNode* left = parseBitwiseXor();

    while(match(TokenType::Pipe)) {
        Token op = previous();
        AstNode* right = parseBitwiseXor();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseBitwiseXor() {
    /*
     *    bitwise_xor ::= bitwise_and ("^" bitwise_and)*
    */
    AstNode* left = parseBitwiseAnd();

    while(match(TokenType::Caret)) {
        Token op = previous();
        AstNode* right = parseBitwiseAnd();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseBitwiseAnd() {
    /*
     *   bitwise_and ::= shift_expr ("&" shift_expr)*
    */
    AstNode* left = parseShiftExpr();

    while (match(TokenType::Ampersand)) {
        Token op = previous();
        AstNode* right = parseShiftExpr();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseShiftExpr() {
    /*
     *   shift_expr ::= factor (("<<" | ">>") factor)*
    */
    AstNode* left = parseFactor();

    while (match({TokenType::LeftShift, TokenType::RightShift})) {
        Token op = previous();
        AstNode* right = parseFactor();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseFactor() {
    /*
     *   factor ::= term (("+" | "-") term)*
    */
    AstNode* left = parseTerm();

    while (match({TokenType::Plus, TokenType::Minus})) {
        Token op = previous();
        AstNode* right = parseTerm();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseTerm() {
    /*
     *   term ::= unary (("*" | "/" | "%") unary)*
    */
    AstNode* left = parseUnary();

    while (match({TokenType::Star, TokenType::Slash, TokenType::Mod})) {
        Token op = previous();
        AstNode* right = parseUnary();
        left = new BinaryOpNode(left, op, right);
    }
    return left;
}

AstNode* Parser::parseUnary() {
    /*
     *   unary ::= (("+" | "-" | "~") unary) | primary
    */
    if (match({TokenType::Minus, TokenType::Tilde})) {
        Token op = previous();
        AstNode* right = parseUnary();
        return new UnaryOpNode(op, right);

    } else if (match(TokenType::Plus)) {
        return parseUnary();

    } else {
        return parsePrimary();
    }
}

AstNode* Parser::parseCall(AstNode* callee) {
    /*
     *  call ::= primary "(" (argument_list)? ")"
    */
    std::vector<AstNode*> args;

    if(peek().type != TokenType::RightParen) {
        do {
            args.push_back(parseExpr());
        } while(match(TokenType::Comma));
    }
    consume(TokenType::RightParen);

    return new CallNode(callee, args);
}

AstNode* Parser::parsePrimary() {
    /*
     *   primary ::= atom | attributeref
     *               | subscription | slicing | call
     *   attributeref ::= primary "." identifier
    */
    AstNode* left = parseAtom();

    while(true) {
        if(match(TokenType::LeftParen)) {
            left = parseCall(left);

        } else if(match(TokenType::Dot)) {
            Token name = consume(TokenType::Name);
            AstNode* right = new NameNode(name);
            left = new PropertyNode(left, right);

        } else {
            break;
        }
    }
    return left;
}

AstNode* Parser::parseAtom() {
    /*
     *  atom ::= identifier | literal | enclosure
     *  literal ::= stringliteral | integer | floatnumber
     *             | "None" | "True" | "False"
     *  enclosure ::= "(" expression ")"
    */
    if (match(TokenType::LeftParen)) {
        AstNode* expr = parseExpr();
        consume(TokenType::RightParen);
        return expr;
    } else {
        Token tk = advance();

        switch(tk.type) {
            case TokenType::Int:
                return new IntNode(tk);
            case TokenType::Float:
                return new FloatNode(tk);
            case TokenType::True:
                return new BooleanNode(true);
            case TokenType::False:
                return new BooleanNode(false);
            case TokenType::None:
                return new NullNode(tk);
            case TokenType::String:
                return new StringNode(tk);
            case TokenType::Name: {
                return new NameNode(tk);
            } default: {
                error("Expected a primary expression");
                return nullptr;
            }
        }
    }
}

Token Parser::consume(TokenType type) {
    if (match(type)) return previous();
    else {
        error("Expected " + std::to_string(type) + "\n");
        return Token(TokenType::Error, "", 0);
    }
}

bool Parser::match(TokenType type) {
    if (isAtEnd() || peek().type != type) {
        return false;
    } else {
        ++current;
        return true;
    }
}

bool Parser::match(std::initializer_list<TokenType> types) {

    if(isAtEnd()) return false;

    for(const auto& type : types) {
        if(peek().type == type) {
            ++current;
            return true;
        }
    }
    return false;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[(current > 0) ? current - 1 : 0];
}

Token Parser::advance() {
    return tokens[current++];
}

void Parser::error(const std::string& message) {
    std::cerr << "Error at line " << peek().line << ": " << message << std::endl;
    exit(EXIT_FAILURE);
}
