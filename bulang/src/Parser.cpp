#include "pch.h"

#include "Parser.hpp"
#include "Utils.hpp"
#include "Interpreter.hpp"
#include "Lexer.hpp"

Parser::Parser(Interpreter* interpreter)
{
    this->interpreter = interpreter;
    current = 0;
    panicMode = false;
    countBegins = 0;
    countEnds = 0;
    tokenError.lexeme = "ERROR";
    tokenError.type = TokenType::ERROR;
    tokenError.literal = "ERROR";
    tokenError.line = -1;
    NONE = Make_Shared<NilLiteral>();
}

Parser::~Parser()
{
}

bool Parser::Load(const String& source)
{
    Lexer lexer;
    lexer.Initialize(interpreter);
    if (lexer.Load(source))
    {

        tokens = lexer.GetTokens();
        // for (u32 i = 0; i < tokens.size(); i++)
        // {
        //     INFO("%s", tokens[i].toString().c_str());
        // }
        if (lexer.Ready())
        {
            return true;
        }
        return false;
    }

    return false;

}

bool Parser::Parse(ByteGenerator* visitor)
{
    if (tokens.size() == 0 || panicMode)
    {
        return false;
    }

    SharedPtr<Program> program = Make_Shared<Program>();
    while (!isAtEnd())
    {
        program->statements.push_back(declaration());
        if (panicMode)  
        {
            return false;
        }
    }


 

    program->accept(visitor);

    return !panicMode;
}

void Parser::Clear()
{

    tokens.clear();
    current = 0;
    panicMode = false;
    countBegins = 0;
    countEnds = 0;
}



bool Parser::match(TokenType type)
{
    if (isAtEnd())
        return false;
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

Token Parser::consume(TokenType type, const String& message)
{
    if (check(type))
    {
        return advance();
    }
    else
    {
        Error(tokens[current], message + " have '" + tokens[current].lexeme + "'");
        return peek();
    }
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;
    return tokens[current].type == type;
}

bool Parser::isAtEnd()
{
    return current >= tokens.size() || tokens[current].type == TokenType::END_OF_FILE;
}

void Parser::synchronize()
{

    // panicMode = false;
    // advance();
    while (!isAtEnd())
    {
        // INFO("synchronize %d  %d %d", tokens[current].type,current, tokens.size());
        // if (tokens[current].type == TokenType::SEMICOLON)
        // {
        //     return;
        // }
        // switch (tokens[current].type)
        // {
        //     case TokenType::CLASS:
        //     case TokenType::FUNCTION:
        //     case TokenType::VAR:
        //     case TokenType::FOR:
        //     case TokenType::IF:
        //     case TokenType::WHILE:
        //     case TokenType::PRINT:
        //     case TokenType::RETURN:
        //         return;

        //     default:
        //         ;
        // }

        advance();
    }
}

Token Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

Token Parser::peek()
{
    if (isAtEnd() || panicMode)
        return tokenError;
    return tokens[current];
}    

Token Parser::previous()
{
    if (current == 0 || tokens.empty() || panicMode)
    {
        return tokenError;
    }
    return tokens[static_cast<int>(current) - 1];
}

Token Parser::lookAhead()
{
    if (isAtEnd())
        return previous();
    if (current + 1 >= (int)tokens.size())
        return previous();
    return tokens[static_cast<int>(current) + 1];
}

void Parser::Error(const Token& token, const String& message)
{
    panicMode = true;
    int line = token.line;
    String text = message + " at line: " + String(line);
    interpreter->Error(text.c_str());
    synchronize();
}
void Parser::Error(const String& message)
{
    panicMode = true;
    interpreter->Error(message.c_str());
    synchronize();
}
void Parser::Warning(const Token& token, const String& message)
{
    int line = token.line;
    String text = message + " at line: " + String(line);
    interpreter->Warning(text);
}

void Parser::Warning(const String& message)
{
    interpreter->Warning(message);
}


//********************************************************************************** */
// COMPILER
//********************************************************************************** */

NodePtr Parser::number()
{
    SharedPtr<NumberLiteral> node = Make_Shared<NumberLiteral>();
    node->op = previous();
    String text = node->op.literal;
    node->value = text.ToDouble();
    return node;
}

NodePtr Parser::string()
{
    SharedPtr<StringLiteral> node = Make_Shared<StringLiteral>();
    node->op = previous();
    node->value= Chars(node->op.literal.c_str());
    return node;
}

NodePtr Parser::nil()
{
    return NONE;
}

NodePtr Parser::expression(bool canAssign)
{
    NodePtr expr  = assignment(canAssign);
    if (panicMode) return NONE;
    return expr;
}




NodePtr Parser::assignment(bool canAssign)
{
    NodePtr expr = expr_or(canAssign);
    if (panicMode) return NONE;
    Token token = previous();

    if (match(TokenType::EQUAL))
    {
        if (!canAssign)
        {
              Error(token,"Invalid 'assignment' target");
              return expr;
        }
 
        Token op = previous();
        NodePtr value = assignment(false);
       
        if (expr->type == NodeType::VARIABLE)
        {
            Variable* v = (Variable*)expr.get();
            SharedPtr<Assignment> assign = Make_Shared<Assignment>();
            assign->name = v->name;
            assign->value = std::move(expr);
            return assign;
        }
        else
        {
            Error(token,"Invalid assignment target: "+ expr->ToString());
            return expr;
        }
    }else  if (match(TokenType::PLUS_EQUAL) || match(TokenType::MINUS_EQUAL) ||
               match(TokenType::SLASH_EQUAL) || match(TokenType::STAR_EQUAL))
    {

        Token op = previous();
        NodePtr value = assignment(false);
        if (expr->type == NodeType::VARIABLE)
        {
            Variable* v = (Variable*)expr.get();
            SharedPtr<Assignment> assign = Make_Shared<Assignment>();
            assign->name = v->name;
            SharedPtr<Binary> addition = Make_Shared<Binary>();
            addition->operation = op;  
            addition->left  = std::move(expr);
            addition->right = std::move(value);
            assign->value = addition;
            return assign;
        }
        else
        {
            Error(token,"Invalid assignment target for '"+op.lexeme+"' operator .");
            return expr;
        }
    }

        


    return expr;
}



NodePtr Parser::expr_or(bool canAssign)
{
    NodePtr expr = expr_and(canAssign);
    if (panicMode) return NONE;
    while (match(TokenType::OR) )
    {
        Token op = previous();
        NodePtr right = expr_and(false);
        NodePtr left = std::move(expr);
        SharedPtr<Binary> binary = Make_Shared<Binary>();
        binary->operation = op;  
        binary->left  = std::move(left);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    return expr;
}


NodePtr Parser::expr_and(bool canAssign)
{
    
    NodePtr expr = expr_xor(canAssign);
    if (panicMode) return NONE;
    while (match(TokenType::AND))
    {
        Token op = previous();
        NodePtr right = expr_xor(false);
        NodePtr left = std::move(expr);
        SharedPtr<Binary> binary = Make_Shared<Binary>();
        binary->operation = op;  
        binary->left  = std::move(left);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    return expr;
}

NodePtr Parser::expr_xor(bool canAssign)
{

    NodePtr expr = equality(canAssign);
    if (panicMode) return NONE;
    while (match(TokenType::XOR))
    {
        Token op = previous();
        NodePtr right = equality(false);
         if (panicMode) return NONE;
        NodePtr left = std::move(expr);
        expr =  Make_Shared<Binary>();
        SharedPtr<Binary> binary = Make_Shared<Binary>();
        binary->operation = op;  
        binary->left  = std::move(left);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    return expr;

}


NodePtr Parser::equality(bool canAssign)
{
    NodePtr expr = comparison(canAssign);
    if (panicMode) return NONE;
    
    while (match(TokenType::BANG_EQUAL) || match(TokenType::EQUAL_EQUAL))
    {
        Token op = previous();
        NodePtr right = comparison(false);
        if (panicMode) return NONE;
        NodePtr left = std::move(expr);
        SharedPtr<Binary> binary = Make_Shared<Binary>();
        binary->operation = op;  
        binary->left  = std::move(left);
        binary->right = std::move(right);
        expr = std::move(binary);

    }
    return expr;
}

NodePtr Parser::comparison(bool canAssign)
{
    NodePtr expr = term(canAssign);
    if (panicMode) return NONE;
    while (match(TokenType::GREATER) || match(TokenType::LESS) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS_EQUAL))
    {
         Token op = previous();
         NodePtr right = term(false);
         NodePtr left =std::move(expr);
         expr =  Make_Shared<Binary>();
        SharedPtr<Binary> binary = Make_Shared<Binary>();
        binary->operation = op;  
        binary->left  = std::move(left);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    return expr;
}



NodePtr Parser::term(bool canAssign)
{
    NodePtr expr = factor(canAssign);
    if (panicMode) return NONE;
    while (match(TokenType::MINUS) || match(TokenType::PLUS) )
    {
        Token op = previous();
        
        NodePtr right = factor(canAssign);
        NodePtr left = std::move(expr);
        expr =  Make_Shared<Binary>();
        SharedPtr<Binary> binary = Make_Shared<Binary>();
        binary->operation = op;  
        binary->left  = std::move(left);
        binary->right = std::move(right);
        expr = std::move(binary);
 
    }

    return expr;
}

NodePtr Parser::factor(bool canAssign)
{
    NodePtr expr = unary(canAssign);
    if (panicMode) return NONE;
    while (match(TokenType::SLASH) || match(TokenType::STAR) || match(TokenType::MOD))
    {
        Token op = previous();
        NodePtr right = unary(canAssign);
        NodePtr left = std::move(expr);
        SharedPtr<Binary> binary = Make_Shared<Binary>();
        binary->operation = op;  
        binary->left  = std::move(left);
        binary->right = std::move(right);
        expr = std::move(binary);

    }
    return expr;
}

NodePtr Parser::unary(bool canAssign)
{
    if (match(TokenType::BANG) || match(TokenType::MINUS))
    {
        Token op = previous();
        NodePtr right = unary(canAssign);
        SharedPtr<Unary> left = Make_Shared<Unary>();
        left->operation = op;
        left->right =  std::move(right);
        return left;
    }

    //  if (match(TokenType::INC) || match(TokenType::DEC))
    // {
    //     Token op = previous();
    //     NodePtr right = unary(canAssign);

    //     if (panicMode) return NONE;

    //     if (right->type == NodeType::VARIABLE)
    //     {
    //         Variable* v = (Variable*)right.get();
    //         SharedPtr<Assignment> assign = Make_Shared<Assignment>();
    //         assign->name = v->name;
           

    //         SharedPtr<Unary> left = Make_Shared<Unary>();
    //         left->operation = op;
    //         left->right =  std::move(right);

    //         assign->value = std::move(left);
    //         return assign;

    //     } else 
    //     {
    //         Error(op,"Invalid assignment target: "+ right->ToString());
    //         return right;
    //     }
        
     


    //     return right;
    // }

    return call(canAssign);

}

NodePtr Parser::call(bool canAssign)
{

    NodePtr expr = primary(canAssign);
    // if (panicMode) return NONE;
    // while (true)
    // {
    //     if (match(TokenType::LEFT_PAREN))
    //     {
    //         expr = finish_call(expr);
    //     }
    //     // else if (match(TokenType::DOT))
    //     // {
    //     //     Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'");
    //     //     SharedPtr<Get> get = Make_Shared<Get>();
    //     //     get->name = name;
    //     //     get->object = std::move(expr);
    //     //     expr = std::move(get);
    //     // }
    //     else
    //     {
    //         break;
    //     }
    // }
    return expr;

}

NodePtr Parser::callStatement(bool native)
{
        Token name=previous();
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'function'");
        SharedPtr<Call> call = Make_Shared<Call>();

        Vector<NodePtr> args = argumentList(false);
        if (panicMode) return NONE;

        call->arguments = std::move(args);
        call->op = name;

        return call;

}
NodePtr Parser::primary(bool canAssign)
{

     if (match(TokenType::IDFUNCTION))
    {
        return callStatement(canAssign);
    }

    if (match(TokenType::FALSE))
    {
        SharedPtr<BooleanLiteral> node = Make_Shared<BooleanLiteral>();
        node->op = previous();
        node->value = false;
        return node;
    }

    if (match(TokenType::TRUE))
    {
        SharedPtr<BooleanLiteral> node = Make_Shared<BooleanLiteral>();
        node->op = previous();
        node->value = true;
        return node;
    }

    if (match(TokenType::NOW))
    {
        return now_statement();
    }

    if (match(TokenType::GOTO))
    {
        return goto_statement();
    }
    if (match(TokenType::GOSUB))
    {
        return gosub_statement();
    }

    if (match(TokenType::NIL))
    {
       return nil();
    }

    if (match(TokenType::NUMBER))
    {
        return number();
    }

    if (match(TokenType::STRING))
    {
        return string();
    }

    if (match(TokenType::LEFT_PAREN))
    {
        return grouping();
    }

    if (match(TokenType::IDENTIFIER))
    {
       NodePtr expr = variable(canAssign);    
     
       return expr;
    }



    Token current = peek();
    Error("Primary : Expect expression. "  + current.lexeme);
  
   return NONE;
}



NodePtr Parser::grouping()
{
    NodePtr expr = expression();
    if (panicMode) return NONE;
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return expr;
}




//************************************************************************* */
//STATEMENTs
//************************************************************************* */

NodePtr Parser::declaration()
{
    //INFO("declaration %s",peek().lexeme.c_str());


    if (match(TokenType::VAR))
    {
        return variable_declaration(true);
    }


    return statement();
}



NodePtr Parser::statement()
{
   // INFO("statement %s",peek().lexeme.c_str());

     if (match(TokenType::FUNCTION))
    {
        return function_declaration();
    }
  
    if (match(TokenType::IDLABEL))
    {
        return label_statement();
    }
   
        
    
    if (match(TokenType::IF))
    {
        return if_statement();
    }
    if (match(TokenType::SWITCH))
    {
        return switch_statement();
    }
    if (match(TokenType::RETURN))
    {
        return return_statement();
    }
    if (match(TokenType::BREAK))
    {
        return break_statement();
    }
    if (match(TokenType::CONTINUE))
    {
        return continue_statement();
    }
    if (match(TokenType::WHILE))
    {
        return while_statement();
    }
    if (match(TokenType::DO))
    {
        return do_statement();
    }
    if (match(TokenType::FOR))
    {
        return for_statement();
    }
    if (match(TokenType::FROM))
    {
      //  return from_statement();
    }
    
     if (match(TokenType::PRINT))
    {
        return print_statement();
    }

    if (match(TokenType::LEFT_BRACE))
    {
        return block();
    }
    return expression_statement();
}

NodePtr Parser::expression_statement(bool canAssign)
{
    NodePtr expr = expression(canAssign);
    if (panicMode) return NONE;

    if (match(TokenType::EQUAL))
    {
        if (!canAssign)
        {
            Error("Expression: Invalid assignment target");
            return NONE;
        }
    }

     consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return expr;
}



NodePtr Parser::block()
{
    SharedPtr<Block> program = Make_Shared<Block>();
    program->op = previous();
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        program->statements.push_back(declaration());
        if (panicMode) return NONE;
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
    return program;
}

NodePtr Parser::print_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'function'");
    SharedPtr<Print> print = Make_Shared<Print>();
    print->value = expression();
    if (panicMode)
    {
        synchronize();
        return NONE;
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    print->op = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after value");
    return print;
}

NodePtr Parser::now_statement()
{
    SharedPtr<Now> now = Make_Shared<Now>();
    now->op = previous();
    return now;
}

Vector<NodePtr> Parser::argumentList(bool canAssign)
{
    Vector<NodePtr> args;
    u8 count = 0;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            args.push_back(std::move(expression(false)));
            if (panicMode)
            {
                synchronize();
                return 0;
            }
            count++;
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    if (count >= 25)
    {
        Error("Can't have more than 25 arguments pass to a function");
    }
    return args;
}


NodePtr Parser::variable_declaration(bool canAssign)
{
   Token name = consume(TokenType::IDENTIFIER, "Expect variable name");
   SharedPtr<Declaration> node = Make_Shared<Declaration>();
   node->name = name;

   if (match(TokenType::EQUAL))
   {

      
        if (!canAssign)
        {
            Error("Declaration: Invalid assignment target");
        } else 
        {
            node->initializer = std::move(expression(canAssign));
            if (panicMode) return NONE;
        }
   } else 
   {
       WARNING("Declaration: Variable '%s' in not initializated !", name.lexeme.c_str());
       node->initializer = NONE;
   }
   consume(TokenType::SEMICOLON, "Expect ';' after variable declaration");

   return node;
}

NodePtr Parser::variable(bool canAssign)
{
    Token name = previous();
    if (canAssign && match(TokenType::EQUAL))
    {
        NodePtr value = expression(canAssign);
        if (panicMode) return NONE;
        SharedPtr<Assignment> node = Make_Shared<Assignment>();
        node->name = name;
        node->value = std::move(value);
        return node;
    }
     else  if (match(TokenType::INC))
    {

        Token op = previous();
         SharedPtr<Variable> var = Make_Shared<Variable>();
         var->name = name;

        SharedPtr<NumberLiteral> value = Make_Shared<NumberLiteral>();
        value->op = name;
        value->value = 1;

        
        INFO("INC (%s) %s",name.lexeme.c_str(),op.lexeme.c_str());
     //   return expr;
        // if (expr->type == NodeType::VARIABLE)
        // {
        //     Variable* v = (Variable*)expr.get();
            SharedPtr<Assignment> assign = Make_Shared<Assignment>();
            assign->name = name;
            SharedPtr<Binary> addition = Make_Shared<Binary>();
            addition->operation = op;  
            addition->left  = std::move(var);
            addition->right = std::move(value);
            assign->value = addition;
            return assign;
        // }
        // else
        // {
        //     Error(token,"Invalid assignment target for '++' operator .");
        //     return expr;
        // }

    }
    // else 
    // {
    //     SharedPtr<Variable> v = Make_Shared<Variable>();
    //     v->name = name;
        
    //     if (match(TokenType::INC) || match(TokenType::DEC))
    //     { 
             
    //         Token op = previous();

  
        
    //         SharedPtr<Unary> unaryOp = Make_Shared<Unary>();
    //         unaryOp->operation = op;
    //         unaryOp->right = v;

    //         SharedPtr<Assignment> assign = Make_Shared<Assignment>();
    //         assign->name = name;
    //         assign->value = std::move(unaryOp);
            

    //     // Retorna o nó que primeiro lê a variável, faz a operação, e depois faz o assign
    //         SharedPtr<Operator> action = Make_Shared<Operator>();
    //         action->operation = OperationType::POSINCREMENT;
    //         action->op = op;
    //         action->variable  = v;
    //         action->value     = assign;
    //         return action;
    //         // action->value     = v;
    //     }

         


    //     return v;
    // }
    return NONE;
}

NodePtr Parser::if_statement()
{

     SharedPtr<IFStatement> node = Make_Shared<IFStatement>();
     node->op = previous();
     consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
     node->condition = expression(false);
     if (panicMode) return NONE;
     consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

     node->thenBranch = statement();
     if (panicMode) return NONE;


    while (match(TokenType::ELIF))
    {
        UniquePtr<ElifStmt> elif = Make_Unique<ElifStmt>();
        elif->op = previous();
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'elif'.");
        elif->condition = expression(false);
        if (panicMode) return NONE;
        consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
        elif->body = statement();
        if (panicMode) return NONE;
        node->elifBranch.push_back(std::move(elif));
    }

     if (match(TokenType::ELSE))
     {
         node->elseBranch = statement();
         if (panicMode) return NONE;
     } else 
     {
         node->elseBranch = nullptr;
     }
     
     
    
     return node;
}

NodePtr Parser::switch_statement()
{

    SharedPtr<SwitchStatement> node = Make_Shared<SwitchStatement>();
    node->op = previous();
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'switch'.");
    node->condition = expression(); // Evaluate the switch value expression
    if (panicMode) return NONE;
    consume(TokenType::RIGHT_PAREN, "Expect ')' after switch condition.");
    if (panicMode) return NONE;
    consume(TokenType::LEFT_BRACE, "Expect '{' before switch cases.");

    int caseCount = 0;
    int asDefault = 0;
     int state = 0;


  while (!match(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) 
  {
    if (match(TokenType::CASE) || match(TokenType::DEFAULT)) 
    {
            Token caseType = previous();
            UniquePtr<CaseStatement> caseStatement = Make_Unique<CaseStatement>();
            caseStatement->op = caseType;

            if (state == 2) 
            {
                Error("Can't have another case or default after the default case.");
                return NONE;
            }



            if (caseType.type == TokenType::CASE) 
            {
                state = 1;
                caseStatement->condition = expression(false);
                consume(TokenType::COLON, "Expect ':' after case value.");
                if (panicMode) return NONE;
                caseCount++;
            } else 
            {
                state = 2;
                consume(TokenType::COLON, "Expect ':' after default.");
            
                if (panicMode) return NONE;    
                asDefault = 1;
            }
            if (panicMode) return NONE;
            
            while (!check(TokenType::CASE) && !check(TokenType::DEFAULT) && !check(TokenType::RIGHT_BRACE)) 
            {
                caseStatement->statements.push_back(statement());
                if (panicMode) return NONE;
            }

            //caseStatement->body = statement();
            if (panicMode) return NONE;
            node->cases.push_back(std::move(caseStatement));
    }
    else 
    {
            if (state == 0) 
            {
                Error("Can't have statements before any case.");
                if (panicMode) return NONE;
            }
            if (panicMode) return NONE;
    }
  }


    if (caseCount == 0 && asDefault == 0)
    {
        Error("Switch statement must have at least one case or a default case.");
        return NONE;
    }
    

    return node;
}

NodePtr Parser::label_statement()
{

    SharedPtr<LabelStatement> node = Make_Shared<LabelStatement>();
    node->name = previous();
    consume(TokenType::COLON, "Expect ':' after label.");

    if (panicMode) return NONE;
    return node;
}

NodePtr Parser::goto_statement()
{

    SharedPtr<GoToStatement> node = Make_Shared<GoToStatement>();
    node->name = previous();
    consume(TokenType::IDENTIFIER, "Expect label name after 'goto'.");
    node->label = previous();
    return node;
}

NodePtr Parser::gosub_statement()
{

    SharedPtr<GoSubStatement> node = Make_Shared<GoSubStatement>();
    node->name = previous();
    consume(TokenType::IDENTIFIER, "Expect label name after 'gosub'.");
    node->label = previous();
    return node;    
}

NodePtr Parser::while_statement()
{

    SharedPtr<WhileStatement> node = Make_Shared<WhileStatement>();
    node->op = previous();
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    if (panicMode) return NONE;
    node->condition = expression(false);
    if (panicMode) return NONE;
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    node->body = statement();
    if (panicMode) return NONE;
    return node;
}

NodePtr Parser::do_statement()
{

    SharedPtr<DoStatement> node = Make_Shared<DoStatement>();
    node->op = previous();
    
    node->body = statement();
    if (panicMode) return NONE;


    consume(TokenType::WHILE, "expected  'while' before ");
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    node->condition = expression(false);
    if (panicMode) return NONE;
    consume(TokenType::RIGHT_PAREN, "Expect ')' after  condition.");
    if (panicMode) return NONE;
    consume(TokenType::SEMICOLON, "Expect ';' after do-while condition.");
    return node;
}

NodePtr Parser::for_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
    if (panicMode) return NONE;
    SharedPtr<ForStatement> node = Make_Shared<ForStatement>();
    node->op = previous();
    if (match(TokenType::VAR))
    {
        node->initializer = variable_declaration(true);
        if (panicMode) return NONE;
    } else if (match(TokenType::SEMICOLON))
    {
       
        node->initializer = nullptr;
    } else 
    {
        node->initializer = expression_statement(true);
        if (panicMode) return NONE;
    }

    if (!match(TokenType::SEMICOLON))
    {
        node->condition = expression(false);
        if (panicMode) return NONE;
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    } else 
    {
        node->condition = nullptr;
    }

    if (!match(TokenType::RIGHT_PAREN))
    {
        node->increment = expression(true);
        if (panicMode) return NONE;
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    } else 
    {
        node->increment = nullptr;
    }

    node->body = statement();
    if (panicMode) return NONE;
    return node;
}

NodePtr Parser::return_statement()
{

    SharedPtr<ReturnStatement> node = Make_Shared<ReturnStatement>();
    node->op = previous();

    if (!check(TokenType::SEMICOLON))
    {
        node->value = expression(false);
        if (panicMode) return NONE;
    } else 
    {
        node->value = NONE;
        node->op = previous();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    callReturn = true;

    return node;

}

NodePtr Parser::break_statement()
{
    consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
    if (panicMode) return NONE;
    SharedPtr<BreakStatement> node = Make_Shared<BreakStatement>();
    node->op = previous();


    return node;
}

NodePtr Parser::continue_statement()
{
   consume(TokenType::SEMICOLON, "Expect ';' after 'continue'.");
     if (panicMode) return NONE;
   SharedPtr<ContinueStatement> node = Make_Shared<ContinueStatement>();
   node->op = previous();
   return node;
}

NodePtr Parser::function_declaration()
{
    Token name = consume(TokenType::IDFUNCTION, "Expect function name.");

    if (name.lexeme == "__main__")
    {
        interpreter->Error("Cannot use '__main__' as a function name");
        return NONE;
    }

    if (panicMode) return NONE;
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
    if (panicMode) return NONE;

    SharedPtr<FunctionStatement> node = Make_Shared<FunctionStatement>();
    node->name = name;


    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
           Token name =  consume(TokenType::IDENTIFIER, "Expect parameter name.");
           node->names.push_back(name.lexeme);
        } while (match(TokenType::COMMA));
    }
    
    callReturn = false;

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    if (panicMode) return NONE;
    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    if (panicMode) return NONE;
    node->body = block();
    if (panicMode) return NONE;



 //   Token prev = previous();
   // if (!callReturn)
  //      interpreter->Warning("Function '%s' without return value!", name.lexeme.c_str());
    node->asreturn = callReturn;
    callReturn = false;
    return node;
}
