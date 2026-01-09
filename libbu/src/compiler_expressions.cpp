#include "compiler.hpp"
#include "interpreter.hpp"
#include "value.hpp"
#include "opcode.hpp"

void Compiler::expression()
{
    parsePrecedence(PREC_ASSIGNMENT);
}

// ============================================
// PREFIX FUNCTIONS
// ============================================

void Compiler::lengthExpression(bool canAssign)
{
    consume(TOKEN_LPAREN, "Expect '(' after len");

    expression(); // empilha o valor (array, string, etc.)

    consume(TOKEN_RPAREN, "Expect ')' after expression");

    emitByte(OP_FUNC_LEN);
}

// 1. Para funções tipo: sin(x)
void Compiler::mathUnary(bool canAssign)
{
    TokenType type = previous.type; // Guardamos qual foi (SIN, COS, etc)

    consume(TOKEN_LPAREN, "Expect '('");
    expression();
    consume(TOKEN_RPAREN, "Expect ')'");

    switch (type)
    {
    case TOKEN_SIN:
        emitByte(OP_SIN);
        break;
    case TOKEN_COS:
        emitByte(OP_COS);
        break;
    case TOKEN_TAN:
        emitByte(OP_TAN);
        break;
    case TOKEN_ATAN:
        emitByte(OP_ATAN);
        break;
    case TOKEN_SQRT:
        emitByte(OP_SQRT);
        break;
    case TOKEN_ABS:
        emitByte(OP_ABS);
        break;
    case TOKEN_FLOOR:
        emitByte(OP_FLOOR);
        break;
    case TOKEN_CEIL:
        emitByte(OP_CEIL);
        break;
    case TOKEN_DEG:
        emitByte(OP_DEG);
        break;
    case TOKEN_RAD:
        emitByte(OP_RAD);
        break;
    case TOKEN_LOG:
        emitByte(OP_LOG);
        break;
    case TOKEN_EXP:
        emitByte(OP_EXP);
        break;
    default:
        return; // Erro
    }
}

// 2. Para funções tipo: atan2(y, x) ou pow(base, exp)
void Compiler::mathBinary(bool canAssign)
{
    TokenType type = previous.type;

    consume(TOKEN_LPAREN, "Expect '('");
    expression(); // Arg 1
    consume(TOKEN_COMMA, "Expect ','");
    expression(); // Arg 2
    consume(TOKEN_RPAREN, "Expect ')'");

    switch (type)
    {
    case TOKEN_ATAN2:
        emitByte(OP_ATAN2);
        break;
    case TOKEN_POW:
        emitByte(OP_POW);
        break;
    default:
        return;
    }
}

void Compiler::expressionClock(bool canAssign)
{
    (void)canAssign;

    consume(TOKEN_LPAREN, "Expect '(' after clock");
    consume(TOKEN_RPAREN, "Expect ')' after '('");


    emitByte(OP_CLOCK);
}

void Compiler::number(bool canAssign)
{
    (void)canAssign;
    if (previous.type == TOKEN_INT)
    {
        int value;
        const char *str = previous.lexeme.c_str();

        // Verifica se é hex (0xFF, 0x1A)
        if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
        {
            value = strtol(str, nullptr, 16); // Base 16!
        }
        else
        {
            value = std::atoi(str); // Base 10
        }

        emitConstant(vm_->makeInt(value));
    }
    else
    {
        double value = std::atof(previous.lexeme.c_str());
        emitConstant(vm_->makeDouble(value));
    }
}
void Compiler::string(bool canAssign)
{
    (void)canAssign;
    emitConstant(vm_->makeString(previous.lexeme.c_str()));
}

void Compiler::literal(bool canAssign)
{
    (void)canAssign;
    switch (previous.type)
    {
    case TOKEN_TRUE:
        emitByte(OP_TRUE);
        break;
    case TOKEN_FALSE:
        emitByte(OP_FALSE);
        break;
    case TOKEN_NIL:
        emitByte(OP_NIL);
        break;
    default:
        return;
    }
}

void Compiler::grouping(bool canAssign)
{
    (void)canAssign;
    expression();
    consume(TOKEN_RPAREN, "Expect ')' after expression");
}

void Compiler::unary(bool canAssign)
{
    (void)canAssign;
    TokenType operatorType = previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType)
    {
    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;
    case TOKEN_BANG:
        emitByte(OP_NOT);
        break;
    case TOKEN_TILDE:
        emitByte(OP_BITWISE_NOT);
        break;
    default:
        return;
    }
}
void Compiler::binary(bool canAssign)
{
    (void)canAssign;
    TokenType operatorType = previous.type;
    ParseRule *rule = getRule(operatorType);

    parsePrecedence((Precedence)(rule->prec + 1));

    switch (operatorType)
    {
    case TOKEN_PLUS:
        emitByte(OP_ADD);
        break;
    case TOKEN_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TOKEN_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TOKEN_SLASH:
        emitByte(OP_DIVIDE);
        break;
    case TOKEN_PERCENT:
        emitByte(OP_MODULO);
        break;
    case TOKEN_EQUAL_EQUAL:
        emitByte(OP_EQUAL);
        break;
    case TOKEN_BANG_EQUAL:
        emitByte(OP_EQUAL);
        emitByte(OP_NOT);
        break;

    case TOKEN_LESS:
        emitByte(OP_LESS);
        break;
    case TOKEN_LESS_EQUAL:
        emitByte(OP_GREATER);
        emitByte(OP_NOT);
        break;
    case TOKEN_GREATER:
        emitByte(OP_GREATER);
        break;
    case TOKEN_GREATER_EQUAL:
        emitByte(OP_LESS);
        emitByte(OP_NOT);
        break;
    case TOKEN_PIPE:
        emitByte(OP_BITWISE_OR);
        break;
    case TOKEN_AMPERSAND:
        emitByte(OP_BITWISE_AND);
        break;
    case TOKEN_CARET:
        emitByte(OP_BITWISE_XOR);
        break;
    case TOKEN_LEFT_SHIFT:
        emitByte(OP_SHIFT_LEFT);
        break;
    case TOKEN_RIGHT_SHIFT:
        emitByte(OP_SHIFT_RIGHT);
        break;
    default:
        return;
    }
}

void Compiler::arrayLiteral(bool canAssign)
{
    (void)canAssign;
    // var arr = [1, 2, 3];

    int count = 0;

    // Array vazio?
    if (!check(TOKEN_RBRACKET))
    {
        do
        {
            expression(); // Empilha elemento
            count++;

            if (count > 255)
            {
                error("Cannot have more than 255 array elements on initialize.");
                break;
            }
        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RBRACKET, "Expect ']' after array elements");

    emitBytes(OP_DEFINE_ARRAY, count);
}

void Compiler::mapLiteral(bool canAssign)
{
    (void)canAssign;
    // var m = {name: "Luis", age: 30};

    int count = 0;

    // Map vazio?
    if (!check(TOKEN_RBRACE))
    {
        do
        {
            // KEY
            if (match(TOKEN_IDENTIFIER))
            {
                // Identifier como key: {name: "Luis"}
                Token key = previous;

                // String literal da key
                emitConstant(vm_->makeString(key.lexeme.c_str()));

                consume(TOKEN_COLON, "Expect ':' after map key");

                // VALUE
                expression();
            }
            else if (match(TOKEN_STRING))
            {
                // String key: {"my-key": value}
                Token key = previous;

                emitConstant(vm_->makeString(key.lexeme.c_str()));

                consume(TOKEN_COLON, "Expect ':' after map key");

                // VALUE
                expression();
            }
            else
            {
                error("Expect identifier or string as map key");
                break;
            }

            count++;

            if (count > 255)
            {
                error("Cannot have more than 255 map entries");
                break;
            }

        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_RBRACE, "Expect '}' after map elements");

    emitBytes(OP_DEFINE_MAP, count);
}