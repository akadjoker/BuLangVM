
#include "pch.h"

#include "Utils.hpp"

#include "Ast.hpp"
#include "Interpreter.hpp"



void ByteGenerator::visit(Node *node)
{
   INFO("NODE %s",node->ToString().c_str());


//alternaive 4 simples AST visitors
    // switch (node->type)
    // {
    // case NodeType::NODE_NONE:
    //     break;
    // case NodeType::EXLITERAL:
    //     visit_literal(static_cast<Literal*>(node));
    //     break;
    // case NodeType::EXPNUMBER:
    //     visit_number(static_cast<NumberLiteral*>(node));
    //     break;
    // case NodeType::EXPSTRING:
    //     visit_string(static_cast<StringLiteral*>(node));
    //     break;
    // case NodeType::EXPNIL:
    //     visit_nil(static_cast<NilLiteral*>(node));
    //     break;
    // case NodeType::EXBINARY:
    //     visit_binary(static_cast<Binary*>(node));
    //     break;
    // case NodeType::EXUNARY:
    //     visit_unary(static_cast<Unary*>(node));
    //     break;
    // case NodeType::EXGROUPING:
    //     visit_grouping(static_cast<Grouping*>(node));
    //     break;
    // case NodeType::PROGRAM:
    //     visit_program(static_cast<Program*>(node));
    //     break;
    // case NodeType::EXSTATEMNT:
    //     visit_expresion_statemnt(static_cast<ExpresionStatement*>(node));       
    //     break;
    // case NodeType::BLOCK:
    //     visit_block(static_cast<Block*>(node));
    //     break;
    // case NodeType::PRINT:
    //     visit_print(static_cast<Print*>(node));
    //     break;
    // default:
    //     INFO("UNKNOW NODE %s",node->ToString().c_str());

    // }
}


void ByteGenerator::visit_literal(Literal* node)
{
    INFO("visit_literal");
   //    emitByte(OpCode::NIL, node->line);
}


void ByteGenerator::visit_number(NumberLiteral* node)
{
   // INFO("visit_number %f", node->value);
    emitConstant(to_number(node->value),node->op.line);
}


void ByteGenerator::visit_string(StringLiteral* node)
{
   // INFO("visit_string %s", node->value.c_str());
    emitConstant(to_string(node->value),node->op.line);
}

void ByteGenerator::visit_boolean(BooleanLiteral *node)
{
  //  INFO("visit_boolean");
    emitConstant(to_boolean(node->value),node->op.line);
}

void ByteGenerator::visit_nil(NilLiteral* node) 
{
   // INFO("visit_nil");
    emitConstant(to_none(),node->op.line);
}



void ByteGenerator::visit_binary(Binary *node)
{
   //  INFO("visit_binary node %s ", node->operation.toString().c_str());
   ///  INFO("visit_binary  left %s ", node->left->op.toString().c_str());
  //   INFO("visit_binary   ight %s ",  node->right->op.toString().c_str());

    TokenType operation = node->operation.type;
    int line = node->operation.line;

    node->left->accept(this);//eval and push the result
    node->right->accept(this);//eval and push the result


    switch (operation)
    {
    case TokenType::PLUS:
        emitByte(OpCode::ADD,line);
        break;
    case TokenType::MINUS:
        emitByte(OpCode::SUBTRACT,line);
        break;
    case TokenType::STAR:
        emitByte(OpCode::MULTIPLY,line);
        break;
    case TokenType::SLASH:
        emitByte(OpCode::DIVIDE,line);
        break;
    case TokenType::MOD:
        emitByte(OpCode::MOD,line);
        break;
    case TokenType::BANG_EQUAL:
        emitByte(OpCode::NOT_EQUAL,line);
        break;
    case TokenType::EQUAL_EQUAL:
        emitByte(OpCode::EQUAL,line);
        break;
    case TokenType::GREATER:
        emitByte(OpCode::GREATER,line);
        break;
    case TokenType::GREATER_EQUAL:
        emitByte(OpCode::GREATER_EQUAL,line);
        break;
    case TokenType::LESS:
        emitByte(OpCode::LESS,line);
        break;
    case TokenType::LESS_EQUAL:
        emitByte(OpCode::LESS_EQUAL,line);
        break;
    case TokenType::AND:
        emitByte(OpCode::AND,line);
        break;
    case TokenType::OR:
        emitByte(OpCode::OR,line);
        break;
    case TokenType::XOR:
        emitByte(OpCode::XOR,line);
        break;

    default:
        INFO("UNKNOW NODE (%s) ON BINARY",node->operation.toString().c_str());
        break;
    
    }
      
}


void ByteGenerator::visit_unary(Unary *node)
{
   // INFO("visit_unary");

   TokenType operation = node->operation.type;
   int line = node->operation.line;

    node->right->accept(this);

    switch (operation)
    {
    case TokenType::MINUS:
        emitByte(OpCode::NEGATE, line);
        break;
    case TokenType::BANG:
        emitByte(OpCode::NOT, line);
        break;
    }

}


void ByteGenerator::visit_grouping(Grouping *node)
{
   // INFO("visit_grouping");
    node->expression->accept(this);
}

void ByteGenerator::visit_program(Program *node)
{
  //  INFO("visit_program");
    size_t count = node->statements.size();
    for (size_t i = 0; i < count; i++)
    {
        node->statements[i]->accept(this);
    }
    int l = static_cast<int>(line());
    emitByte(OpCode::NIL, l);
    emitReturn(l);


    for (size_t i = 0; i < current->patchList.size(); i++)
    {
       String name = current->patchList[i].name;
       int address = current->patchList[i].address;
       JumpLabel *label;
       if (current->getLabel(name, &label))
       {
           int offset = label->position - address - 2;
           patchAdress(address, offset);
            
        }else 
        {
            INFO("label not found %s", name.c_str());
        }
    } 


}

void ByteGenerator::visit_expresion_statemnt(ExpresionStatement *node)
{
   // INFO("visit_expresion_statemnt");
    node->expression->accept(this);
}

void ByteGenerator::visit_block(Block *node)
{
  //  INFO("visit_block");

    emitByte(OpCode::ENTER_SCOPE, node->op.line);

    size_t count = node->statements.size();
    for (size_t i = 0; i < count; i++)
    {
        node->statements[i]->accept(this);
    }

    emitByte(OpCode::EXIT_SCOPE, node->op.line);
}

void ByteGenerator::visit_print(Print *node)
{
   // INFO("visit_print");
    node->value->accept(this);
    emitByte(OpCode::PRINT, node->op.line);
}


void ByteGenerator::visit_declaration(Declaration *node)
{
    u8 index = addConstString(node->name.literal.c_str());
    node->initializer->accept(this);
    emitBytes(OpCode::GLOBAL_DEFINE, index, node->name.line);
}

void ByteGenerator::visit_variable(Variable *node)
{
   // INFO("visit_variable  %d", node->op.line);

    u8 index = addConstString(node->name.literal.c_str());
    emitBytes(OpCode::GLOBAL_GET, index, node->name.line);
    
}

void ByteGenerator::visit_assignment(Assignment *node)
{
    u8 index = addConstString(node->name.literal.c_str());

    node->value->accept(this);


    emitBytes(OpCode::GLOBAL_ASSIGN, index, node->name.line);
}

void ByteGenerator::visit_if(IFStatement *node)
{
    
    node->condition->accept(this);  //expresion   if ( expresion ) { ... }
    
    u8 theJump = emitJump(OpCode::JUMP_IF_FALSE, node->op.line);
    emitByte(OpCode::POP, node->op.line);
    node->thenBranch->accept(this); // block


    
    u8 elseJump = emitJump(OpCode::JUMP, node->op.line);
    patchJump(theJump);
    emitByte(OpCode::POP, node->op.line);


    if (node->elifBranch.size() > 0)
    {
        for (u32 i = 0; i < node->elifBranch.size(); i++)
        {
            node->elifBranch[i]->condition->accept(this);

            theJump = emitJump(OpCode::JUMP_IF_FALSE, node->op.line);
            emitByte(OpCode::POP, node->op.line);
            node->elifBranch[i]->body->accept(this);

            patchJump(elseJump);

            elseJump = emitJump(OpCode::JUMP, node->op.line);
            patchJump(theJump);
            emitByte(OpCode::POP, node->op.line);

        }
    }



    if (node->elseBranch != nullptr)
    {
        node->elseBranch->accept(this); // block
    }

    patchJump(elseJump);

}

void ByteGenerator::patchBreakJumps()
{
    for (int i = 0; i < current->breakCount; i++)
    {
        patchJump(current->breakJumps[i]);
    }
}

void ByteGenerator::visit_while(WhileStatement *node)
{


    
    int surroundingLoopStart =     current->loopStart;
    current->loopStart     =     count();
    int breakCounts =  current->breakCount;
    current->breakCount = 0;
    current->doContinue.clear();

    node->condition->accept(this);//compile expresion

    int exitJump = emitJump(OpCode::JUMP_IF_FALSE, node->op.line);
    emitByte(OpCode::POP, node->op.line);

    node->body->accept(this);//compile loop body 


    emitLoop(current->loopStart, node->op.line);

    patchBreakJumps();

    patchJump(exitJump);
    emitByte(OpCode::POP, node->op.line);



    current->loopStart = surroundingLoopStart;
    current->breakCount = breakCounts;
    
}

void ByteGenerator::visit_do(DoStatement *node)
{
    int previousLoopStart = current->loopStart;
    int previousBreakJumpCount = current->breakCount;
    
    current->breakCount = 0;
    current->doContinue.clear();
    
    current->loopStart = count();
    int exitJump = -1;

    current->doContinue.push_back(current->loopStart);//just to be safe this just mark do while loop for use with continue
    

    node->body->accept(this);

    for (u32 i = 1; i < current->doContinue.size(); i++)
    {
        patchJump(current->doContinue[i]); //fill the jumps for continue
        // emitByte(OpCode::POP, node->op.line);
    }
    
    node->condition->accept(this);

    exitJump = emitJump(OpCode::JUMP_IF_FALSE, node->op.line);
    emitByte(OpCode::POP, node->op.line);
    
    emitLoop(current->loopStart, node->op.line);
    
    patchJump(exitJump);
    emitByte(OpCode::POP, node->op.line);
    

    
    patchBreakJumps();

    


    current->loopStart = previousLoopStart;
    current->breakCount = previousBreakJumpCount;

}


void ByteGenerator::visit_switch(SwitchStatement *node)
{
    

    Vector<int> caseEnds;
    int previousCaseSkip = -1;
    int defaultJump = -1;

    node->condition->accept(this);  // Compila a condição do switch
    int state = 0;  // 0: antes de todos os cases, 1: antes do default, 2: após o default.

    // Processa cada case ou default no switch
    for (u32 i = 0; i < node->cases.size(); i++)
    {
        CaseStatement *caseStmt = node->cases[i].get();
        int line = caseStmt->op.line;
        if (state == 2) 
        {
            interpreter->Error("Can't have another case or default after the default case.");
        }

        if (state == 1) 
        {
            // Salta para o final do switch após o case anterior
            caseEnds.push_back(emitJump(OpCode::JUMP, line));
            patchJump(previousCaseSkip);
            emitByte(OpCode::POP,line);  // Limpa o valor de comparação
        }

        if (caseStmt->op.type == TokenType::DEFAULT) 
        {
            state = 2;
            defaultJump = -1;
        } else 
        {
            state = 1;
            // Duplica a expressão do switch e a compara com o valor do case
            emitByte(OpCode::DUP, line);
            caseStmt->condition->accept(this);
            emitByte(OpCode::EQUAL, line);
            previousCaseSkip = emitJump(OpCode::JUMP_IF_FALSE, line);
            emitByte(OpCode::POP, line);  // Limpa o valor da comparação
        }

        for (u32 j = 0; j < caseStmt->statements.size(); j++)
             caseStmt->statements[j]->accept(this);
    }

    // Se terminarmos sem um case default, corrige o salto
    if (state == 1) 
    {
        patchJump(previousCaseSkip);
        emitByte(OpCode::POP, node->op.line);
    }

    // Corrige os saltos de todos os cases para o final
    for (int i = 0; i < caseEnds.size(); i++) 
    {
        patchJump(caseEnds[i]);
    }

    // Limpa a condição original do switch
    emitByte(OpCode::POP, node->op.line);
}

 





void ByteGenerator::visit_for(ForStatement *node)
{
    
    int previousLoopStart = current->loopStart;
    int previousBreakJumpCount = current->breakCount;
    current->breakCount = 0;
    current->doContinue.clear();

    emitByte(OpCode::ENTER_SCOPE, node->op.line);

    // 1. Inicializador  (for '*';; )
    if (node->initializer != nullptr)
    {
        node->initializer->accept(this); // initializer expression
        emitByte(OpCode::POP, node->op.line); 
    }

    // 2. // exit condition expression  for (;'*';)
    current->loopStart = count();
    int exitJump = -1;
    if (node->condition != nullptr)
    {
        node->condition->accept(this); 
        exitJump = emitJump(OpCode::JUMP_IF_FALSE, node->op.line);
        emitByte(OpCode::POP, node->op.line); 
    }


    // 3. Incremento (avaliação no final de cada iteração)
    if (node->increment != nullptr)
    {
        int bodyJump = emitJump(OpCode::JUMP, node->op.line);
        int incrementStart = count();
        node->increment->accept(this); // expressão de incremento
        emitByte(OpCode::POP, node->op.line); // Limpa o incremento da pilha
        emitLoop(current->loopStart, node->op.line);
        current->loopStart = incrementStart;
        patchJump(bodyJump); // Salta do início do loop para o corpo do loop
    }



  
    node->body->accept(this); // body of the loop
    emitLoop(current->loopStart, node->op.line);



    if (exitJump != -1)
    {
        patchJump(exitJump);
        emitByte(OpCode::POP, node->op.line); // Limpa a pilha
    }


     patchBreakJumps();


    current->loopStart = previousLoopStart;
    current->breakCount = previousBreakJumpCount;


    emitByte(OpCode::EXIT_SCOPE, node->op.line);
}





void ByteGenerator::visit_label(LabelStatement *node)
{
    int exits = -1;
    if (current->labelExists(node->name.lexeme, &exits))
    {
        interpreter->Error("Label '%s' already exists.", node->name.lexeme.c_str());
        return;
    }
    current->addLabel(node->name.lexeme, count());
}

void ByteGenerator::visit_go_to(GoToStatement *node)
{
    

    String name = node->label.lexeme;


    JumpLabel *label;
    if  (current->getLabel(name, &label))
    {

         emitByte(OpCode::BACK, node->label.line);//jump up
         int address = current->chunk->count;
         int offset = label->position - (address + 2); 
         offset = -offset;
         emitByte((offset >> 8) & 0xff, node->label.line);
         emitByte(offset & 0xff, node->label.line);

    } else 
    {
        emitByte(OpCode::JUMP, node->label.line);// jump down
        int jump = emitPlaceholder(node->label.line);
        current->patchList.push_back({name, jump});
    }
    
}

void ByteGenerator::visit_go_sub(GoSubStatement *node)
{
    
    String name = node->label.lexeme;

    // Verifica se a label já foi declarada
    JumpLabel *label;
    if  (current->getLabel(name, &label))
    {

         emitByte(OpCode::BACK, node->label.line);//jump up
         int address = current->chunk->count;
         int offset = label->position - (address + 2); 
         offset = -offset;
         emitByte((offset >> 8) & 0xff, node->label.line);
         emitByte(offset & 0xff, node->label.line);

    } else 
    {
        emitByte(OpCode::SAVE, node->label.line);// jump down

        emitByte(OpCode::JUMP, node->label.line);// jump down
        int jump = emitPlaceholder(node->label.line);
        current->patchList.push_back({name, jump});
    }
}



void ByteGenerator::visit_break(BreakStatement *node)
{
    if (current->loopStart ==-1)
    {
        interpreter->Error(" 'break' used outside of a loop.");
        return;
    }
     if (current->breakJumpCount == UINT8_MAX)
    {
        interpreter->Error("Too many breaks");
        return;
    }
    int jump = emitJump(OpCode::JUMP, node->op.line);
    current->breakJumps[current->breakCount++] = jump;
}

void ByteGenerator::visit_continue(ContinueStatement *node)
{
    if (current->loopStart==-1)
    {
        interpreter->Error(" 'continue' used outside of a loop.");
        return;
    }

    if (!current->doContinue.empty())
    {
        int jump = emitJump(OpCode::JUMP, node->op.line);
        current->doContinue.push_back(jump);
        return;
    }

   
    emitLoop(current->loopStart, node->op.line);
  
}

void ByteGenerator::visit_return(ReturnStatement *node)
{
   
    node->value->accept(this);

    emitReturn(node->op.line);
}

void ByteGenerator::visit_function(FunctionStatement *node)
{
    INFO("Declare '%s' function",node->name.lexeme.c_str());
    Compiler *prev = current;
    Compiler *compiler = interpreter->newCompiler(node->name.lexeme.c_str(), current);
    compiler->createScope(prev->local);
    current = compiler;
    current->args = std::move(node->names);

    

    node->body->accept(this);


    current = prev;
}

void ByteGenerator::visit_struct(StructStatement *node)
{
}

void ByteGenerator::visit_array(ArrayStatement *node)
{
}

void ByteGenerator::visit_map(MapStatement *node)
{
}

void ByteGenerator::visit_call(Call *node)
{
    INFO("call %s",node->op.lexeme.c_str());

    emitConstant(to_string(node->op.lexeme.c_str()),node->op.line);


    for (u32 i = 0; i < node->arguments.size(); i++)
    {
        node->arguments[i]->accept(this);
    }

    u8 arguments = (u8)node->arguments.size();
    emitBytes(OpCode::CALL_SCRIPT, arguments, node->op.line);
    
}

//
// generate
//



//********************************************************************************** */
// COMPILER
//********************************************************************************** */

ByteGenerator::ByteGenerator(Interpreter *i)
{
    interpreter = i;
    current = i->getCurrent();
    NONE = to_none();
}

ByteGenerator::~ByteGenerator()
{
}

void ByteGenerator::emitByte(u8 byte, int line)
{
    current->write_byte(byte, line);
}

void ByteGenerator::emitBytes(u8 byte1, u8 byte2, int line)
{
    emitByte(byte1, line);
    emitByte(byte2, line);
}

void ByteGenerator::emitReturn(int line)
{
    emitByte(OpCode::RETURN, line);
}

void ByteGenerator::emitConstant(const Value &value, int line)
{
    emitBytes(OpCode::CONST, current->makeConstant(value), line);
}

void ByteGenerator::emitLoop(int loopStart, int line)
{
    emitByte(OpCode::LOOP, line);
    int offset = current->chunk->count - loopStart + 2;
    if (offset > UINT16_MAX)
    {
        interpreter->Error("Loop offset overflow");
        return;
    }
    emitByte((offset >> 8) & 0xff,line);
    emitByte(offset & 0xff,line);
}



void ByteGenerator::emitShort(int op,int value, int line)
{
    emitByte(op, line);
    if (value > UINT16_MAX)
    {
        interpreter->Error("Valor excede o limite de 16 bits");
        return;
    }
    emitByte((value >> 8) & 0xff,line);
    emitByte(value & 0xff,line);
}

u8 ByteGenerator::addConst(Value value)
{
    return current->makeConstant(std::move(value));
}

u8 ByteGenerator::addConstString(const char *str)
{
    return current->makeConstant(std::move(to_string(str)));
}

u8 ByteGenerator::addConstNumber(double number)
{
    return current->makeConstant(std::move(to_number(number)));
}

u8 ByteGenerator::addConstInteger(int number)
{
    return current->makeConstant(std::move(to_integer(number)));
}

u8 ByteGenerator::addConstBoolean(bool b)
{
    return current->makeConstant(std::move(to_boolean(b)));
}

int ByteGenerator::emitPlaceholder(int line)
{
    emitByte(0xff, line);
    emitByte(0xff, line);
    int jumpOffset = current->chunk->count - 2;
    return jumpOffset;
}

int ByteGenerator::emitJump(u8 instruction, int line)
{
    emitByte(instruction, line);
    emitByte(0xff, line);
    emitByte(0xff, line);
    int jumpOffset = current->chunk->count - 2;
    return jumpOffset;

}

void ByteGenerator::patchJump(int offset)
{

    int jump = (int)current->chunk->count - offset - 2;

    // Verificação adicional para evitar overflow
    if (jump < 0 || jump > UINT16_MAX) 
    {
        interpreter->Error("Jump offset out of range: %d (offset: %d, current: %d)\n", jump, offset, current->chunk->count);
        return;
    }
    current->chunk->code[offset] = (jump >> 8) & 0xff;
    current->chunk->code[offset + 1] = jump & 0xff;
   
}

void ByteGenerator::patchAdress(int patchAddress, int labelOffset)
{
    if (patchAddress < 0 || patchAddress > (int)current->chunk->count - 2) 
    {
        interpreter->Error("Target adress %d is out of range", patchAddress);
        return;
    }

    if (labelOffset < 0 || labelOffset > UINT16_MAX) 
    {
        interpreter->Error("Target position %d is out of range", labelOffset);
        return;
    }


    current->chunk->code[patchAddress] = (labelOffset >> 8) & 0xff;  
    current->chunk->code[patchAddress + 1] = labelOffset & 0xff;    
}

u32 ByteGenerator::count() const
{
    return current->chunk->count;
}

u32 ByteGenerator::line() const
{
    return  current->chunk->lines[current->chunk->count - 1];
}


