
#include "pch.h"

#include "Utils.hpp"
#include "Ast.hpp"
#include "Parser.hpp"
#include "Interpreter.hpp"
#include "Lexer.hpp"


//****************************************************************************************************
//NODE
//****************************************************************************************************

Node::Node()
{
	type = NodeType::NODE_NONE;

}



String Node::ToString()
{
	switch (type)
	{
	case NodeType::NODE_NONE:	
		return "NONE";
	case NodeType::EXLITERAL:
		return "LITERAL";
	case NodeType::EXPNIL:
		return "NIL";
	case NodeType::EXPNUMBER:
		return "NUMBER";
	case NodeType::EXPSTRING:
		return "STRING";
	case NodeType::EXPBOOLEAN:
		return "BOOLEAN";
	case NodeType::EXBINARY:
		return "BINARY";
	case NodeType::EXUNARY:
		return "UNARY";
	case NodeType::EXGROUPING:
		return "GROUPING";
	case NodeType::PROGRAM:
		return "PROGRAM";
	case NodeType::EXSTATEMNT:
		return "EXPRESSION STATEMENT";
	case NodeType::BLOCK:
		return "BLOCK";
	case NodeType::PRINT:
		return "PRINT";
	case NodeType::GOTO:
		return "GOTO";
	case NodeType::GOSUB:
		return "GOSUB";
	case NodeType::LABEL:
		return "LABEL";
	case NodeType::DECLARATION:
		return "DECLARATION";
	case NodeType::VARIABLE:
		return "VARIABLE";
	case NodeType::ASSIGNMENT:
		return "ASSIGNMENT";
	case NodeType::IF_STATEMNT:
		return "IF STATEMENT";
	case NodeType::WHILE_STATEMNT:
		return "WHILE STATEMENT";
	case NodeType::FOR_STATEMNT:
		return "FOR STATEMENT";
	case NodeType::SWICH_STATEMNT:
		return "SWITCH STATEMENT";
	case NodeType::DO_STATEMNT:
		return "DO STATEMENT";
	case NodeType::RETURN_STATEMNT:
		return "RETURN STATEMENT";
	case NodeType::BREAK_STATEMNT:
		return "BREAK STATEMENT";
	case NodeType::CONTINUE_STATEMNT:
		return "CONTINUE STATEMENT";
	case NodeType::FUNCTION_STATEMNT:
		return "FUNCTION STATEMENT";
	case NodeType::CLASS_STATEMNT:
		return "CLASS STATEMENT";
	case NodeType::STRUCT_STATEMNT:
		return "STRUCT STATEMENT";
	case NodeType::ENUM_STATEMNT:
		return "ENUM STATEMENT";
	case NodeType::CALL_STATEMNT:
		return "CALL STATEMENT";
	case NodeType::GET_STATEMNT:
		return "GET STATEMENT";
	case NodeType::SET_STATEMNT:
		return "SET STATEMENT";
	case NodeType::NODE_COUNT:
		return "NODE COUNT";
	default:
		return "UNKNOWN NODE TYPE";
	}
}


//****************************************************************************************************
//VISITOR
//****************************************************************************************************

void Literal::accept(Visitor *v)
{
	v->visit_literal(this);
}

void NumberLiteral::accept(Visitor *v)
{
	v->visit_number(this);
}

void StringLiteral::accept(Visitor *v)
{
	v->visit_string(this);
}

void NilLiteral::accept(Visitor *v)
{
	v->visit_nil(this);
}

void Binary::accept(Visitor *v)
{
	v->visit_binary(this);
}

void Unary::accept(Visitor *v)
{
	v->visit_unary(this);
}

void Grouping::accept(Visitor *v)
{
	v->visit_grouping(this);
}

void Program::accept(Visitor *v)
{
	v->visit_program(this);
}

void ExpresionStatement::accept(Visitor *v)
{
	v->visit_expresion_statemnt(this);
}

void Block::accept(Visitor *v)
{
	v->visit_block(this);
}

void Print::accept(Visitor *v)
{
	v->visit_print(this);
}

void BooleanLiteral::accept(Visitor *v)
{
	v->visit_boolean(this);
}


void Declaration::accept(Visitor *v)
{
	v->visit_declaration(this);
}
void Variable::accept(Visitor *v)
{
	v->visit_variable(this);
}

void Assignment::accept(Visitor *v)
{
	v->visit_assignment(this);
}

void IFStatement::accept(Visitor *v)
{
	v->visit_if(this);
}

void WhileStatement::accept(Visitor *v)
{
	v->visit_while(this);
}

void BreakStatement::accept(Visitor *v)
{
	v->visit_break(this);
}

void ContinueStatement::accept(Visitor *v)
{
	v->visit_continue(this);
}

void ReturnStatement::accept(Visitor *v)
{
	v->visit_return(this);
}

void SwitchStatement::accept(Visitor *v)
{
	v->visit_switch(this);
}

void DoStatement::accept(Visitor *v)
{
	v->visit_do(this);
}

void ForStatement::accept(Visitor *v)
{
	v->visit_for(this);
}

void GoToStatement::accept(Visitor *v)
{
	v->visit_go_to(this);
}

void GoSubStatement::accept(Visitor *v)
{
	v->visit_go_sub(this);
}

void LabelStatement::accept(Visitor *v)
{
	v->visit_label(this);
}



void FunctionStatement::accept(Visitor *v)
{
	v->visit_function(this);
}

void StructStatement::accept(Visitor *v)
{
	v->visit_struct(this);
}

void ArrayStatement::accept(Visitor *v)
{
	v->visit_array(this);
}

void MapStatement::accept(Visitor *v)
{
	v->visit_map(this);
}

void Call::accept(Visitor *v)
{
	v->visit_call(this);
}
