#include "pch.h"
#include "Vm.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"


void Parser::setTask(Task *currentTask)
{
    
    this->currentTask = currentTask;
}

Parser::Parser()
{
    current = 0;
    panicMode = false;
    countBegins = 0;
    countEnds = 0;
    hasReturned = false;
    vm=nullptr;
    lexer.initialize();
    
}
void Parser::Init(VirtualMachine *vm)
{
    this->vm = vm;
    this->currentTask = vm->getCurrentTask();
}


bool Parser::Load(String text)
{
    current = 0;
    panicMode = false;
    countBegins = 0;
    countEnds = 0;
    if( lexer.Load(std::move(text)))
    {
        tokens = std::move(lexer.process());
        Print();
        return true;
    }
    return false;
}

void Parser::Clear()
{
    lexer.clear();
    functions.clear();
    process.clear();
    current = 0;
    panicMode = false;
    countBegins = 0;
    countEnds = 0;
}

void Parser::Print()
{
        int line = -1;
        for (size_t i = 0; i < tokens.size(); i++)
        {
            Token token = tokens[i];
            if (token.line != line)
            {
                line = token.line;
                printf("%4d ", line);
            }
            else
            {
                printf("   | ");
            }
            if (token.type == TokenType::IDFUNCTION)
            {
                printf("DEF '%s' \n", token.lexeme.c_str());
            }
            else if (token.type == TokenType::IDNATIVE)
            {
                printf("NATIVE '%s' \n",  token.lexeme.c_str());
        } else if (token.type == TokenType::IDPROCESS)
        {
            printf("PROCESS '%s' \n",  token.lexeme.c_str());
        } else 
        {
            printf("%2d '%.*s' \n", (int)token.type, (int)token.lexeme.size(), token.lexeme.c_str());
        }
    }   
}

void Parser::addNative(const char *name)
{
    lexer.addNative(name);
}

bool Parser::match(TokenType type)
{
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

bool Parser::match(Vector<TokenType> types)
{
    for (auto type : types)
    {
        if (match(type))
        {
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const String &message)
{
    if (check(type))
    {
        return advance();
    }
    else
    {
        Error(tokens[current], message + " have '" + tokens[current].lexeme + "'");
        return Token::errorToken();
    }
}

bool Parser::check(TokenType type)
{
    if (abort())
        return false;
    if (isAtEnd())
        return false;
    return tokens[current].type == type;
}

bool Parser::isAtEnd()
{
    if (abort())     return true;
    if (current >= (int)tokens.size())
        return true;
    if (panicMode)
        return true;
   // return tokens[current].type == TokenType::END_OF_FILE;

    return false;
}

void Parser::synchronize()
{
    advance();
    while (!isAtEnd())
    {
        advance();
    }
}

bool Parser::abort()
{
    if ((current < 0) || (current >= (int)tokens.size()) || panicMode)
        return true;

    return false;
}

Token Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

Token Parser::peek()
{
    if (current >= (int)tokens.size())
    {
        Error("current exceed tokens size");
        return Token::errorToken();
    }
    return tokens[current];
}

Token Parser::previous()
{
    if (current == 0)
    {
        return Token::errorToken();
    }
    return tokens[current - 1];
}

Token Parser::lookAhead()
{
    if (current + 1 >= (int)tokens.size())
        return previous();
    return tokens[current + 1];
}

void Parser::Error(const Token &token, const String &message)
{
    emitByte(OpCode::HALT);
    panicMode = true;
    int line = token.line;
    String textLine = String(line);
    String text = "Parsing: " + message + " at line: " + textLine;
    Log(2, text.c_str());
    synchronize();

    //  Clear();
}

void Parser::Error(const String &message)
{
    emitByte(OpCode::HALT);
    panicMode = true;
    String text = "Parsing: " + message;
    Log(2, text.c_str());
    synchronize();

    // Clear();
}

void Parser::Warning(const String &message)
{
    String text = "Parsing: " + message;
    Log(1, text.c_str());
}

void Parser::Warning(const Token &token, const String &message)
{
    int line = token.line;
    String textLine = String(line);
    String text ="Parsing: " + message + " at line: " + textLine;
    Log(1, text.c_str());
}
//************************************************************************************************************* */

u8 Parser::makeConstant(const Value &value)
{

    int constant = (int)currentTask->addConst(value);
    if (constant > 255)
    {
        vm->Error("Too many constants in one task");
        panicMode = true;
        return 0;
    }

    return (u8)constant;
}

void Parser::emitByte(u8 byte)
{
    int line = previous().line;
    currentTask->write_byte(byte, line);
}

void Parser::emitBytes(u8 byte1, u8 byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

void Parser::emitReturn()
{
    emitByte(OpCode::RETURN);
}

void Parser::emitConstant(const Value &value)
{
    emitBytes(OpCode::CONST, makeConstant(std::move(value)));
}

void Parser::emitLoop(int loopStart)
{
    emitByte(OpCode::JUMP_BACK);

    int offset = currentTask->code_size() - loopStart + 2;
    if (offset > UINT16_MAX)
    {
        vm->Error("[EMIT LOOP] Loop offset overflow");
        panicMode = true;
        return;
    }
    emitByte((offset >> 8) & 0xff);
    emitByte(offset & 0xff);
}

int Parser::emitJump(u8 instruction)
{
    emitByte(instruction);
    emitByte(0xff);
    emitByte(0xff);
    return currentTask->code_size() - 2;
}

int Parser::jumpSize(u8 instruction)
{
    return currentTask->code_size() - 2;
}

void Parser::patchJump(int offset)
{
    currentTask->patch(offset);
}

//************************************************************************************************************* */


void Parser::number()
{
    String text = previous().literal;
    double value = text.ToDouble();
    emitConstant(NUMBER(value));
}

void Parser::string()
{
    String text = previous().literal;
    emitConstant(STRING(text));
}



bool Parser::Process()
{
    if (!currentTask)
    {
        vm->Error("No current task");
        return false;
    }
    program();
    if (panicMode)
        return false;


    emitReturn();
    return true;
}

void Parser::program()
{
    consume(TokenType::PROGRAM,"Expect 'program' at the beginning.");
    Token name = consume(TokenType::IDENTIFIER,"Expect program name after 'program'.");
    String nameStr = name.literal;
    consume(TokenType::SEMICOLON,"Expect ';' after program name.");

    while (!isAtEnd())
    {
        declaration();
    }    
    
}

void Parser::expression(bool canAssign)
{
    expr_or(canAssign);
    if (canAssign && match(TokenType::EQUAL))
    {
        Error("Invalid assignment target.");
    }
}

void Parser::expr_or(bool canAssign)
{

    expr_and(canAssign);
    while (match(TokenType::OR))
    {

        // short circuit
        int elseJump = emitJump(OpCode::JUMP_IF_FALSE);
        int endJump = emitJump(OpCode::JUMP);

        patchJump(elseJump);

        expr_and(canAssign);
        patchJump(endJump);
    }
}

void Parser::expr_and(bool canAssign)
{

    expr_xor(canAssign);
    while (match(TokenType::AND))
    {

        int endJump = emitJump(OpCode::JUMP_IF_FALSE);

        expr_xor(canAssign);

        patchJump(endJump);
    }
}

void Parser::expr_xor(bool canAssign)
{

    equality(canAssign);
    while (match(TokenType::XOR))
    {
        Token op = previous();
        equality(false);
        emitByte(OpCode::XOR);
    }
}

void Parser::equality(bool canAssign)
{

    comparison(canAssign);
    while (match(TokenType::BANG_EQUAL) || match(TokenType::EQUAL_EQUAL))
    {

        Token op = previous();
        comparison(false);
        if (op.type == TokenType::BANG_EQUAL)
            emitByte(OpCode::NOT_EQUAL);
        else if (op.type == TokenType::EQUAL_EQUAL)
            emitByte(OpCode::EQUAL);
    }
}

void Parser::comparison(bool canAssign)
{

    term(canAssign);
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL))
    {

        Token op = previous();
        term(false);
        if (op.type == TokenType::GREATER)
            emitByte(OpCode::GREATER);
        else if (op.type == TokenType::GREATER_EQUAL)
            emitByte(OpCode::GREATER_EQUAL);
        else if (op.type == TokenType::LESS)
            emitByte(OpCode::LESS);
        else if (op.type == TokenType::LESS_EQUAL)
            emitByte(OpCode::LESS_EQUAL);
    }
}

void Parser::term(bool canAssign)
{

    factor(canAssign);
    while (match(TokenType::PLUS) || match(TokenType::MINUS))
    {

        Token op = previous();
        factor(false);
        if (op.type == TokenType::PLUS)
            emitByte(OpCode::ADD);
        else if (op.type == TokenType::MINUS)
            emitByte(OpCode::SUBTRACT);
    }
}

void Parser::factor(bool canAssign)
{

    power(canAssign);
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MOD))
    {

        Token op = previous();
        power(false);
        if (op.type == TokenType::STAR)
            emitByte(OpCode::MULTIPLY);
        else if (op.type == TokenType::SLASH)
            emitByte(OpCode::DIVIDE);
        else if (op.type == TokenType::MOD)
            emitByte(OpCode::MOD);
    }
}

void Parser::power(bool canAssign)
{

    unary(canAssign);
    while (match(TokenType::POWER))
    {

        Token op = previous();
        unary(false);
        if (op.type == TokenType::POWER)
            emitByte(OpCode::POWER);
    }
}
void Parser::unary(bool canAssign)
{
    if (match(TokenType::MINUS) || match(TokenType::BANG) || match(TokenType::NOT))
    {

        Token op = previous();
        unary(false);
        if (op.type == TokenType::MINUS)
        {
            emitByte(OpCode::NEGATE);
        }
        else if (op.type == TokenType::BANG || op.type == TokenType::NOT)
        {
            emitByte(OpCode::NOT);
        }
    }
    else
    {

        call(canAssign);
    }
}
void Parser::call(bool canAssign)
{
    while (true)
    {
        if (match(TokenType::IDPROCESS))
        {
            callProcess();
            return;
        }
        else if (match(TokenType::IDFUNCTION))
        {
            
            callStatement(false);
            return;
        }
        
        else if (match(TokenType::IDNATIVE))
        {
            callStatement(true);
            return;
         } 

        else
        {
            break;
        }
    }

    primary(canAssign);
}
void Parser::primary(bool canAssign)
{

    if (match(TokenType::NOW))
    {
        emitByte(OpCode::NOW);
    }
    else if (match(TokenType::NUMBER))
    {
        number();
    }
    else if (match(TokenType::STRING))
    {
        string();
    }
    else if (match(TokenType::TRUE))
    {
        emitConstant(BOOLEAN(true));
    }
    else if (match(TokenType::FALSE))
    {
        emitConstant(BOOLEAN(false));
    }
    else if (match(TokenType::NIL))
    {
        emitConstant(NONE());
    }
    else if (match(TokenType::IDENTIFIER))
    {
        variable(canAssign);
    }
    else if (match(TokenType::LEFT_PAREN))
    {
        grouping();
    }
    else
    {
        advance(); 
    }
}

void Parser::grouping()
{
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
}



// --------------------------------- DECLATIONS ----------------------------------------------------
void Parser::declaration()
{

     if (match(TokenType::FUNCTION))
    {

        functionDeclaration();
    } else if (match(TokenType::PROCESS))
    {
        processDeclaration();
    }  else if (match(TokenType::VAR))
    {
        variableDeclaration();
    } 
    else
    {
        statement();
    }

    if (panicMode)
        synchronize();
}



void Parser::functionDeclaration()
{


    Token name = consume(TokenType::IDFUNCTION, "Expect function name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

    

    const char *rawName = name.lexeme.c_str();
    if (name.lexeme == "__main__")
    {
        vm->Error("Cannot use '__main__' as a function name");
        return;
    }

   

    

    Task *defaultTask = currentTask;
    Task *task = vm->newTask(rawName);
    setTask(task);
    emitByte(OpCode::ENTER_SCOPE);
    hasReturned = false;


    if (!match(TokenType::RIGHT_PAREN))
    {
        do
        {
            Token name = consume(TokenType::IDENTIFIER, "Expect parameter name");
            if (!task->addArgs(name.lexeme))
            {
                vm->Error("Function '%s' already has argument '%s'", rawName, name.lexeme.c_str());
                return;
            }
            
        } while (match(TokenType::COMMA));
    }

    if (task->argsCount != 0)
        consume(TokenType::RIGHT_PAREN, "Expect ')' after function name.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");


    block();

  //  consume(TokenType::RIGHT_BRACE, "Expect '}' after function body.");

    Token prev = previous();
    if (previous().type == TokenType::RIGHT_BRACE && !hasReturned)
    {

        emitByte(OpCode::NIL);
        emitByte(OpCode::RETURN);
        vm->Warning("Function '%s' without return value!", name.lexeme.c_str());
    }

    
    emitByte(OpCode::EXIT_SCOPE);
    setTask(defaultTask);
    vm->setMainTask();

    hasReturned = false;

   // INFO("Function: %s", name.lexeme.c_str());
}

void Parser::processDeclaration()
{
    Token name = consume(TokenType::IDPROCESS, "Expect process name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after process name.");

    

    const char *rawName = name.lexeme.c_str();
    if (name.lexeme == "__main__")
    {
        vm->Error("Cannot use '__main__' as a process name");
        return;
    }



   // INFO("Declare process %s", rawName);

    Task *defaultTask = currentTask;
    Task *task = vm->newTask(rawName);
    setTask(task);
    emitByte(OpCode::ENTER_SCOPE);
    hasReturned = false;

    if (!match(TokenType::RIGHT_PAREN))
    {
        do
        {
            Token name = consume(TokenType::IDENTIFIER, "Expect parameter name");
            if (!task->addArgs(name.lexeme))
            {
                vm->Error("Process '%s' already has argument '%s'", rawName, name.lexeme.c_str());
                return;
            }
            
        } while (match(TokenType::COMMA));
    }

    if (task->argsCount != 0)
        consume(TokenType::RIGHT_PAREN, "Expect ')' after process name.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before process body.");


    block();


    
    emitByte(OpCode::EXIT_SCOPE);
    emitByte(OpCode::RETURN_PROCESS);
    setTask(defaultTask);
    vm->setMainTask();

   //     INFO("Process: %s", name.lexeme.c_str());
}

void Parser::statement()
{
    if (match(TokenType::IF))
    {
        ifStatement();
    }
    else if (match(TokenType::SWITCH))
    {
        switchStatement();
    }
    else if (match(TokenType::WHILE))
    {
        whileStatement();
    } else if (match(TokenType::LOOP))
    {
        loopStatement();
    }
    else if (match(TokenType::DO))
    {
        doWhileStatement();
    }
    else if (match(TokenType::FOR))
    {
        forStatement();
    }
    else if (match(TokenType::BREAK))
    {
        breakStatement();
    }
    else if (match(TokenType::CONTINUE))
    {
        continueStatement();
    }
    else if (match(TokenType::RETURN))
    {
        returnStatement();
    }
    else if (match(TokenType::PRINT))
    {
        printStatement();
    }
    else if (match(TokenType::LEFT_BRACE))
    {
     //   emitByte(OpCode::ENTER_SCOPE);
        block();
      //  emitByte(OpCode::EXIT_SCOPE);
    }
    else
    {
        expressionStatement();
    }
    if (panicMode)
        synchronize();
}

void Parser::block()
{

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        declaration();
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
}

void Parser::expressionStatement()
{
   
    expression();
   

    if (match(TokenType::EQUAL))
    {
        Error("Invalid assignment target");
    }

    if (check(TokenType::LEFT_PAREN))
    {
        Error("Function "+previous().lexeme+" is not declared");
        return;
    }

    consume(TokenType::SEMICOLON, "Expect ';' after expression");
    emitByte(OpCode::POP);
}

void Parser::printStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'print'");
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after value");
    consume(TokenType::SEMICOLON, "Expect ';' after value");
    emitByte(OpCode::PRINT);
}

void Parser::variableDeclaration()
{

    // Token next = lookAhead();
    // if (next.type == TokenType::COMMA)
    // {

    //     Vector<u8> variableIndices;
    //     do
    //     {

    //         Token name = consume(TokenType::IDENTIFIER, "Expect variable name");
    //         Value value = STRING(name.lexeme);
    //         u8 index = makeConstant(std::move(value));
    //         variableIndices.push_back(index);
    //     } while (match(TokenType::COMMA));

    //     if (match(TokenType::EQUAL))
    //     {
    //         expression();
    //     }
    //     else
    //     {
    //         emitConstant(NONE());
    //     }

    //     emitConstant(NUMBER(static_cast<double>(variableIndices.size())));
    //     emitConstant(NUMBER(static_cast<double>(variableIndices[0])));
    //     emitBytes(OpCode::GLOBAL_MULTIPLE_DEFINE, variableIndices.size());


    // } else 
    // {

    //         Token name = consume(TokenType::IDENTIFIER, "Expect variable name");
    //         Value value = STRING(name.lexeme);
    //         u8 index = makeConstant(std::move(value));
    //         if (match(TokenType::EQUAL))
    //         {

    //             expression();
    //         }
    //         else
    //         {
    //             emitConstant(NONE());
    //         }

    //         emitBytes(OpCode::GLOBAL_DEFINE, index);
        
            

    // }

    

    
    

    Token name = consume(TokenType::IDENTIFIER, "Expect variable name");
    Value value = STRING(name.lexeme);
    u8 index = makeConstant(std::move(value));
    if (match(TokenType::EQUAL))
    {

        expression();
    }
    else
    {
        emitConstant(NONE());
    }

    emitBytes(OpCode::VARIAVEL_DEFINE, index);
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration");
}

void Parser::variable(bool canAssign)
{
    Token name = previous();
    Value value = STRING(name.lexeme);
    u8 arg = makeConstant(std::move(value));

    if (canAssign && match(TokenType::EQUAL))
    {

        expression();
        emitBytes(OpCode::VARIAVEL_ASSIGN, arg);
    }
    else if (!canAssign && match(TokenType::EQUAL))
    {

        Error("Invalid assignment target");
    }
    else
    {
       emitBytes(OpCode::VARIAVEL_GET, arg);
        
    }
}

void Parser::ifStatement()
{

    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'");
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition");

    u8 thenJump = emitJump(OpCode::JUMP_IF_FALSE);
    statement();

    u8 elseJump = emitJump(OpCode::JUMP);
    patchJump(thenJump);

    while (match(TokenType::ELIF))
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'elif'.");
        expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after elif condition.");

        thenJump = emitJump(OpCode::JUMP_IF_FALSE);

        statement(); // Executa o bloco 'elif'

        patchJump(elseJump);
        elseJump = emitJump(OpCode::JUMP);
        patchJump(thenJump);
    }

    if (match(TokenType::ELSE))
    {
        statement();
    }
    patchJump(elseJump);
}

void Parser::switchStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'switch'.");
    expression(); // Evaluate the switch value expression
    consume(TokenType::RIGHT_PAREN, "Expect ')' after switch condition.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before switch cases.");

    Vector<u8> endJumps;
    endJumps.reserve(32);

    int caseCount = 0;
    int defaultJump = -1;

    while (match(TokenType::CASE))
    {
        emitByte(OpCode::DUP); // Duplica o valor da expressão do switch
        expression();          // Expressão do case
        consume(TokenType::COLON, "Expect ':' after case value.");

        emitByte(OpCode::EQUAL); // Compara a expressão do switch com a expressão do case
        u8 caseJump = emitJump(OpCode::JUMP_IF_FALSE);

        statement(); // Executa o bloco do case

        u8 jump = emitJump(OpCode::JUMP); // Pula para o fim do switch
        endJumps.push_back(jump);
        caseCount++;

        patchJump(caseJump);
    }

    if (match(TokenType::DEFAULT))
    {
        consume(TokenType::COLON, "Expect ':' after default case.");
        statement();                          // Executa o bloco default
        defaultJump = emitJump(OpCode::JUMP); // Salto para o bloco default
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after switch cases.");

    if (caseCount == 0 && defaultJump == -1)
    {
        Error("Switch statement must have at least one case or a default case.");
        return;
    }

    for (int i = 0; i < caseCount; i++)
    {
        patchJump(endJumps[i]);
    }

    if (defaultJump != -1)
    {
        patchJump(defaultJump);
    }
    emitByte(OpCode::POP); // Remove o valor da expressão do switch duplicado
}

void Parser::whileStatement()
{

    int loopStartBackup = currentTask->loopStart;
    int exitJumpBackup = currentTask->exitJump;

    currentTask->loopStart = currentTask->code_size();

    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

    currentTask->exitJump = emitJump(OpCode::JUMP_IF_FALSE);

    statement();

    emitLoop(currentTask->loopStart);
    patchJump(currentTask->exitJump);

    currentTask->loopStart = loopStartBackup;
    currentTask->exitJump = exitJumpBackup;
}
void Parser::doWhileStatement()
{

    int breakJumpCount = currentTask->breakJumps.size();

    int loopStartBackup = currentTask->loopStart;
    int exitJumpBackup = currentTask->exitJump;

    currentTask->loopStart = currentTask->code_size();

    currentTask->exitJump = -2;

    statement();

    consume(TokenType::WHILE, "Expect 'while' after loop body in do-while statement.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");

    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    consume(TokenType::SEMICOLON, "Expect ';' after do-while condition.");

    currentTask->exitJump = emitJump(OpCode::JUMP_IF_FALSE);
    emitLoop(currentTask->loopStart);

    for (int i = breakJumpCount; i < (int)currentTask->breakJumps.size(); i++)
    {
        patchJump(currentTask->breakJumps[i]);
    }

    patchJump(currentTask->exitJump);

    currentTask->breakJumps.resize(breakJumpCount);

    currentTask->loopStart = loopStartBackup;
    currentTask->exitJump = exitJumpBackup;
}


void Parser::loopStatement()
{
    
    int loopStartBackup = currentTask->loopStart;
    int exitJumpBackup = currentTask->exitJump;

    currentTask->loopStart = currentTask->code_size();

    emitConstant(BOOLEAN(true));
    
    currentTask->exitJump = emitJump(OpCode::JUMP_IF_FALSE);
    statement();

  //  emitByte(OpCode::POP);

    emitLoop(currentTask->loopStart);
    patchJump(currentTask->exitJump);

    currentTask->loopStart = loopStartBackup;
    currentTask->exitJump = exitJumpBackup;


  

  
    
    

}

void Parser::forStatement()
{

    int loopStartBackup = currentTask->loopStart;
    int exitJumpBackup = currentTask->exitJump;
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    emitByte(OpCode::ENTER_SCOPE);

    if (match(TokenType::SEMICOLON))
    {
        // No initializer
    }
    else if (match(TokenType::VAR))
    {
        variableDeclaration();
    }
    else
    {
        expressionStatement();
    }

    currentTask->loopStart = currentTask->code_size();
    currentTask->exitJump = -1;

    if (!match(TokenType::SEMICOLON)) // exit condition
    {
        expression();
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        currentTask->exitJump = emitJump(OpCode::JUMP_IF_FALSE);
    }

    if (!match(TokenType::RIGHT_PAREN))
    {
        int bodyJump = emitJump(OpCode::JUMP);
        int incrementStart = currentTask->code_size();
        expression();
        emitByte(OpCode::POP);
        consume(TokenType::RIGHT_PAREN, "Expect ')' after loop body.");

        emitLoop(currentTask->loopStart);

        currentTask->loopStart = incrementStart;

        patchJump(bodyJump);
    }


    
    statement();
    emitLoop(currentTask->loopStart);

    
    if (currentTask->exitJump != -1)
    {
        patchJump(currentTask->exitJump);
    }

    currentTask->loopStart = loopStartBackup;
    currentTask->exitJump = exitJumpBackup;

   

    emitByte(OpCode::EXIT_SCOPE);
}



void Parser::breakStatement()
{

    consume(TokenType::SEMICOLON, "Expect ';' after 'break'");

    if (currentTask->exitJump == -2) // do while
    {
        u8 breakAddress = emitJump(OpCode::JUMP);
        currentTask->breakJumps.push_back(breakAddress);
        return;
    }

    if (currentTask->exitJump == -1)
    {
        vm->Warning("break statement outside of loop");
        return;
    }
    int breakAddress = emitJump(OpCode::JUMP);
    currentTask->addToInPlaceJumpOffsetList(currentTask->exitJump, breakAddress);
}

void Parser::continueStatement()
{
    consume(TokenType::SEMICOLON, "Expect ';' after 'continue'");

    if (currentTask->loopStart == -1)
    {
        vm->Warning("continue statement outside of loop");
        return;
    }
    emitLoop(currentTask->loopStart);
}

void Parser::returnStatement()
{
    hasReturned = true;
    if (match(TokenType::SEMICOLON))
    {
        emitByte(OpCode::NIL);
        emitByte(OpCode::RETURN);
    }
    else
    {
        expression();
        consume(TokenType::SEMICOLON, "Expect ';' after return value.");
        emitByte(OpCode::RETURN);
    }
}

void Parser::frameStatement()
{
    consume(TokenType::SEMICOLON, "Expect ';' after 'frame'");
    emitByte(OpCode::FRAME);
}

void Parser::callStatement(bool native)
{
    Token name = previous();
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

    // push function name

    emitConstant(STRING(name.lexeme));
    u8 argCount = argumentList(false);

    if (native)
    {
        emitBytes(OpCode::CALL, argCount); // push args count and call
      //emitByte(OpCode::RETURN_NATIVE);
        
    
    }
    else
        emitBytes(OpCode::CALL_SCRIPT, argCount);
}

void Parser::callProcess()
{
    Token name = previous();
    consume(TokenType::LEFT_PAREN, "Expect '(' after process name.");
    emitConstant(STRING(name.lexeme));
    u8 argCount = argumentList(true);
    emitBytes(OpCode::CALL_PROCESS, argCount);
}
u8 Parser::argumentList(bool canAssign)
{
    u8 count = 0;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            expression(canAssign);
            count++;
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    if (count >= 255)
    {
        vm->Error("Can't have more than 255 arguments pass to a task");
        panicMode = true;
        return 0;
    }
    return count;
}
