#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <string>
#include "interpreter.hpp"
#include "token.hpp"
#include "tokentype.hpp"
#include "runtime_exceptions.hpp" 

PyObject* Interpreter::visitProgramNode(ProgramNode* node)
{
    #ifdef DEBUG_INTERPRETER
      std::cout << "visiting " << "ProgramNode" << "\n";
    #endif


    return node->body->accept(this);

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "ProgramNode" << "\n";
    #endif

}

std::string formatNumber(llf number) {

    std::ostringstream oss;
    int intPart = static_cast<int>(number);
    double fracPart = number - intPart;

    if (fracPart == 0.0) {
        oss << intPart << ".0";
    } else {
        oss << number;
    }
    return oss.str();
}

PyObject* Interpreter::visitPrintNode(PrintNode* node) {

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "PrintNode" << "\n";
    #endif


    PyObject* argValue = nullptr;

    if (node->args != nullptr) {
        argValue = node->args[0].accept(this);
    }
    if (argValue != nullptr) {
        if((*argValue).isStr()) {
            const PyStr* obj = dynamic_cast<const PyStr*>(argValue);
            std::cout << "############Output############\n";
            std::cout << (*obj).getStr();
            std::cout << "\n############Output############";
        } else if((*argValue).isFloat()) {
            const PyFloat* obj = dynamic_cast<const PyFloat*>(argValue);
            std::cout << "############Output############\n";
            std::cout << formatNumber((*obj).getFloat());
            std::cout << "\n############Output############";
        } else {

            if((*argValue).isInstance())
            {
                argValue = static_cast<PyInstance*>(argValue)->get_class();
            }
            std::cout << "############Output############\n";
            std::cout << *argValue;
            std::cout << "\n############Output############";
        }
    }
    std::cout << "\n" << std::flush;

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "PrintNode" << "\n";
    #endif

    return new PyNone();
}

PyObject* Interpreter::visitIntNode(IntNode* node){

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "IntNode" << "\n";
    #endif


    const std::string& str = node->getLexeme();
    PyObject* value = new PyInt(str);
    GC.pushObject(value);

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "IntNode" << "\n";
    #endif


    return value;


}

PyObject* Interpreter::visitFloatNode(FloatNode* node){

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "FloatNode" << "\n";
    #endif

    const std::string& str = node->getLexeme();
    PyObject* value = new PyFloat(str);
    GC.pushObject(value);

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "FloatNode" << "\n";
    #endif

    return value;
}

PyObject* Interpreter::visitFunctionNode(FunctionNode* node){

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "FunctionNode" << "\n";
    #endif


    const std::string& fname = node->getName();
    PyFunction* value = new PyFunction(node);
    currentEnv.top()->define(fname, value);

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "FunctionNode" << "\n";
    #endif


    return new PyNone();
}

PyObject* Interpreter::visitClassNode(ClassNode* node) {

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "ClassNode" << "\n";
    #endif


    Scope* parentEnv = currentEnv.top();
    Scope* objEnv = new Scope(parentEnv , ScopeType::CLASS);

    currentEnv.push(objEnv);
    node->getBody()->accept(this);
    currentEnv.pop();

    const std::string& kname = node->getName();

    PyClass* value = new PyClass(kname, objEnv);

    currentEnv.top()->define(kname, value);

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "ClassNode" << "\n";
    #endif


    return new PyNone();
}

PyObject* Interpreter::visitPropertyNode(PropertyNode* node)
{
    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "PropertyNode" << "\n";
    #endif


    PyObject* object = node->object->accept(this);

    if(object->isInstance()) {
        PyInstance* obj = static_cast<PyInstance*>(object);

        // 跟踪执行流程中的实例
        currentInstance.push(obj);

        Scope* scope = obj->getScope();

      //  scope->show_scope_content();

        currentEnv.push(scope);
        PyObject* attr = node->attribute->accept(this);
        currentEnv.pop();

        if(!currentInstance.empty())
          currentInstance.pop();


        return attr;
    }

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "PropertyNode" << "\n";
    #endif

    return new PyNone();
}

PyObject* Interpreter::visitBlockNode(BlockNode* node) {

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "BlockNode" << "\n";
    #endif


    for(auto statement : node->statements) {
    	statement->accept(this);
    }

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "BlockNode" << "\n";
    #endif


    return new PyNone();
}

PyObject* Interpreter::visitWhileNode(WhileNode* node)
{

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "WhileNode" << "\n";
    #endif

    PyObject* cond = node->cond->accept(this);

    while(cond->isTruthy()){
     	try {
     	    node->body->accept(this);
     	} catch(BreakException be) {
     	    break;
     	} catch(ContinueException ce) {
     	    ; 
     	}
        cond = node->cond->accept(this);
    }

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "WhileNode" << "\n";
    #endif

    return new PyNone();
}

PyObject* Interpreter::visitBreakNode(BreakNode* node) {

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "BreakNode" << "\n";
    #endif


    throw BreakException();

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "BreakNode" << "\n";
    #endif

    return nullptr;
}

PyObject* Interpreter::visitContinueNode(ContinueNode* node) {

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "ContinueNode" << "\n";
    #endif

    throw ContinueException();

    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "ContinueNode" << "\n";
    #endif

    return nullptr;
}

PyObject* Interpreter::visitPassNode(PassNode* node) {

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "PassNode" << "\n";
    #endif


    #ifdef DEBUG_INTERPRETER
        std::cout << "exiting " << "PassNode" << "\n";
    #endif

    return new PyNone();

}

PyObject* Interpreter::visitIfNode(IfNode* node) {

    #ifdef DEBUG_INTERPRETER
        std::cout << "visiting " << "IfNode" << "\n";
    #endif

    PyObject* cond = node->cond->accept(this);

    if (cond->isTruthy()) {
        return node->trueBranch->accept(this);
    } else {
        for (const auto& elif : node->elifBranches) {
            PyObject* elifCond = elif.first->accept(this);
            if (elifCond->isTruthy()) {
                return elif.second->accept(this);
            }
        }
        if (node->elseBranch != nullptr) {
            return node->elseBranch->accept(this);
        }
    }


    #ifdef DEBUG_INTERPRETER
            std::cout << "exiting " << "IfNode" << "\n";
    #endif

    return new PyNone();
}

PyObject* Interpreter::visitTernaryOpNode(TernaryOpNode* node)
{


    #ifdef DEBUG_INTERPRETER
          std::cout << "visiting " << "TernaryOpNode" << "\n";
    #endif

    PyObject* cond = node->cond->accept(this);

    PyObject * result  = nullptr;

    if(cond->isTruthy()) {

        result =  node->left->accept(this);
    } else {
        result =  node->right->accept(this);
    }


    #ifdef DEBUG_INTERPRETER
            std::cout << "exiting " << "TernaryOpNode" << "\n";
    #endif

    return result;

}

PyObject* Interpreter::visitBinaryOpNode(BinaryOpNode* node)  {

    #ifdef DEBUG_INTERPRETER
          std::cout << "visiting " << "BinaryOpNode" << "\n";
    #endif

    PyObject* leftValue = node->left->accept(this);
    leftValue->incRc();

    PyObject* rightValue = node->right->accept(this);
    rightValue->incRc();

    PyObject* value = nullptr;

    switch(node->op.type) {
        case TokenType::Plus: 
            value = *leftValue + *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Minus: 
            value = *leftValue - *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Star: 
            value = *leftValue * *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Slash: 
            value = *leftValue / *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Ampersand: 
            value = *leftValue & *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Pipe: 
            value = *leftValue | *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Caret:
            value = *leftValue ^ *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Mod: 
            value = *leftValue % *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::EqualEqual: 
            value = *leftValue == *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::BangEqual: 
            value = !(*(*leftValue == *rightValue));
            GC.pushObject(value);
            break;
        case TokenType::Less: 
            value = *leftValue < *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Greater:
            value = *leftValue > *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::LessEqual:
            value = !(*(*leftValue > *rightValue));
            GC.pushObject(value);
            break;
        case TokenType::GreaterEqual: 
            value = !(*(*leftValue < *rightValue));
            GC.pushObject(value);
            break;
        case TokenType::LeftShift: 
            value = *leftValue << *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::RightShift: 
            value = *leftValue >> *rightValue;
            GC.pushObject(value);
            break;
        case TokenType::Or:
            value = leftValue->isTruthy() ? leftValue : rightValue;
            break;
        case TokenType::And:
            value = !(leftValue->isTruthy()) ? leftValue : rightValue;
            break;
        default:
            throw std::logic_error("Unsupported binary operator");
    }
    leftValue->decRc();
    rightValue->decRc();


    #ifdef DEBUG_INTERPRETER
          std::cout << "exiting " << "BinaryOpNode" << "\n";
    #endif

    return value;
}

PyObject* Interpreter::visitAssignNode(AssignNode* node) {

    #ifdef DEBUG_INTERPRETER
          std::cout << "visiting " << "AssignNode" << "\n";
    #endif

    AstNode* targetNode = node->name;
    Scope* topEnv;

    std::string varName;

    if(targetNode->is_name_node()) {
        NameNode* name = static_cast<NameNode*>(targetNode);
        varName = name->getLexeme();
        topEnv = currentEnv.top();

    } else if(targetNode->is_property_node()) {
        PropertyNode* ppr = static_cast<PropertyNode*>(targetNode);
        PyInstance* object = static_cast<PyInstance*>(ppr->object->accept(this));
        NameNode* attribute = static_cast<NameNode*>(ppr->attribute);
        std::cout << attribute->getLexeme() << std::endl;
        varName = attribute->getLexeme();
        topEnv = object->getScope();

    } else {
        throw std::runtime_error("Unsupported target expression");
    }

    PyObject* value = node->value->accept(this);
    value->incRc();

    if(node->op.type == TokenType::Equals) {
        topEnv->define(varName, value);
    } else {
        PyObject* targetValue = topEnv->get(varName);
        targetValue->incRc();

        switch(node->op.type) {
            case TokenType::PlusEqual:
                value = *targetValue + *value;
                break;
            case TokenType::MinusEqual:
                value = *targetValue - *value;
                break;
            case TokenType::StarEqual:
                value = *targetValue * *value;
                break;
            case TokenType::SlashEqual:
                value = *targetValue / *value;
                break;
            case TokenType::ModEqual:
                value = *targetValue % *value;
                break;
            case TokenType::AndEqual:
                value = *targetValue & *value;
                break;
            case TokenType::OrEqual:
                value = *targetValue | *value;
                break;
            case TokenType::XorEqual:
                value = *targetValue ^ *value;
                break;
            case TokenType::LeftShiftEqual:
                value = *targetValue << *value;
                break;
            case TokenType::RightShiftEqual:
                value = *targetValue >> *value;
                break;
            default:
                throw std::runtime_error("Unsupported assignment operator");
        }
        topEnv->define(varName, value);
        targetValue->decRc();
    }
    value->decRc();

    #ifdef DEBUG_INTERPRETER
          std::cout << "exiting " << "AssignNode" << "\n";
    #endif

    return value;
}

PyObject* Interpreter::visitNameNode(NameNode* node){

    #ifdef DEBUG_INTERPRETER
          std::cout << "visiting " << "NameNode" << "\n";
    #endif

    const std::string& varname = node->getLexeme();
    Scope* topEnv = currentEnv.top();

    // if this is a class atribute
    #ifdef DEBUG_INTERPRETER
          std::cout << "exiting " << "NameNode" << "\n";
    #endif

    return topEnv->get(varname);
}

PyObject* Interpreter::visitBooleanNode(BooleanNode* node){


    #ifdef DEBUG_INTERPRETER
            std::cout << "visiting " << "BooleanNode" << "\n";
    #endif

    PyObject* value = new PyBool(node->value);
    GC.pushObject(value);

    #ifdef DEBUG_INTERPRETER
              std::cout << "exiting " << "BooleanNode" << "\n";
    #endif

    return value;
}

PyObject* Interpreter::visitStringNode(StringNode* node)
{
    #ifdef DEBUG_INTERPRETER
            std::cout << "visiting " << "StringNode" << "\n";
    #endif

    const std::string& str = node->getLexeme();
    PyObject* value = new PyStr(str);
    GC.pushObject(value);

    #ifdef DEBUG_INTERPRETER
            std::cout << "exiting " << "StringNode" << "\n";
    #endif

    return value;
}

PyObject* Interpreter::visitUnaryOpNode(UnaryOpNode* node)
{
    #ifdef DEBUG_INTERPRETER
            std::cout << "visiting " << "UnaryOpNode" << "\n";
    #endif

    PyObject* operandValue = node->right->accept(this);
    PyObject* result = nullptr;

    switch(node->op.type) {
        case TokenType::Minus:
            result = -(*operandValue);
            break;
        case TokenType::Not:
            result = !(*operandValue);
            break;
        case TokenType::Tilde:
            result = ~(*operandValue);
            break;
        default:
            throw std::logic_error("Unsupported unary operator");
    }
    GC.pushObject(result);

    #ifdef DEBUG_INTERPRETER
            std::cout << "exiting " << "StringNode" << "\n";
    #endif
    return result;
}

PyObject* Interpreter::visitNullNode(NullNode* expr)
{
    #ifdef DEBUG_INTERPRETER
            std::cout << "visiting " << "NullNode" << "\n";
    #endif

    PyObject* value = new PyNone();
    GC.pushObject(value);

    #ifdef DEBUG_INTERPRETER
            std::cout << "exiting " << "NullNode" << "\n";
    #endif

    return value;
}

PyObject* Interpreter::visitCallNode(CallNode* expr)
{
    #ifdef DEBUG_INTERPRETER
            std::cout << "visiting " << "CallNode" << "\n";
    #endif

    AstNode* caller = expr->caller;
    PyObject* calleeRef = caller->accept(this);

    if(calleeRef->isKlass()) {

        PyClass* callee = static_cast<PyClass*>(calleeRef);

        Scope* parentEnv = callee->getScope(); // 获取类的范围
        Scope* objEnv = new Scope(parentEnv , ScopeType::CLASS);

        PyInstance* self = new PyInstance(objEnv , callee);

        PyFunction* constructor = static_cast<PyFunction*>(objEnv->get("__init__"));

        const std::vector<AstNode*>& params = constructor->getParams();
        const std::vector<AstNode*>& args = expr->args;

        for (size_t i = 0; i < params.size(); i++){
            NameNode* paramNode = static_cast<NameNode*>(params[i]);
            const std::string& argName = paramNode->getLexeme();
            if(i == 0) {
                objEnv->define(argName, self);
            } else {
                objEnv->define(argName, args[i-1]->accept(this));
            }
        }
        currentEnv.push(objEnv);
        constructor->getBody()->accept(this);
        currentEnv.pop();


        #ifdef DEBUG_INTERPRETER
                std::cout << "exiting " << "CallNode" << "\n";
        #endif

        return self;

    } else if(calleeRef->isFunc()) {

        std::cout << "call " << std::endl;

        if(dynamic_cast<PyFunctionBuiltIn*>(calleeRef))
        {
            std::vector<PyObject *> builtin_args;

            const std::vector<AstNode*>& args = expr->args;

            for(AstNode*  arg : args)
            {
                 builtin_args.push_back(arg->accept(this));
            }

            BuiltInfunctionType builtin_func = static_cast<PyFunctionBuiltIn*>(calleeRef)->get_builtin();

            #ifdef DEBUG_INTERPRETER
                    std::cout << "exiting " << "CallNode" << "\n";
            #endif

            return  builtin_func(builtin_args);
        }

        Scope* parentEnv = currentEnv.top();

      //  parentEnv->show_scope_content();

        PyInstance* bound = nullptr;

        Scope* objEnv = nullptr;

        if(caller->is_property_node())
        {

           bound = (caller->is_property_node() ? new PyInstance(objEnv , nullptr) : nullptr);

           if(!currentInstance.empty()){
              objEnv = currentInstance.top()->getScope();
              currentEnv.push(objEnv);
            }

        }else
        {
            objEnv = new Scope(parentEnv , ScopeType::FUNC);
            currentEnv.push(objEnv);
        }

        //currentEnv.top()->show_scope_content();

        PyFunction* callee = static_cast<PyFunction*>(calleeRef);

        const std::vector<AstNode*>& params = callee->getParams();
        const std::vector<AstNode*>& args = expr->args;

        if((args.size() + (bound ? 1 : 0)) != params.size()) {
            throw std::runtime_error("error on positional args");
        }

        if(bound) {
            for (size_t i = 0; i < params.size(); i++){
                NameNode* paramNode = static_cast<NameNode*>(params[i]);
                const std::string& argName = paramNode->getLexeme();

                if(i == 0) {
                    objEnv->define(argName, bound);
                } else {
                    objEnv->define(argName, args[i-1]->accept(this));
                }
            }

        } else {

            for (size_t i = 0; i < args.size(); i++){
                NameNode* paramNode = static_cast<NameNode*>(params[i]);
                const std::string& argName = paramNode->getLexeme();
                  std::cout << "calling "  << argName<< std::endl;
                objEnv->define(argName, args[i]->accept(this));
            }
        }

        PyObject* retValue = nullptr;
        try {
            callee->getBody()->accept(this);
        } catch(ReturnException re) {
            retValue = re.value;
        }
        currentEnv.pop();


        #ifdef DEBUG_INTERPRETER
                std::cout << "exiting " << "CallNode" << "\n";
        #endif

        return retValue;

    } else {
        throw std::logic_error("Not a callable.");
    }

    #ifdef DEBUG_INTERPRETER
            std::cout << "exiting " << "CallNode" << "\n";
    #endif

    return new PyNone();
}

PyObject* Interpreter::visitReturnNode(ReturnNode* node) {

    #ifdef DEBUG_INTERPRETER
            std::cout << "visiting " << "ReturnNode" << "\n";
    #endif

    AstNode* value = node->value;
    PyObject* retValue = new PyNone();
    if(value) retValue = value->accept(this);
    throw ReturnException(retValue);

    #ifdef DEBUG_INTERPRETER
            std::cout << "exiting " << "ReturnNode" << "\n";
    #endif

    return nullptr; 
}

PyObject* Interpreter::interpret(ProgramNode* node) {
    return node->accept(this);
}
