#pragma once

#include "Config.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Mem.hpp"
#include "Value.hpp"

class Interpreter;
class Lexer;
class Compiler;
class Parser;
class Visitor;

enum class NodeType
{
	NODE_NONE,
	EXLITERAL,
	EXPNIL,
	EXPNUMBER,
	EXPSTRING,
	EXPBOOLEAN,
	EXBINARY,
	EXUNARY,
	EXGROUPING,
	PROGRAM,
	EXSTATEMNT,
	BLOCK,
	PRINT,
	GOTO,
	GOSUB,
	LABEL,

	DECLARATION,
	VARIABLE,
	ASSIGNMENT,

	IF_STATEMNT,
	WHILE_STATEMNT,
	FOR_STATEMNT,
	SWICH_STATEMNT,
	DO_STATEMNT,
	RETURN_STATEMNT,
	BREAK_STATEMNT,
	CONTINUE_STATEMNT,

	FUNCTION_STATEMNT,
	CLASS_STATEMNT,
	STRUCT_STATEMNT,
	ENUM_STATEMNT,

	ARRAY_STATEMNT,
	MAP_STATEMNT,

	CALL_STATEMNT,
	GET_STATEMNT,
	SET_STATEMNT,

	NODE_COUNT
};

struct Node
{
	NodeType type;

	Node();
	virtual ~Node() {}

	


	virtual String ToString();
	virtual void accept(Visitor* v) = 0;
};

using NodePtr = SharedPtr<Node>;

struct Literal : Node
{
		Token op;
	Literal() { type = NodeType::EXLITERAL; }
	void accept(Visitor* v) override;
};

struct NilLiteral : Literal
{
	Token op;
	NilLiteral() { type = NodeType::EXPNIL;}
	void accept(Visitor* v) override;
};

struct NumberLiteral : Literal
{
	Token op;
	NumberLiteral() { type = NodeType::EXPNUMBER; value = 0; }
	void accept(Visitor* v) override;
	double value{0};
};

struct StringLiteral : Literal
{
	Token op;
	StringLiteral() { type = NodeType::EXPSTRING; value = ""; }
	void accept(Visitor* v) override;
	Chars value{""};
};

struct BooleanLiteral : Literal
{
	Token op;
	BooleanLiteral():Literal() { type = NodeType::EXPBOOLEAN; value = false; }
	void accept(Visitor* v) override;
	bool value{false};
};


struct Binary : Node
{
	
	Token operation;
	NodePtr left{ nullptr };
	NodePtr right{ nullptr };
	Binary():Node() { type = NodeType::EXBINARY; }
	 void accept(Visitor* v) override;
};

struct Unary : Node
{

	Token operation;
	NodePtr right{ nullptr };;
	Unary():Node() { type = NodeType::EXUNARY; }
	 void accept(Visitor* v) override;
};

struct Grouping : Node
{
	Token op;
	NodePtr expression{ nullptr };
	Grouping():Node() { type = NodeType::EXGROUPING; }
	 void accept(Visitor* v) override;
};

//statemns

struct Program : Node 
{
	Token op;
	Vector<NodePtr> statements;

	Program() {type = NodeType::PROGRAM;}

	 void accept(Visitor* v) override;
};

struct ExpresionStatement : Node
{
	Token op;
	NodePtr expression;

	ExpresionStatement() { type = NodeType::EXSTATEMNT; }

	 void accept(Visitor* v) override;
};

struct Block : Node
{
	Token op;
	Vector<NodePtr> statements;
	Block() { type = NodeType::BLOCK; }
	virtual void accept(Visitor* v) override;
};

struct Print: Node 
{
	Token op;
	NodePtr  value;

	Print() { type = NodeType::PRINT; }

	 void accept(Visitor* v) override;
};


struct Declaration : Node 
{
	Token name;
	NodePtr initializer;
	Declaration() { type = NodeType::DECLARATION; }
	 void accept(Visitor* v) override;
};

struct Variable: Node
{
	Token name;
	Variable() { type = NodeType::VARIABLE; }
	void accept(Visitor* v) override;
};

struct Assignment: Node
{
	Token name;
	NodePtr value;
	Assignment() { type = NodeType::ASSIGNMENT; }
	 void accept(Visitor* v) override;
};

struct ElifStmt
{
		NodePtr condition;
		NodePtr body;
		Token op;
};

struct IFStatement : Node
{
			Token op;
			NodePtr condition;
			NodePtr thenBranch;
			NodePtr elseBranch;
			Vector<UniquePtr<ElifStmt>> elifBranch;
			IFStatement() { type = NodeType::IF_STATEMNT; }
			 void accept(Visitor *v) override;
};

struct WhileStatement :Node 
{
	Token op;
	NodePtr condition;
	NodePtr body;
	WhileStatement() { type = NodeType::WHILE_STATEMNT; }
	 void accept(Visitor *v) override;

};

struct DoStatement :Node 
{
	Token op;
	NodePtr condition;
	NodePtr body;
	DoStatement() { type = NodeType::DO_STATEMNT; }
	void accept(Visitor *v) override;
};


struct ForStatement : Node
{
	Token op;
	NodePtr initializer;
	NodePtr condition;
	NodePtr increment;
	NodePtr body;
	ForStatement() { type = NodeType::FOR_STATEMNT; }
	void accept(Visitor *v) override;
};

struct GoToStatement : Node
{
	Token name;
	Token label;
	GoToStatement() { type = NodeType::GOTO; }
	void accept(Visitor *v) override;
};

struct GoSubStatement : Node
{
	Token name;
	Token label;
	GoSubStatement() { type = NodeType::GOSUB;}
	void accept(Visitor *v) override;
};



struct LabelStatement : Node
{
	Token name;
	LabelStatement() { type = NodeType::LABEL;}
	void accept(Visitor *v) override;
};

struct CaseStatement
{
	Token op;
	NodePtr condition;
	Vector<NodePtr> statements;
};

struct SwitchStatement :Node
{
	Token op;
	NodePtr condition;
	Vector<UniquePtr<CaseStatement>> cases;
	SwitchStatement() { type = NodeType::SWICH_STATEMNT; }
	void accept(Visitor *v) override;
};

struct BreakStatement :Node 
{
		Token op;
	BreakStatement() { type = NodeType::BREAK_STATEMNT; }
	 void accept(Visitor *v) override;
};

struct ContinueStatement : Node 
{
		Token op;
	ContinueStatement() { type = NodeType::CONTINUE_STATEMNT; }
	void accept(Visitor *v) override;
};

struct ReturnStatement :Node 
{
	Token op;
	NodePtr value;
	ReturnStatement() {type = NodeType::RETURN_STATEMNT; }
	void accept(Visitor *v) override;
};

struct FunctionStatement :Node 
{
	Token name;
	Vector<Chars> names;
	NodePtr body;
	FunctionStatement() {type = NodeType::FUNCTION_STATEMNT; }
	void accept(Visitor *v) override;
};

struct StructStatement :Node 
{
	Token name;
	Vector<NodePtr> attributes;
	StructStatement() {type = NodeType::STRUCT_STATEMNT; }
	void accept(Visitor *v) override;
};

struct ArrayStatement :Node 
{
	Token name;
	Vector<NodePtr> attributes;
	ArrayStatement() {type = NodeType::ARRAY_STATEMNT; }
	void accept(Visitor *v) override;
};

struct MapStatement :Node 
{
	Token name;
	Vector<NodePtr> attributes;
	MapStatement() {type = NodeType::MAP_STATEMNT; }
	void accept(Visitor *v) override;
};

struct Call : Node 
{
	Token op;

	Vector<NodePtr> arguments;
	
	Call() { type = NodeType::CALL_STATEMNT; }
	void accept(Visitor *v) override;
};



class Visitor
{
	public:
		Visitor() = default;
		virtual ~Visitor()=default;
		virtual void visit(Node* node) = 0;
		virtual void visit_literal(Literal* node) = 0;
		virtual void visit_number(NumberLiteral* node) = 0;
		virtual void visit_string(StringLiteral* node) = 0;
		virtual void visit_boolean(BooleanLiteral* node) = 0;
		virtual void visit_nil(NilLiteral* node) = 0;

		virtual void visit_binary(Binary* node) = 0;
		virtual void visit_unary(Unary* node) = 0;
		virtual void visit_grouping(Grouping* node) = 0;

		virtual void visit_program(Program* node) = 0;
		virtual void visit_expresion_statemnt(ExpresionStatement* node) = 0;
		virtual void visit_block(Block* node) = 0;
		virtual void visit_print(Print* node) = 0;

		virtual void visit_declaration(Declaration* node) = 0;
		virtual void visit_variable(Variable* node) = 0;
		virtual void visit_assignment(Assignment* node) = 0;

		virtual void visit_if(IFStatement *node) = 0;
		virtual void visit_while(WhileStatement *node) = 0;
		virtual void visit_do(DoStatement *node) = 0;
		virtual void visit_switch(SwitchStatement *node) = 0;
		virtual void visit_for(ForStatement *node) = 0;

		virtual void visit_label(LabelStatement *node) = 0;
		virtual void visit_go_to(GoToStatement *node) = 0;
		virtual void visit_go_sub(GoSubStatement *node) = 0;

		
		

		virtual void visit_break(BreakStatement *node) = 0;
		virtual void visit_continue(ContinueStatement *node) = 0;
		virtual void visit_return(ReturnStatement *node) = 0;

		virtual void visit_function(FunctionStatement *node)=0;
		virtual void visit_struct(StructStatement *node) = 0;

		virtual void visit_array(ArrayStatement *node)=0;
		virtual void visit_map(MapStatement *node) = 0;

		virtual void visit_call(Call *node) = 0;
};


class ByteGenerator: Visitor
{

	public:
		ByteGenerator(Interpreter *i);
		~ByteGenerator();

		void emitByte(u8 byte, int line);
		void emitBytes(u8 byte1, u8 byte2, int line);
		void emitReturn(int line);
		void emitConstant(const Value &value, int line);
		void emitLoop(int loopStart,int line);
		void emitShort(int op, int value, int line);
		u8   addConst(Value value);
		u8   addConstString(const char *str);
		u8   addConstNumber(double number);
		u8   addConstInteger(int number);
		u8   addConstBoolean(bool b);

		int  emitPlaceholder(int line);
		int  emitJump(u8 instruction, int line);
		int  jumpSize(u8 instruction,int line);

		void patchJump(int offset);
		void patchAdress(int address, int position);

		u32 count() const;
		u32 line() const;


	private:
		friend class Parser;
		friend class Compiler;

		Interpreter* interpreter{ nullptr };
		Compiler* current{ nullptr };

		Value NONE;

		void patchBreakJumps();

		void visit(Node* node) override;
		void visit_literal(Literal* node) override;
		void visit_number(NumberLiteral* node) override;
		void visit_string(StringLiteral* node) override;
		void visit_boolean(BooleanLiteral* node) override;
		void visit_nil(NilLiteral* node) override;

		void visit_binary(Binary* node) override;
		void visit_unary(Unary* node) override;
		void visit_grouping(Grouping* node) override;



		void visit_program(Program* node) override;
		void visit_expresion_statemnt(ExpresionStatement* node) override; 
		void visit_block(Block* node) override;
		void visit_print(Print* node) override;

		void visit_declaration(Declaration *node);
		void visit_variable(Variable* node) ;
		void visit_assignment(Assignment* node) ;

		void visit_if(IFStatement *node) ;
		void visit_while(WhileStatement *node) ;
		void visit_do(DoStatement *node) ;
		void visit_switch(SwitchStatement *node);
		void visit_for(ForStatement *node) ;

		void visit_label(LabelStatement *node) ;
		void visit_go_to(GoToStatement *node) ;
		void visit_go_sub(GoSubStatement *node) ;
		
		void visit_break(BreakStatement *node);
		void visit_continue(ContinueStatement *node);
		void visit_return(ReturnStatement *node);

		void visit_function(FunctionStatement *node);
		void visit_struct(StructStatement *node);

		
		void visit_array(ArrayStatement *node);
		void visit_map(MapStatement *node);

		void visit_call(Call *node);
};