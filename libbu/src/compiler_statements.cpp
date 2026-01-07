#include "compiler.hpp"
#include "interpreter.hpp"
#include "value.hpp"
#include "opcode.hpp"
#include "pool.hpp"
#include "debug.hpp"

// ============================================
// STATEMENTS
// ============================================

void Compiler::declaration()
{

    if (match(TOKEN_DEF))
    {
        funDeclaration();
    }
    else if (match(TOKEN_PROCESS))
    {
        processDeclaration();
    }
    else if (match(TOKEN_VAR))
    {
        varDeclaration();
    }
    else if (match(TOKEN_IMPORT))
    {
        parseImport();
    }
    else if (match(TOKEN_INCLUDE))
    {
        includeStatement();
    }
    else if (match(TOKEN_USING))
    {
        parseUsing();
    }
    else
    {
        statement();
    }

    if (panicMode)
    {
        synchronize();
    }
}

void Compiler::statement()
{
    if (check(TOKEN_IDENTIFIER) && peek(0).type == TOKEN_COLON)
    {
        labelStatement();
    }
    else if (match(TOKEN_FRAME))
    {
        frameStatement();
    }
    else if (match(TOKEN_YIELD))
    {
        yieldStatement();
    }
    else if (match(TOKEN_FIBER))
    {
        fiberStatement();
    }
    else if (match(TOKEN_EXIT))
    {
        exitStatement();
    }
    else if (match(TOKEN_PRINT))
    {
        printStatement();
    }
    else if (match(TOKEN_IF))
    {
        ifStatement();
    }
    else if (match(TOKEN_WHILE))
    {
        whileStatement();
    }
    else if (match(TOKEN_GOTO))
    {
        gotoStatement();
    }
    else if (match(TOKEN_GOSUB))
    {
        gosubStatement();
    }
    else if (match(TOKEN_DO))
    {
        doWhileStatement();
    }
    else if (match(TOKEN_LOOP))
    {
        loopStatement();
    }
    else if (match(TOKEN_FOR))
    {
        forStatement();
    }
    else if (match(TOKEN_FOREACH))
    {
        foreachStatement();
    }
    else if (match(TOKEN_BREAK))
    {
        breakStatement();
    }
    else if (match(TOKEN_SWITCH))
    {
        switchStatement();
    }
    else if (match(TOKEN_CONTINUE))
    {
        continueStatement();
    }
    else if (match(TOKEN_STRUCT))
    {
        structDeclaration();
    }
    else if (match(TOKEN_CLASS))
    {
        classDeclaration();
    }
    else if (match(TOKEN_TRY))
    {
        tryStatement();
    }
    else if (match(TOKEN_THROW))
    {
        throwStatement();
    }
    else if (match(TOKEN_LBRACE))
    {
        beginScope();
        block();
        endScope();
    }
    else if (match(TOKEN_RETURN))
    {
        returnStatement();
    }
    else
    {
        expressionStatement();
    }
}

void Compiler::printStatement()
{
    uint8_t argCount = 0;
    
    // Se não tem parênteses, aceita sintaxe antiga: print x;
    if (!check(TOKEN_LPAREN))
    {
        expression();
        argCount = 1;
    }
    else
    {
        consume(TOKEN_LPAREN, "Expect '('");
      
        if (!check(TOKEN_RPAREN))
        {
            do
            {
                expression();
                argCount++;
                
                if (argCount > 255)
                {
                    error("Cannot have more than 255 arguments");
                }
            } while (match(TOKEN_COMMA));
        }
        
        consume(TOKEN_RPAREN, "Expect ')' after arguments");
    }
    
    consume(TOKEN_SEMICOLON, "Expect ';'");
    
    emitBytes(OP_PRINT, argCount);  
}


// void Compiler::printStatement()
// {
//     expression();
//     consume(TOKEN_SEMICOLON, "Expect ';' after value");
//     emitByte(OP_PRINT);
// }
 
void Compiler::expressionStatement()
{

    // Expressão normal
    expression();
    consume(TOKEN_SEMICOLON, "Expresion statemnt Expect ';'");
    emitByte(OP_POP);
}

// ============================================
// VARIABLES
// ============================================

void Compiler::varDeclaration()
{
    do
    {
        consume(TOKEN_IDENTIFIER, "Expect variable name");
        Token nameToken = previous;

        uint8_t global = identifierConstant(nameToken);

        if (scopeDepth > 0)
        {
            declareVariable();

            if (currentClass != nullptr && loopDepth_ > 1 && scopeDepth > 1)
            {
                Warning("Variable '%s' is declared inside loops in class methods.", nameToken.lexeme.c_str());
            }
        }

        //  Se tem '=' e NÃO tem vírgula depois
        if (match(TOKEN_EQUAL) && !check(TOKEN_COMMA))
        {
            expression();
        }
        else
        {
            // Ignora '=' se tem vírgula, ou não tem '='
            if (match(TOKEN_EQUAL))
            {
                expression(); // Compila mas descarta
                emitByte(OP_POP);
            }
            emitByte(OP_NIL);
        }

        defineVariable(global);

    } while (match(TOKEN_COMMA)); //  Continua se tem vírgula

    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration");
}

void Compiler::variable(bool canAssign)
{
    Token name = previous;
    std::string nameStr = name.lexeme;

    // =====================================================
    // PASSO 1: Procura em módulos USING (flat access)
    // =====================================================
    bool foundInUsing = false;

    for (const auto &modName : usingModules)
    {

        uint16 moduleId;
        if (!vm_->getModuleId(modName.c_str(), &moduleId))
        {
            continue; // Módulo não existe (não deve acontecer)
        }

        ModuleDef *mod = vm_->getModule(moduleId);
        if (!mod)
        {
            continue;
        }

        // Tenta como função
        uint16 funcId;
        if (mod->getFunctionId(nameStr.c_str(), &funcId))
        {
            // É função! Deve ser chamada
            if (!match(TOKEN_LPAREN))
            {
                error("Module functions must be called");
                return;
            }

            // Emite ModuleRef
            Value ref = vm_->makeModuleRef(moduleId, funcId);
            emitConstant(ref);

            // Compila argumentos e CALL
            call(false);

            foundInUsing = true;
            return; //  Encontrou!
        }

        // Tenta como constante
        uint16 constId;
        if (mod->getConstantId(nameStr.c_str(), &constId))
        {
            // É constante! Emite valor direto
            Value *value = mod->getConstant(constId);
            if (value)
            {
                emitConstant(*value);
                foundInUsing = true;
                return; //  Encontrou!
            }
        }
    }

    // =====================================================
    // PASSO 2: Verifica se é module.member (DOT access)
    // =====================================================
    if (check(TOKEN_DOT))
    {
        // Verifica se é módulo importado
        if (importedModules.find(nameStr) != importedModules.end())
        {
            // É módulo! Processa module.member
            advance(); // Consome DOT
            consume(TOKEN_IDENTIFIER, "Expect member name");
            Token member = previous;

            uint16 moduleId;
            if (!vm_->getModuleId(nameStr.c_str(), &moduleId))
            {
                fail("Module '%s' not found", nameStr.c_str());
                return;
            }

            ModuleDef *mod = vm_->getModule(moduleId);
            if (!mod)
            {
                fail("Module '%s' not found", nameStr.c_str());
                return;
            }
            // Tenta como função
            uint16 funcId;
            if (mod->getFunctionId(member.lexeme.c_str(), &funcId))
            {
                // É função! Deve ser chamada
                if (!match(TOKEN_LPAREN))
                {
                    error("Module functions must be called");
                    return;
                }

                // Emite ModuleRef
                Value ref = vm_->makeModuleRef(moduleId, funcId);
                emitConstant(ref);

                // Compila argumentos e CALL
                call(false);
                return; //  Sucesso!
            }

            // Tenta como constante
            uint16 constId;
            if (mod->getConstantId(member.lexeme.c_str(), &constId))
            {
                // É constante! Emite valor direto
                Value *value = mod->getConstant(constId);
                if (value)
                {
                    emitConstant(*value);
                    return; //  Sucesso!
                }
            }

            // Não encontrou
            fail("'%s' not found in module '%s'",
                 member.lexeme.c_str(),
                 nameStr.c_str());
            return;
        }
    }

    // =====================================================
    // PASSO 3: Variável normal (local ou global)
    // =====================================================
    namedVariable(name, canAssign);
}

void Compiler::and_(bool canAssign)
{
    (void)canAssign;
    int endJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    parsePrecedence(PREC_AND);
    patchJump(endJump);
}

void Compiler::or_(bool canAssign)
{
    (void)canAssign;
    int elseJump = emitJump(OP_JUMP_IF_FALSE);
    int endJump = emitJump(OP_JUMP);

    patchJump(elseJump);
    emitByte(OP_POP);

    parsePrecedence(PREC_OR);

    patchJump(endJump);
}

uint8 Compiler::identifierConstant(Token &name)
{

    return makeConstant(vm_->makeString(name.lexeme.c_str()));
}

void Compiler::handle_assignment(uint8 getOp, uint8 setOp, int arg, bool canAssign)
{

    if (match(TOKEN_PLUS_PLUS))
    {
        // i++ (postfix)
        emitBytes(getOp, (uint8)arg);
        emitBytes(getOp, (uint8)arg);
        emitConstant(vm_->makeInt(1));
        emitByte(OP_ADD);
        emitBytes(setOp, (uint8)arg);
        emitByte(OP_POP);
    }
    else if (match(TOKEN_MINUS_MINUS))
    {
        // i-- (postfix)
        emitBytes(getOp, (uint8)arg);
        emitBytes(getOp, (uint8)arg);
        emitConstant(vm_->makeInt(1));
        emitByte(OP_SUBTRACT);
        emitBytes(setOp, (uint8)arg);
        emitByte(OP_POP);
    }
    else if (canAssign && match(TOKEN_EQUAL))
    {
        expression();
        emitBytes(setOp, (uint8)arg);
    }
    else if (canAssign && match(TOKEN_PLUS_EQUAL))
    {
        emitBytes(getOp, (uint8)arg);
        expression();
        emitByte(OP_ADD);
        emitBytes(setOp, (uint8)arg);
    }
    else if (canAssign && match(TOKEN_MINUS_EQUAL))
    {
        emitBytes(getOp, (uint8)arg);
        expression();
        emitByte(OP_SUBTRACT);
        emitBytes(setOp, (uint8)arg);
    }
    else if (canAssign && match(TOKEN_STAR_EQUAL))
    {
        emitBytes(getOp, (uint8)arg);
        expression();
        emitByte(OP_MULTIPLY);
        emitBytes(setOp, (uint8)arg);
    }
    else if (canAssign && match(TOKEN_SLASH_EQUAL))
    {
        emitBytes(getOp, (uint8)arg);
        expression();
        emitByte(OP_DIVIDE);
        emitBytes(setOp, (uint8)arg);
    }
    else if (canAssign && match(TOKEN_PERCENT_EQUAL))
    {
        emitBytes(getOp, (uint8)arg);
        expression();
        emitByte(OP_MODULO);
        emitBytes(setOp, (uint8)arg);
    }
    else
    {
        emitBytes(getOp, (uint8)arg);
    }
}

void Compiler::namedVariable(Token &name, bool canAssign)
{
    uint8 getOp, setOp;
    int arg;

    // === 1. SE estamos em PROCESS e é private conhecido ===
    if (isProcess_)
    {
        arg = (int)vm_->getProcessPrivateIndex(name.lexeme.c_str());
        if (arg != -1)
        {
            getOp = OP_GET_PRIVATE;
            setOp = OP_SET_PRIVATE;
            handle_assignment(getOp, setOp, arg, canAssign);
            return;
        }
    }

    // === 2. Tenta LOCAL ===
    arg = resolveLocal(name);
    if (arg != -1)
    {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
        handle_assignment(getOp, setOp, arg, canAssign);
        return;
    }

    // === 3. É GLOBAL ===
    arg = identifierConstant(name);
    getOp = OP_GET_GLOBAL;
    setOp = OP_SET_GLOBAL;

    handle_assignment(getOp, setOp, arg, canAssign);
}

void Compiler::defineVariable(uint8 global)
{
    if (scopeDepth > 0)
    {

        markInitialized();
        return;
    }

    emitBytes(OP_DEFINE_GLOBAL, global);
}

void Compiler::declareVariable()
{
    if (scopeDepth == 0)
        return;

    Token &name = previous;

    for (int i = localCount_ - 1; i >= 0; i--)
    {
        Local &local = locals_[i];

        if (local.depth != -1 && local.depth < scopeDepth)
        {
            break;
        }

        if (local.equals(name.lexeme))
        {
            error("Variable with this name already declared in this scope");
        }
    }

    addLocal(name);
}

void Compiler::addLocal(Token &name)
{
    if (localCount_ >= MAX_LOCALS)
    {
        error("Too many local variables in function");
        return;
    }

    size_t len = name.lexeme.length();

    if (len >= MAX_IDENTIFIER_LENGTH)
    {
        error("Identifier name too long (max 31 characters)");
        return;
    }

    // Copia string
    std::memcpy(locals_[localCount_].name, name.lexeme.c_str(), len);
    locals_[localCount_].name[len] = '\0';
    locals_[localCount_].length = (uint8)len;
    locals_[localCount_].depth = -1;

    localCount_++;
}

void Compiler::markInitialized()
{
    if (scopeDepth == 0)
        return;

    if (localCount_ == 0)
    {
        error("Internal error: marking uninitialized with no locals");
        return;
    }
    locals_[localCount_ - 1].depth = scopeDepth;
}

void Compiler::beginScope()
{
    scopeDepth++;
}

void Compiler::endScope()
{

    int popped = discardLocals(scopeDepth);
    localCount_ -= popped;
    scopeDepth--;
}

int Compiler::resolveLocal(Token &name)
{
    for (int i = localCount_ - 1; i >= 0; i--)
    {
        Local &local = locals_[i];

        if (local.equals(name.lexeme))
        {
            if (local.depth == -1)
            {
                error("Can't read local variable in its own initializer");
            }
            return i;
        }
    }

    return -1;
}

void Compiler::block()
{
    while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF))
    {
        declaration();
    }
    consume(TOKEN_RBRACE, "Expect '}' after block");
}

void Compiler::ifStatement()
{
    // if (condition)
    consume(TOKEN_LPAREN, "Expect '(' after 'if'");
    expression();
    consume(TOKEN_RPAREN, "Expect ')' after condition");

    // Jump para próximo bloco se condição for falsa
    int thenJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP); // Pop da condição se for true

    // Then branch
    statement();

    // Lista de jumps para o final (depois de cada then/elif executar)
    std::vector<int> endJumps;
    endJumps.push_back(emitJump(OP_JUMP)); // Jump do if

    // Patch do thenJump (aponta para o próximo elif/else/end)
    patchJump(thenJump);
    emitByte(OP_POP); // Pop da condição se for false

    // Elif branches (pode ter vários)
    while (match(TOKEN_ELIF))
    {
        // elif (condition)
        consume(TOKEN_LPAREN, "Expect '(' after 'elif'");
        expression();
        consume(TOKEN_RPAREN, "Expect ')' after elif condition");

        // Jump para próximo bloco se condição for falsa
        int elifJump = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP); // Pop se elif for true

        // Elif body
        statement();

        // Jump para o final após executar elif
        endJumps.push_back(emitJump(OP_JUMP));

        // Patch do elifJump (aponta para próximo elif/else/end)
        patchJump(elifJump);
        emitByte(OP_POP); // Pop se elif for false
    }

    // Else branch (opcional)
    if (match(TOKEN_ELSE))
    {
        statement();
    }

    // Patch todos os jumps para apontarem para o final
    for (int jump : endJumps)
    {
        patchJump(jump);
    }
}

void Compiler::beginLoop(int loopStart, bool isForeach)
{
    if (loopDepth_ >= MAX_LOOP_DEPTH)
    {
        error("Too many nested loops");
        return;
    }

    loopContexts_[loopDepth_].loopStart = loopStart;
    loopContexts_[loopDepth_].scopeDepth = scopeDepth;
    loopContexts_[loopDepth_].breakCount = 0;
    loopContexts_[loopDepth_].isForeach = isForeach;
    loopDepth_++;
}

void Compiler::endLoop()
{
    if (loopDepth_ == 0)
    {
        error("Internal error: endLoop without beginLoop");
        return;
    }
    loopDepth_--;
    LoopContext &ctx = loopContexts_[loopDepth_];
    for (int i = 0; i < ctx.breakCount; i++)
    {
        patchJump(ctx.breakJumps[i]);
    }
}

int Compiler::discardLocals(int depth)
{
    int local = localCount_ - 1;
    while (local >= 0 && locals_[local].depth >= depth)
    {
        emitByte(OP_POP);
        local--;
    }
    return localCount_ - local - 1;
}

void Compiler::emitBreak()
{
    if (loopDepth_ == 0)
    {
        error("Cannot use 'break' outside of a loop");
        return;
    }

    if (tryDepth > 0)
    {
        error("Cannot use 'break' inside try-catch-finally block");
        return;
    }

    LoopContext &ctx = loopContexts_[loopDepth_ - 1];

    discardLocals(ctx.scopeDepth + 1);

    if (ctx.isForeach)
    {
        emitDiscard(2);
    }

    if (!ctx.addBreak(emitJump(OP_JUMP)))
    {
        error("Too many breaks");
    }
}

void Compiler::emitContinue()
{
    if (loopDepth_ == 0)
    {
        error("Cannot use 'continue' outside of a loop");
        return;
    }
    if (tryDepth > 0)
    {
        error("Cannot use 'continue' inside try-catch-finally block");
        return;
    }
    LoopContext &ctx = loopContexts_[loopDepth_ - 1];

    discardLocals(ctx.scopeDepth + 1);

    emitLoop(ctx.loopStart);
}

void Compiler::whileStatement()
{
    int loopStart = currentChunk->count;

    consume(TOKEN_LPAREN, "Expect '(' after 'while'");
    expression();
    consume(TOKEN_RPAREN, "Expect ')' after condition");

    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);

    beginLoop(loopStart); // Guarda loopStart SEM scope

    statement(); // Se for {}, o bloco cria o próprio scope

    emitLoop(loopStart);

    endLoop(); // Patch dos breaks

    patchJump(exitJump);
    emitByte(OP_POP);
}
void Compiler::doWhileStatement()
{
    // do
    consume(TOKEN_LBRACE, "Expect '{' after 'do'");

    int loopStart = currentChunk->count;

    beginLoop(loopStart);

    // BODY (executa primeiro)
    beginScope();
    block();
    endScope();

    // while (condition)
    consume(TOKEN_WHILE, "Expect 'while' after do body");
    consume(TOKEN_LPAREN, "Expect '(' after 'while'");
    expression(); // Condição
    consume(TOKEN_RPAREN, "Expect ')' after condition");
    consume(TOKEN_SEMICOLON, "Expect ';' after do-while");

    // Se condição for TRUE, volta ao início
    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP); // Pop se true
    emitLoop(loopStart);

    patchJump(exitJump);
    emitByte(OP_POP); // Pop se false

    endLoop();
}

void Compiler::loopStatement()
{
    // loop { ... }

    int loopStart = currentChunk->count;

    beginLoop(loopStart);

    // Body
    consume(TOKEN_LBRACE, "Expect '{' after 'loop'");
    beginScope();
    block();
    endScope();

    // Volta sempre ao início (loop infinito)
    emitLoop(loopStart);

    // Patch dos breaks (única forma de sair!)
    endLoop();
}

void Compiler::switchStatement()
{
    consume(TOKEN_LPAREN, "Expect '(' after 'switch'");
    expression(); // [value]
    consume(TOKEN_RPAREN, "Expect ')' after switch expression");
    consume(TOKEN_LBRACE, "Expect '{' before switch body");

    std::vector<int> endJumps;
    std::vector<int> caseFailJumps;

    // Parse cases
    while (match(TOKEN_CASE))
    {
        emitByte(OP_DUP); // [value, value]
        expression();     // [value, value, case_val]
        consume(TOKEN_COLON, "Expect ':' after case value");
        emitByte(OP_EQUAL); // [value, bool]

        int caseJump = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP); // [value] - Pop comparison result

        emitByte(OP_POP); // []

        // Case body
        while (!check(TOKEN_CASE) && !check(TOKEN_DEFAULT) &&
               !check(TOKEN_RBRACE) && !check(TOKEN_EOF))
        {
            statement();
        }

        endJumps.push_back(emitJump(OP_JUMP));

        patchJump(caseJump);
        emitByte(OP_POP); // [value] - Pop comparison result
    }

    // Default (opcional)
    if (match(TOKEN_DEFAULT))
    {
        consume(TOKEN_COLON, "Expect ':' after 'default'");

        emitByte(OP_POP); // []

        while (!check(TOKEN_CASE) && !check(TOKEN_RBRACE) && !check(TOKEN_EOF))
        {
            statement();
        }
    }
    else
    {

        emitByte(OP_POP); // []
    }

    consume(TOKEN_RBRACE, "Expect '}' after switch body");

    for (int jump : endJumps)
    {
        patchJump(jump);
    }
}

void Compiler::breakStatement()
{

    emitBreak();
    consume(TOKEN_SEMICOLON, "Expect ';' after 'break'");
}

void Compiler::continueStatement()
{
    emitContinue();
    consume(TOKEN_SEMICOLON, "Expect ';' after 'continue'");
}

void Compiler::forStatement()
{
    // for cria um scope próprio para o initializer
    beginScope();

    consume(TOKEN_LPAREN, "Expect '(' after 'for'");

    // INITIALIZER (opcional)
    // Pode ser: var i = 0; ou i = 0; ou vazio
    if (match(TOKEN_SEMICOLON))
    {
        // Sem initializer
    }
    else if (match(TOKEN_VAR))
    {
        varDeclaration(); // var i = 0;
    }
    else
    {
        expressionStatement(); // i = 0;
    }

    // Marca onde o loop começa (para continue e para o loop)
    int loopStart = currentChunk->count;

    // CONDITION (opcional)
    int exitJump = -1;
    if (!check(TOKEN_SEMICOLON))
    {
        expression(); // i < 10
        consume(TOKEN_SEMICOLON, "Expect ';' after loop condition");

        // salta para fora se condição for falsa
        exitJump = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP); // Pop da condição
    }
    else
    {
        consume(TOKEN_SEMICOLON, "Expect ';' after loop condition");
    }

    // INCREMENT (opcional)
    // Problema: increment vem ANTES do body no código, mas executa DEPOIS
    // Solução: saltar o increment, executar body, depois voltar pro increment
    if (!check(TOKEN_RPAREN))
    {
        // salta sobre o código do increment
        int bodyJump = emitJump(OP_JUMP);

        int incrementStart = currentChunk->count;
        expression();     // i = i + 1
        emitByte(OP_POP); // Pop do resultado
        consume(TOKEN_RPAREN, "Expect ')' after for clauses");

        // Volta para o início do loop (condition)
        emitLoop(loopStart);

        // Agora loopStart aponta para o increment (para continue)
        loopStart = incrementStart;

        // Patch do bodyJump para saltar o increment
        patchJump(bodyJump);
    }
    else
    {
        consume(TOKEN_RPAREN, "Expect ')' after for clauses");
    }

    // Registra o loop para break/continue
    beginLoop(loopStart);

    // BODY
    statement();

    // Volta para o increment (ou condition se não houver increment)
    emitLoop(loopStart);

    // Patch do exitJump (se houver condition)
    if (exitJump != -1)
    {
        patchJump(exitJump);
        emitByte(OP_POP); // Pop da condição falsa
    }

    endLoop();  // Patch dos breaks
    endScope(); // Limpa variáveis do initializer
}

void Compiler::foreachStatement()
{
    consume(TOKEN_LPAREN, "Expect '(' after 'foreach'");
    consume(TOKEN_IDENTIFIER, "Expect variable name");
    Token itemName = previous;
    consume(TOKEN_IN, "Expect 'in'");

    expression();
    consume(TOKEN_RPAREN, "Expect ')'");

    Token tmp;
    tmp.lexeme = "__seq___";
    tmp.type = TOKEN_IDENTIFIER;
    tmp.column = previous.column;
    addLocal(tmp);
    markInitialized();
    emitByte(OP_NIL);
    tmp.lexeme = "__iter__";
    addLocal(tmp);
    markInitialized();
    int loopStart = currentChunk->count;
    beginLoop(loopStart, true);

    emitByte(OP_COPY2);
    emitByte(OP_ITER_NEXT);

    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);

    emitByte(OP_SWAP);
    emitByte(OP_POP);

    emitByte(OP_COPY2);
    emitByte(OP_ITER_VALUE);

    beginScope();
    addLocal(itemName);
    markInitialized();
    statement();

    endScope(); // Remove item (slot 2), faz POP

    emitLoop(loopStart);

    patchJump(exitJump);
    emitDiscard(4);

    localCount_ -= 2; // Remove seq e iter

    endLoop();
}

// void Compiler::foreachStatement()
// {
//     consume(TOKEN_LPAREN, "Expect '(' after 'foreach'");
//     consume(TOKEN_IDENTIFIER, "Expect variable name");
//     Token itemName = previous;
//     consume(TOKEN_IN, "Expect 'in'");

//     expression();
//     consume(TOKEN_RPAREN, "Expect ')'");

//     beginScope();

//     Token tmp;
//     tmp.lexeme = "__seq___";
//     tmp.type = TOKEN_IDENTIFIER;
//     tmp.column = previous.column;

//     addLocal(tmp);
//     markInitialized();
//     uint8_t seqSlot = localCount_ - 1;
//     emitByte(OP_SET_LOCAL);
//     emitByte(seqSlot);

//     // var __iter = nil
//     emitByte(OP_NIL);
//     tmp.lexeme = "__ite___";
//     addLocal(tmp);
//     markInitialized();
//     uint8_t iterSlot = localCount_ - 1;
//     emitByte(OP_SET_LOCAL);
//     emitByte(iterSlot);

//     int loopStart = currentChunk->count;
//     beginLoop(loopStart);

//     emitByte(OP_GET_LOCAL);
//     emitByte(seqSlot);
//     emitByte(OP_GET_LOCAL);
//     emitByte(iterSlot);
//     emitByte(OP_ITER_NEXT);

//     int exitJump = emitJump(OP_JUMP_IF_FALSE);
//     emitByte(OP_POP);  //   POP (bool)

//     emitByte(OP_SET_LOCAL);
//     emitByte(iterSlot);

//     emitByte(OP_GET_LOCAL);
//     emitByte(seqSlot);
//     emitByte(OP_GET_LOCAL);
//     emitByte(iterSlot);
//     emitByte(OP_ITER_VALUE);

//     beginScope();
//     addLocal(itemName);
//     markInitialized();
//     uint8_t itemSlot = localCount_ - 1;
//     emitByte(OP_SET_LOCAL);
//     emitByte(itemSlot);

//     statement();
//     endScope();

//     emitLoop(loopStart);

//     patchJump(exitJump);

//     endLoop();
//     endScope();
// }

void Compiler::returnStatement()
{

    if (isProcess_)
    {
        consume(TOKEN_SEMICOLON, "Expect ';'"); // quando usamo gosub nos processos
        emitByte(OP_RETURN_SUB);
        return;
    }

    if (function == nullptr)
    {
        error("Can't return from top-level code");
        return;
    }

    if (match(TOKEN_SEMICOLON))
    {
        emitReturn();
    }
    else
    {
        if (currentFunctionType == FunctionType::TYPE_INITIALIZER)
        {
            error("Cannot return a value from an initializer");
            return;
        }
        // return <expr>;
        expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after return value");
        emitByte(OP_RETURN);
    }

    function->hasReturn = true;
}

uint8 Compiler::argumentList()
{
    uint8 argCount = 0;

    if (!check(TOKEN_RPAREN))
    {
        do
        {
            expression();

            if (argCount == 255)
            {
                error("Can't have more than 255 arguments");
            }
            argCount++;
        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RPAREN, "Expect ')' after arguments");
    return argCount;
}

void Compiler::call(bool canAssign)
{
    (void)canAssign;
    uint8 argCount = argumentList();
    emitByte(OP_CALL);
    emitByte(argCount);
}

void Compiler::funDeclaration()
{
    consume(TOKEN_IDENTIFIER, "Expect function name");
    Token nameToken = previous;

    Function *func = vm_->addFunction(nameToken.lexeme.c_str(), 0);

    if (!func)
    {
        error("Function already exists");
        return;
    }

    // Compila função
    compileFunction(func, false); // false = não é process

    // Emite constant com o index da função
    emitConstant(vm_->makeFunction(func->index));
    // Define como global
    uint8 nameConstant = identifierConstant(nameToken);
    defineVariable(nameConstant);
}

void Compiler::processDeclaration()
{
    consume(TOKEN_IDENTIFIER, "Expect process name");
    Token nameToken = previous;
    isProcess_ = true;
    argNames.clear();

    // Warning("Compiling process '%s'", nameToken.lexeme.c_str());

    // Cria função para o process

    Function *func = vm_->addFunction(nameToken.lexeme.c_str(), 0);

    if (!func)
    {
        error("Function already exists");
        return;
    }

    // Compila processo
    compileFunction(func, true); // true = É PROCESS!

    // Cria blueprint (process não vai para globals como callable)
    ProcessDef *proc = vm_->addProcess(nameToken.lexeme.c_str(), func);

    for (uint32 i = 0; i < argNames.size(); i++)
    {
        int privateIndex = vm_->getProcessPrivateIndex(argNames[i]->chars());

        if (privateIndex >= 0)
        {
            if (privateIndex == (int)PrivateIndex::ID)
            {
                Warning("Property 'ID' is readonly!");
            }
            else if (privateIndex == (int)PrivateIndex::FATHER)
            {
                Warning("Property 'FATHER' is readonly!");
            }
            else
            {
                proc->argsNames.push((uint8)privateIndex);
            }
        }
        else
        {

            proc->argsNames.push(255); // Marcador "sem private"
        }
    }
    argNames.clear();

    // Warning("Process '%s' registered with index %d", nameToken.lexeme.c_str(), index);

    emitConstant(vm_->makeProcess(proc->index));
    uint8 nameConstant = identifierConstant(nameToken);
    defineVariable(nameConstant);

    proc->finalize();

    isProcess_ = false;
}

void Compiler::compileFunction(Function *func, bool isProcess)
{
    // Salva estado
    Function *enclosing = this->function;
    Code *enclosingChunk = this->currentChunk;
    int enclosingScopeDepth = this->scopeDepth;
    int enclosingLocalCount = this->localCount_;
    bool wasInProcess = this->isProcess_;

    // Troca contexto
    this->function = func;
    this->currentChunk = func->chunk;
    this->scopeDepth = 0;
    this->localCount_ = 0;
    this->isProcess_ = isProcess;
    labels.clear();
    pendingGotos.clear();
    pendingGosubs.clear();

    if (!func)
    {
        Error("Error in funcion");
        return;
    }

    if (!func->chunk)
    {
        Error("Error in funcion code");
        return;
    }

    // Parse parâmetros
    beginScope();
    consume(TOKEN_LPAREN, "Expect '(' after name");
    if (!isProcess)
    {
        Token dummyToken;
        dummyToken.lexeme = func->name->chars();
        addLocal(dummyToken);
        markInitialized();
    }

    if (!check(TOKEN_RPAREN))
    {
        do
        {
            func->arity++;
            if (func->arity > 255)
            {
                error("Can't have more than 255 parameters");
                break;
            }

            consume(TOKEN_IDENTIFIER, "Expect parameter name");
            if (isProcess)
            {
                argNames.push(vm_->createString(previous.lexeme.c_str()));
            }
            addLocal(previous);
            markInitialized();

        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RPAREN, "Expect ')' after parameters");

    // Parse corpo
    consume(TOKEN_LBRACE, "Expect '{' before body");
    block();
    endScope();

    resolveGotos();
    resolveGosubs();

    labels.clear();
    pendingGotos.clear();
    pendingGosubs.clear();

    if (!function->hasReturn)
    {
        emitReturn();
    }

    // Restaura estado
    this->function = enclosing;
    this->currentChunk = enclosingChunk;
    this->scopeDepth = enclosingScopeDepth;
    this->localCount_ = enclosingLocalCount;
    this->isProcess_ = wasInProcess;
}

void Compiler::prefixIncrement(bool canAssign)
{
    (void)canAssign;
    // ++i
    // previous = '++', current deve ser identifier

    if (!check(TOKEN_IDENTIFIER))
    {
        error("Expect variable name after '++'");
        return;
    }

    advance();             // Consome o identifier manualmente
    Token name = previous; // Agora previous é o identifier

    uint8 getOp, setOp;
    int arg = resolveLocal(name);

    if (arg != -1)
    {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    }
    else
    {
        arg = identifierConstant(name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    // i = i + 1
    emitBytes(getOp, (uint8)arg);
    emitConstant(vm_->makeInt(1));
    emitByte(OP_ADD);
    emitBytes(setOp, (uint8)arg);

    // Lê o novo valor para retornar
    emitBytes(getOp, (uint8)arg);
}

void Compiler::prefixDecrement(bool canAssign)
{
    (void)canAssign;
    // --i

    if (!check(TOKEN_IDENTIFIER))
    {
        error("Expect variable name after '--'");
        return;
    }

    advance();
    Token name = previous;

    uint8 getOp, setOp;
    int arg = resolveLocal(name);

    if (arg != -1)
    {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    }
    else
    {
        arg = identifierConstant(name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    emitBytes(getOp, (uint8)arg);
    emitConstant(vm_->makeInt(1));
    emitByte(OP_SUBTRACT);
    emitBytes(setOp, (uint8)arg);

    emitBytes(getOp, (uint8)arg);
}

void Compiler::frameStatement()
{
    if (!isProcess_)
    {
        error("'frame' can only be used in process body");
        return;
    }

    if (match(TOKEN_LPAREN))
    {
        // frame(expression)
        expression(); // Percentagem vai para stack
        consume(TOKEN_RPAREN, "Expect ')' after percentage");
    }
    else
    {
        // frame; = frame(100);
        emitBytes(OP_CONSTANT, makeConstant(vm_->makeInt(100)));
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after frame");
    emitByte(OP_FRAME);
}

void Compiler::exitStatement()
{
    if (!isProcess_)
    {
        error("'exit' can only be used in process body");
        return;
    }

    if (match(TOKEN_LPAREN))
    {
        // exit(expression)
        expression();
        consume(TOKEN_RPAREN, "Expect ')' after exit code");
    }
    else
    {
        // exit;
        emitConstant(vm_->makeInt(0));
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after exit");
    emitByte(OP_EXIT);
}

void Compiler::includeStatement()
{
    consume(TOKEN_STRING, "Expect filename after include");

    std::string filename = previous.lexeme.c_str();

    // std::set para proteção circular
    if (includedFiles.find(filename) != includedFiles.end())
    {
        fail("Circular include: %s", filename.c_str());
        return;
    }

    if (!fileLoader)
    {
        fail("No file loader set");
        return;
    }

    // CALLBACK retorna C-style
    size_t sourceSize = 0;
    const char *source = fileLoader(filename.c_str(), &sourceSize, fileLoaderUserdata);

    if (!source || sourceSize == 0)
    {
        fail("Cannot load %s %d", filename.c_str(), sourceSize);
        return;
    }

    // Adiciona ao set
    includedFiles.insert(filename);

    // SALVA estado
    Lexer *oldLexer = this->lexer;
    std::vector<Token> oldTokens = this->tokens;
    Token oldCurrent = this->current;
    Token oldPrevious = this->previous;
    int oldCursor = this->cursor;

    // COMPILA inline
    this->lexer = new Lexer(source, sourceSize);
    this->tokens = lexer->scanAll();
    this->cursor = 0;
    advance();

    while (!check(TOKEN_EOF) && !hadError)
    {
        declaration();
    }

    // RESTAURA
    delete this->lexer;
    this->lexer = oldLexer;
    this->tokens = oldTokens;
    this->current = oldCurrent;
    this->previous = oldPrevious;
    this->cursor = oldCursor;

    // Remove do set
    includedFiles.erase(filename);

    consume(TOKEN_SEMICOLON, "Expect ';' after include");
}

void Compiler::parseUsing()
{
    // using raylib, math;

    do
    {
        consume(TOKEN_IDENTIFIER, "Expect module name");
        Token moduleName = previous;
        std::string modName = moduleName.lexeme;

        if (importedModules.find(modName) == importedModules.end())
        {
            fail("Module '%s' not imported. Use 'import %s;' first",
                 moduleName.lexeme.c_str(),
                 moduleName.lexeme.c_str());
            return;
        }

        if (usingModules.find(modName) != usingModules.end())
        {
            Warning("Module '%s' already using", moduleName.lexeme.c_str());
        }
        else
        {
            usingModules.insert(modName);
        }

    } while (match(TOKEN_COMMA));

    consume(TOKEN_SEMICOLON, "Expect ';'");
}

void Compiler::parseImport()
{
    // import *;
    if (match(TOKEN_STAR))
    {
        // Importa TODOS os módulos definidos
        for (size_t i = 0; i < vm_->modules.size(); i++)
        {
            ModuleDef *mod = vm_->modules[i];
            importedModules.insert(mod->getName()->chars());
        }
        consume(TOKEN_SEMICOLON, "Expect ';'");
        return;
    }

    // import timer, net, fs;
    do
    {
        consume(TOKEN_IDENTIFIER, "Expect module name");
        Token moduleName = previous;
        std::string modName = moduleName.lexeme;

        // Verifica se módulo existe
        if (!vm_->containsModule(modName.c_str()))
        {
            fail("Module '%s' not defined", moduleName.lexeme.c_str());
            return;
        }

        //  Adiciona a importedModules, não usingModules!
        if (importedModules.find(modName) == importedModules.end())
        {
            importedModules.insert(modName);
        }
        else
        {
            Warning("Module '%s' already imported", moduleName.lexeme.c_str());
        }

    } while (match(TOKEN_COMMA));

    consume(TOKEN_SEMICOLON, "Expect ';' after import");
}

void Compiler::yieldStatement()
{

    if (match(TOKEN_LPAREN))
    {
        expression(); // Percentagem vai para stack
        consume(TOKEN_RPAREN, "Expect ')' after percentage");
    }
    else
    {

        emitBytes(OP_CONSTANT, makeConstant(vm_->makeDouble(1.0)));
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after yild");
    emitByte(OP_YIELD);
}

void Compiler::fiberStatement()
{
    consume(TOKEN_IDENTIFIER, "Expect function name after 'fiber'.");
    Token nameToken = previous;

    namedVariable(nameToken, false); // empilha callee

    consume(TOKEN_LPAREN, "Expect '(' after fiber function name.");

    uint8 argCount = 0;

    if (!check(TOKEN_RPAREN))
    {
        do
        {
            expression();

            if (argCount == 255)
            {
                error("Can't have more than 255 arguments");
            }
            argCount++;
        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RPAREN, "Expect ')' after arguments");
    Warning("Compiling fiber call to '%s' with %d arguments", nameToken.lexeme.c_str(), argCount);

    consume(TOKEN_SEMICOLON, "Expect ';' after fiber call.");

    emitByte(OP_SPAWN);
    emitByte(argCount);
}

void Compiler::dot(bool canAssign)
{
    consume(TOKEN_IDENTIFIER, "Expect property name after '.'");
    Token propName = previous;

    uint8_t nameIdx = identifierConstant(propName);

    //  METHOD CALL
    if (match(TOKEN_LPAREN))
    {

        uint8_t argCount = argumentList();
        emitBytes(OP_INVOKE, nameIdx);
        emitByte(argCount);
    }
    // SIMPLE ASSIGNMENT
    else if (canAssign && match(TOKEN_EQUAL))
    {
        expression();
        emitBytes(OP_SET_PROPERTY, nameIdx);
    }
    //  COMPOUND ASSIGNMENTS
    else if (canAssign && match(TOKEN_PLUS_EQUAL))
    {
        // self.x += value
        // Stack antes: [self]
        emitByte(OP_DUP);                    // [self, self]
        emitBytes(OP_GET_PROPERTY, nameIdx); // [self, old_x]
        expression();                        // [self, old_x, value]
        emitByte(OP_ADD);                    // [self, new_x]
        emitBytes(OP_SET_PROPERTY, nameIdx); // []
    }
    else if (canAssign && match(TOKEN_MINUS_EQUAL))
    {
        emitByte(OP_DUP);
        emitBytes(OP_GET_PROPERTY, nameIdx);
        expression();
        emitByte(OP_SUBTRACT);
        emitBytes(OP_SET_PROPERTY, nameIdx);
    }
    else if (canAssign && match(TOKEN_STAR_EQUAL))
    {
        emitByte(OP_DUP);
        emitBytes(OP_GET_PROPERTY, nameIdx);
        expression();
        emitByte(OP_MULTIPLY);
        emitBytes(OP_SET_PROPERTY, nameIdx);
    }
    else if (canAssign && match(TOKEN_SLASH_EQUAL))
    {
        emitByte(OP_DUP);
        emitBytes(OP_GET_PROPERTY, nameIdx);
        expression();
        emitByte(OP_DIVIDE);
        emitBytes(OP_SET_PROPERTY, nameIdx);
    }
    else if (canAssign && match(TOKEN_PERCENT_EQUAL))
    {
        emitByte(OP_DUP);
        emitBytes(OP_GET_PROPERTY, nameIdx);
        expression();
        emitByte(OP_MODULO);
        emitBytes(OP_SET_PROPERTY, nameIdx);
    }
    //  INCREMENT/DECREMENT
    else if (canAssign && match(TOKEN_PLUS_PLUS))
    {
        // self.x++ (postfix)
        emitByte(OP_DUP);                    // [self, self]
        emitBytes(OP_GET_PROPERTY, nameIdx); // [self, old_x]
        emitConstant(vm_->makeInt(1));       // [self, old_x, 1]
        emitByte(OP_ADD);                    // [self, new_x]
        emitBytes(OP_SET_PROPERTY, nameIdx); // []
    }
    else if (canAssign && match(TOKEN_MINUS_MINUS))
    {
        // self.x-- (postfix)
        emitByte(OP_DUP);
        emitBytes(OP_GET_PROPERTY, nameIdx);
        emitConstant(vm_->makeInt(1));
        emitByte(OP_SUBTRACT);
        emitBytes(OP_SET_PROPERTY, nameIdx);
    }
    //  GET ONLY
    else
    {
        emitBytes(OP_GET_PROPERTY, nameIdx);
    }
}

void Compiler::subscript(bool canAssign)
{
    // arr[index] ou arr[index] = value

    expression(); // Index expression
    consume(TOKEN_RBRACKET, "Expect ']' after subscript");

    if (canAssign && match(TOKEN_EQUAL))
    {
        // arr[i] = value
        expression(); // Value
        emitByte(OP_SET_INDEX);
    }
    else
    {
        // arr[i]
        emitByte(OP_GET_INDEX);
    }
}

void Compiler::labelStatement()
{
    consume(TOKEN_IDENTIFIER, "Expect label");
    Token name = previous;
    consume(TOKEN_COLON, "Expect ':'");

    Label label;
    label.name = name.lexeme;
    label.offset = currentChunk->count;
    for (auto &l : labels)
    {
        if (l.name == name.lexeme)
        {
            fail("Duplicate '%s' label", name.lexeme.c_str());
        }
    }
    labels.push_back(label);
}

void Compiler::gotoStatement()
{
    consume(TOKEN_IDENTIFIER, "Expect label");
    Token target = previous;
    consume(TOKEN_SEMICOLON, "Expect ';'");

    // Backward
    for (const Label &l : labels)
    {
        if (l.name == target.lexeme)
        {
            emitLoop(l.offset);
            return;
        }
    }

    // Forward
    GotoJump jump;
    jump.target = target.lexeme;
    jump.jumpOffset = emitJump(OP_JUMP);
    pendingGotos.push_back(jump);
}

void Compiler::gosubStatement()
{
    consume(TOKEN_IDENTIFIER, "Expect label");
    Token target = previous;
    consume(TOKEN_SEMICOLON, "Expect ';'");

    // se já existe label (pode ser backward)
    for (const Label &l : labels)
    {
        if (l.name == target.lexeme)
        {
            emitGosubTo(l.offset);
            return;
        }
    }

    // forward: placeholder
    GotoJump j;
    j.target = target.lexeme;
    j.jumpOffset = emitJump(OP_GOSUB); // devolve offset do OPERANDO
    pendingGosubs.push_back(j);
}

void Compiler::structDeclaration()
{
    consume(TOKEN_IDENTIFIER, "Expect struct name");
    Token structName = previous;
    uint8_t nameConstant = identifierConstant(structName);

    consume(TOKEN_LBRACE, "Expect '{' before struct body");

    StructDef *structDef = vm_->registerStruct(vm_->createString(structName.lexeme.c_str()));

    if (!structDef)
    {
        fail("Struct with name '%s' already exists", structName.lexeme.c_str());
        return;
    }

    structDef->argCount = 0;

    //  Loop externo: múltiplas linhas de fields
    while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF))
    {
        //  Opcional: pode ter 'var' ou não
        bool hasVar = match(TOKEN_VAR);

        //  Loop interno: múltiplos fields separados por vírgula
        do
        {
            consume(TOKEN_IDENTIFIER, "Expect field name");

            String *fieldName = vm_->createString(previous.lexeme.c_str());
            structDef->names.set(fieldName, structDef->argCount);
            structDef->argCount++;

        } while (match(TOKEN_COMMA));

        //  Se tinha 'var', precisa de ';'
        if (hasVar)
        {
            consume(TOKEN_SEMICOLON, "Expect ';' after field declaration");
        }
        //  Se não tinha 'var', aceita ',' ou '}'
    }

    consume(TOKEN_RBRACE, "Expect '}' after struct body");
    consume(TOKEN_SEMICOLON, "Expect ';' after struct");

    emitConstant(vm_->makeStruct(structDef->index));
    defineVariable(nameConstant);
}

void Compiler::self(bool canAssign)
{
    (void)canAssign;
    if (currentClass == nullptr)
    {
        error("Cannot use 'self' outside of a class");
        return;
    }
    Token selfToken;
    selfToken.lexeme = "self";
    selfToken.type = TOKEN_IDENTIFIER;
    namedVariable(selfToken, canAssign);
}

void Compiler::super(bool canAssign)
{
    (void)canAssign;

    if (currentClass == nullptr)
    {
        error("Cannot use 'super' outside of a class");
        return;
    }

    if (currentClass->superclass == nullptr)
    {
        error("Cannot use 'super' in a class with no superclass");
        return;
    }

    consume(TOKEN_DOT, "Expect '.' after 'super'");
    consume(TOKEN_IDENTIFIER, "Expect superclass method name");
    Token methodName = previous;
    uint8_t nameIdx = identifierConstant(methodName);

    consume(TOKEN_LPAREN, "Expect '(' after method name");

    // SELF PRIMEIRO!
    emitBytes(OP_GET_LOCAL, 0);

    // DEPOIS ARGUMENTOS!
    uint8_t argCount = argumentList();

    // printf("[COMPILER] super.%s em classe %s (ID=%d), super=%s\n",
    //        methodName.lexeme.c_str(),
    //        currentClass->name->chars(),
    //        currentClass->index,
    //        currentClass->superclass->name->chars());

    emitBytes(OP_SUPER_INVOKE, currentClass->index);
    emitByte(nameIdx);
    emitByte(argCount);
}

void Compiler::classDeclaration()
{
    consume(TOKEN_IDENTIFIER, "Expect class name");
    Token className = previous;
    uint8_t nameConstant = identifierConstant(className);

    //  Regista class blueprint na VM

    ClassDef *classDef = vm_->registerClass(
        vm_->createString(className.lexeme.c_str()));

    if (!classDef)
    {
        fail("Class with name '%s' already exists", className.lexeme.c_str());
        return;
    }

    // printf("ClassDef criado: %p, ID: %d\n", (void*)classDef, classId);
    // printf("fieldCount inicial: %d\n", classDef->fieldCount);

    // Emite class ID como constante
    emitConstant(vm_->makeClass(classDef->index));
    defineVariable(nameConstant);

    // Herança?
    if (match(TOKEN_COLON))
    {
        consume(TOKEN_IDENTIFIER, "Expect superclass name");
        Token superName = previous;

        const char *name = superName.lexeme.c_str();

        ClassDef *classSuper = nullptr;
        if (!vm_->tryGetClassDefenition(name, &classSuper))
        {
            fail("Undefined superclass '%s'", superName.lexeme.c_str());
            return;
        }
        if (classSuper == classDef)
        {
            fail("A class cannot inherit from itself");
            return;
        }
        classDef->inherited = true;
        classDef->parent = classSuper->name;
        classDef->superclass = classSuper;
        // //  Copia fieldCount também ( herdar fields ??)
        classSuper->fieldNames.forEach([&](String *fieldName, uint8_t index)
                                       {
            classDef->fieldNames.set(fieldName, classDef->fieldCount);
            classDef->fieldCount++; });
    }
    consume(TOKEN_LBRACE, "Expect '{'");

    while (check(TOKEN_VAR))
    {
        advance(); // Consome 'var'
        do
        {
            consume(TOKEN_IDENTIFIER, "Expect field name");
            Token fieldName = previous;

            String *name = vm_->createString(fieldName.lexeme.c_str());
            classDef->fieldNames.set(name, classDef->fieldCount);
            classDef->fieldCount++;

            // Ignora inicialização (vai no init)
            if (match(TOKEN_EQUAL) && !check(TOKEN_COMMA))
            {
                expression();
                emitByte(OP_POP);
            }
            else if (match(TOKEN_EQUAL))
            {
                expression();
                emitByte(OP_POP);
            }

        } while (match(TOKEN_COMMA)); //  Continua se tem vírgula

        consume(TOKEN_SEMICOLON, "Expect ';'");
    }
    // Parse métodos
    while (match(TOKEN_DEF))
    {
        method(classDef); // Passa ClassDef!
    }

    consume(TOKEN_RBRACE, "Expect '}'");

    if (classDef->constructor == nullptr)
    {
        Warning("Class '%s' has no init() method - fields will be uninitialized (nil)",
                className.lexeme.c_str());
    }

    //    Debug::dumpFunction(classDef->constructor);
}

void Compiler::method(ClassDef *classDef)
{
    consume(TOKEN_IDENTIFIER, "Expect method name");
    Token methodName = previous;

    // Tipo de função
    this->currentFunctionType = FunctionType::TYPE_METHOD;
    // Registra função
    //    std::string funcName = classDef->name->chars() +std::string("::") + methodName.lexeme;
    std::string funcName = methodName.lexeme;
    Function *func = classDef->canRegisterFunction(vm_->createString(funcName.c_str()));
    if (!func)
    {
        fail("Function '%s' already exists in class '%s' ", funcName.c_str(), classDef->name->chars());
        return;
    }

    if (funcName == "init")
    {
        classDef->constructor = func;
        this->currentFunctionType = FunctionType::TYPE_INITIALIZER;
    }

    vm_->addFunctionsClasses(func);

    // ===== SALVA ESTADO =====
    Function *enclosing = this->function;
    Code *enclosingChunk = this->currentChunk;
    int enclosingScopeDepth = this->scopeDepth;
    int enclosingLocalCount = this->localCount_;
    ClassDef *enclosingClass = this->currentClass;

    // ===== TROCA CONTEXTO =====
    this->function = func;
    this->currentChunk = func->chunk;
    this->scopeDepth = 0;
    this->localCount_ = 0;
    this->currentClass = classDef;

    // ===== SCOPE DA FUNÇÃO =====
    beginScope(); // scopeDepth = 1

    // ===== SELF = LOCAL[0] =====
    Token selfToken;
    selfToken.lexeme = "self";
    selfToken.type = TOKEN_IDENTIFIER;

    addLocal(selfToken);
    markInitialized();

    // ===== PARAMS =====
    consume(TOKEN_LPAREN, "Expect '('");

    if (!check(TOKEN_RPAREN))
    {
        do
        {
            func->arity++;
            if (func->arity > 255)
            {
                error("Can't have more than 255 parameters");
                break;
            }

            consume(TOKEN_IDENTIFIER, "Expect parameter name");
            addLocal(previous);
            markInitialized();

        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RPAREN, "Expect ')'");

    // ===== CORPO =====
    consume(TOKEN_LBRACE, "Expect '{'");
    block(); // block() chama beginScope/endScope internamente

    // Os parâmetros (self + args) ficam até o return

    // ===== RETURN =====
    if (currentFunctionType == FunctionType::TYPE_INITIALIZER)
    {
        // init() retorna self automaticamente
        emitBytes(OP_GET_LOCAL, 0); // self
        emitByte(OP_RETURN);
        function->hasReturn = true;
    }
    else if (!function->hasReturn) // sempre retorna ??
    {
        emitBytes(OP_GET_LOCAL, 0); // self
        emitByte(OP_RETURN);

        function->hasReturn = true;
    }

    endScope();

    // ===== RESTAURA ESTADO =====
    this->function = enclosing;
    this->currentChunk = enclosingChunk;
    this->scopeDepth = enclosingScopeDepth;
    this->localCount_ = enclosingLocalCount;
    this->currentClass = enclosingClass;
    this->currentFunctionType = FunctionType::TYPE_SCRIPT;
}

void Compiler::tryStatement()
{
    consume(TOKEN_LBRACE, "Expect '{' after 'try'");
    tryDepth++;

    emitByte(OP_TRY);

    int catchAddrOffset = currentChunk->count;
    emitByte(0xFF);
    emitByte(0xFF);

    int finallyAddrOffset = currentChunk->count;
    emitByte(0xFF);
    emitByte(0xFF);

    // TRY BLOCK
    beginScope();
    block();
    endScope();

    emitByte(OP_POP_TRY);
    int tryExitJump = emitJump(OP_JUMP);

    // CATCH BLOCK
    int catchStart = -1;
    int catchExitJump = -1;

    if (match(TOKEN_CATCH))
    {
        catchStart = currentChunk->count;

        consume(TOKEN_LPAREN, "Expect '(' after 'catch'");
        consume(TOKEN_IDENTIFIER, "Expect exception variable");
        Token errorVar = previous;
        consume(TOKEN_RPAREN, "Expect ')'");
        consume(TOKEN_LBRACE, "Expect '{'");

        emitByte(OP_ENTER_CATCH);

        beginScope();
        addLocal(errorVar);
        markInitialized();

        block();
        endScope();

        emitByte(OP_POP_TRY);
        catchExitJump = emitJump(OP_JUMP); // ← CRÍTICO! Catch também pula para finally
    }

    // FINALLY BLOCK
    int finallyStart = -1;
    if (match(TOKEN_FINALLY))
    {
        finallyStart = currentChunk->count;

        // Patch tryExitJump para ir ao finally
        patchJump(tryExitJump);

        // Patch catchExitJump para ir ao finally (se houver catch)
        if (catchExitJump != -1)
        {
            patchJump(catchExitJump);
        }

        consume(TOKEN_LBRACE, "Expect '{'");

        emitByte(OP_ENTER_FINALLY);

        beginScope();
        block();
        endScope();

        emitByte(OP_EXIT_FINALLY);
    }
    else
    {
        // Sem finally
        patchJump(tryExitJump);
        if (catchExitJump != -1)
        {
            patchJump(catchExitJump);
        }
    }

    // Valida
    if (catchStart == -1 && finallyStart == -1)
    {
        error("Try must have catch or finally block");
    }

    // Patch endereços absolutos
    if (catchStart != -1)
    {
        currentChunk->code[catchAddrOffset] = (catchStart >> 8) & 0xFF;
        currentChunk->code[catchAddrOffset + 1] = catchStart & 0xFF;
    }

    if (finallyStart != -1)
    {
        currentChunk->code[finallyAddrOffset] = (finallyStart >> 8) & 0xFF;
        currentChunk->code[finallyAddrOffset + 1] = finallyStart & 0xFF;
    }
    tryDepth--;
}

void Compiler::throwStatement()
{
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after throw");
    emitByte(OP_THROW);
}