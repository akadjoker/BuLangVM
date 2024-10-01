
#include "pch.h"


#include "Lexer.hpp"
#include "Utils.hpp"



Lexer::Lexer()
{
	input = "";
	start = 0;
	current = 0;
	line = 1;
	panicMode = false;
}

bool Lexer::Load(const String& input)
{
	if (input.length() == 0) return false;
	this->input = input;
	start = 0;
	current = 0;
	line = 1;
	panicMode = false;
	return true;
}

void Lexer::Process()
{
	char c = advance();
	switch (c)
	{
	case '(':
	{
		parens++;
		addToken(TokenType::LEFT_PAREN);
		break;
	}
	case ')':
	{
		parens--;
		addToken(TokenType::RIGHT_PAREN);
		break;
	}
	case '{':
	{
		braces++;
		addToken(TokenType::LEFT_BRACE);
		break;
	}
	case '}':
	{
		braces--;
		addToken(TokenType::RIGHT_BRACE);
		break;
	}
	case '[':
	{
		brackets++;
		addToken(TokenType::LEFT_BRACKET);
		break;
	}
	case ']':
	{
		brackets--;
		addToken(TokenType::RIGHT_BRACKET);
		break;
	}

	case ',':
		addToken(TokenType::COMMA);
		break;
	case '.':
		addToken(TokenType::DOT);
		break;
	case '-':
	{
		if (match('-'))
			addToken(TokenType::DEC);
		else if (match('='))
			addToken(TokenType::MINUS_EQUAL);
		else
			addToken(TokenType::MINUS);
		break;
	}
	case '+':
	{
		if (match('+'))
			addToken(TokenType::INC);
		else if (match('='))
			addToken(TokenType::PLUS_EQUAL);
		else
			addToken(TokenType::PLUS);

		break;
	}
	case ';':
		addToken(TokenType::SEMICOLON);
		break;
	case ':':
		addToken(TokenType::COLON);
		break;



	case '%':
		addToken(TokenType::MOD);
		break;

	case '*':
	{
		if (match('='))
			addToken(TokenType::STAR_EQUAL);
		else
			addToken(TokenType::STAR);
		break;
	}
	case '|':
	{
		if (match('|'))
			addToken(TokenType::OR);
		break;
	}
	case '&':
	{
		if (match('&'))
			addToken(TokenType::AND);
		break;
	}

	case '!':
		addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
		break;
	case '=':
		addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
		break;
	case '<':
		addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
		break;
	case '>':
		addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
		break;

	case '/':
		if (match('='))
		{
			addToken(TokenType::SLASH_EQUAL);
		}
		else if (match('/'))
		{
			while (peek() != '\n' && !isAtEnd())
				advance();
		}
		else if (match('*'))
		{
			advance();
			while (!isAtEnd())
			{
				if (peek() == '*')
					break;
				if (peek() == '\n')
					line++;
				advance();
			}
			if (peek() == '*' && peekNext() == '/')
			{
				advance();
				advance();
				break;
			}
			else
			{
				Error("Unterminated comment");
				return;
			}
		}
		else
		{
			addToken(TokenType::SLASH);
		}
		break;

	case ' ':
	case '\r':
	case '\t':
		//Ignore whitespace.
		break;

	case '\n':
		line++;
		break;
	case '"':
		string();
		break;
	case '\'':
		singleQuoteString();
		break;

	default:
		if (isDigit(c))
		{
			number();
		}
		else if (isAlpha(c))
		{
			identifier();
		}
		else
		{
			Error("Unexpected character");
		}

		break;
	}
}



bool Lexer::Ready()
{
	if (input.length() == 0)
	{
		Log(2, "Input is empty");
		return false;
	}

	if (brackets != 0)
	{
		Log(2, "Brackets miss matched");
		return false;
	}
	if (braces != 0)
	{
		Log(2, "Braces miss matched");
		return false;
	}
	if (parens != 0)
	{
		Log(2, "Parens miss matched");
		return false;
	}

	for (size_t i = 0; i < tokens.size(); i++)
	{

		Token tcurrent = tokens[i];
		if (tcurrent.type == TokenType::IDENTIFIER)
		{

			for (size_t j = 0; j < tokens.size(); j++)
			{
				const Token tfind = tokens[j];
				if (tfind.type == TokenType::IDFUNCTION && tcurrent.lexeme == tfind.lexeme)
				{
					tcurrent.type = TokenType::IDFUNCTION;
					tokens[i] = std::move(tcurrent);
					break;
				}
				else if (tfind.type == TokenType::IDNATIVE && tcurrent.lexeme == tfind.lexeme)
				{
					tcurrent.type = TokenType::IDFUNCTION;
					tokens[i] = std::move(tcurrent);
					break;
				}
			}

		}



		// if (i>=1)
		// { 
		//   const Token pref = tokens[i-1];
		//   Token curr = tokens[i];
		//   if (pref.type == TokenType::FUNCTION && curr.type == TokenType::IDENTIFIER)
		//   {
		//     curr.type = TokenType::IDFUNCTION;
		//     tokens[i, std::move(curr);
		//   }
		// }
	}

	return !panicMode;
}

void Lexer::Clear()
{

	input = "";
	start = 0;
	current = 0;
	line = 1;
	tokens.clear();
	brackets = 0;
	braces = 0;
	parens = 0;
	natives.clear();
	panicMode = false;

}

void Lexer::AddNative(const String& name)
{
	int count = static_cast<int>(natives.size());
	int v = 0;
	if (!natives.find(name, v))
	{
		natives.insert(name, count);
	}
	else
	{
		Log(1, "Native %s already exists", name.c_str());
	}
}

Vector<Token> Lexer::GetTokens()
{
	if (panicMode)
	{
		tokens.clear();
		String error = input.substr(start, current - start);
		auto token = Token(TokenType::ERROR, "Error", error, line);
		tokens.push_back(token);
		return tokens;
	}
	while (!isAtEnd())
	{
		start = current;
		Process();
		if (panicMode)
		{
			tokens.clear();
			String error = input.substr(start, current - start);
			auto token = Token(TokenType::ERROR, "Error", error, line);
			tokens.push_back(token);
			return tokens;
		}
	}
	if (!Ready())
	{
		tokens.clear();
		auto token = Token(TokenType::ERROR, "Error", "Lexer error", 0);
		tokens.push_back(token);
		return tokens;
	}
	auto token = Token(TokenType::END_OF_FILE, "EOF", "", line);
	tokens.push_back(token);
	return tokens;
}

char Lexer::peek()
{
	if (isAtEnd())
		return '\0';
	return input[current];
}

char Lexer::advance()
{
	return input[current++];
}

bool Lexer::match(char expected)
{
	if (isAtEnd())
		return false;
	if (input[current] != expected)
		return false;

	current++;
	return true;
}

char Lexer::peekNext()
{
	if (current + 1 >= (int)input.length())
		return '\0';
	return input[current + 1];
}

char Lexer::peekAhead(int n)
{
	if (current + n >= (int)input.length())
		return '\0';
	return input[current + n];
}

char Lexer::previous()
{
	if (current - 1 < 0)
		return '\0';
	return input[current - 1];
}

Token Lexer::back()
{
	if (tokens.size() == 0)
		return Token();
	return tokens[tokens.size() - 1];
}

bool Lexer::isAtEnd()
{
	return current >= (int)input.length();
}

bool Lexer::isDigit(char c)
{
	return (c >= '0' && c <= '9');
}

bool Lexer::isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

bool Lexer::isAlphaNumeric(char c)
{
	return isAlpha(c) || isDigit(c);
}

String Lexer::extractIdentifier(const String& str)
{
	String result;
	for (u32 i = 0; i < str.length(); i++)
	{
		char c = str[i];
		if (isAlpha(c) || c == '_')
		{
			result += c;
		}
	}
	return result;
}





void Lexer::Initialize(Interpreter *interpreter)
{
	 this->interpreter = interpreter;



	keywords.insert("and", TokenType::AND);

	keywords.insert("or", TokenType::OR);

	keywords.insert("xor", TokenType::XOR);

	keywords.insert("if", TokenType::IF);

	keywords.insert("else", TokenType::ELSE);

	keywords.insert("elif", TokenType::ELIF);


	keywords.insert("goto", TokenType::GOTO);
	//keywords.insert("gosub", TokenType::GOSUB);


	keywords.insert("switch", TokenType::SWITCH);

	keywords.insert("case", TokenType::CASE);

	keywords.insert("default", TokenType::DEFAULT);




	keywords.insert("for", TokenType::FOR);

	keywords.insert("from", TokenType::FROM);

	keywords.insert("while", TokenType::WHILE);
	keywords.insert("loop", TokenType::LOOP);

	keywords.insert("do", TokenType::DO);

	keywords.insert("return", TokenType::RETURN);

	keywords.insert("break", TokenType::BREAK);

	keywords.insert("continue", TokenType::CONTINUE);

	keywords.insert("def", TokenType::FUNCTION);

	keywords.insert("class", TokenType::CLASS);

	keywords.insert("struct", TokenType::STRUCT);

	keywords.insert("import", TokenType::IMPORT);

	// keywords["self", TokenType::SELF;

	 //keywords["super", TokenType::SUPER;

	keywords.insert("nil", TokenType::NIL);
	keywords.insert("var", TokenType::VAR);
	keywords.insert("false", TokenType::FALSE);
	keywords.insert("true", TokenType::TRUE);

	keywords.insert("print", TokenType::PRINT);
	keywords.insert("now", TokenType::NOW);
}

void Lexer::identifier()
{

	while (isAlphaNumeric(peek()))
		advance();

	// std::cout<<" ,"<<peek()<<" ";

	String text = input.substr(start, current - start);
	TokenType value;
	if (keywords.find(text, value))
	{
		addToken(value);
		return;
	}



	const Token pref = back();
	if (pref.type == TokenType::FUNCTION)
	{
		addToken(TokenType::IDFUNCTION, text);
		return;
	}
	if (pref.type == TokenType::CLASS)
	{
		addToken(TokenType::IDCLASS, text);
		return;
	}
	if (pref.type == TokenType::STRUCT)
	{
		addToken(TokenType::IDSTRUCT, text);
		return;
	}
	if (peek() == ':')
	{
		addToken(TokenType::IDLABEL, text);
		return;
	}
	if (natives.contains(text))
	{
		addToken(TokenType::IDNATIVE, text);
		return;
	}

	//INFO("ID: %s", text.c_str());
	addToken(TokenType::IDENTIFIER, text);

}

void Lexer::singleQuoteString()
{
	String value;

	while (!isAtEnd())
	{

		if (peek() == '\'' && peekNext() != '\'')
		{
			break;
		}



		if (peek() == '\n')
		{
			line++;
		}



		if (match('\\'))
		{
			char escaped = advance();
			switch (escaped) {
			case 'n': value += '\n'; break;  
			case 't': value += '\t'; break;  
			case 'r': value += '\r'; break;  
			case '\\': value += '\\'; break; 
			case '\'': value += '\''; break; 
			default:
				Error("Unknown escape sequence");
				return;
			}
		}
		else
		{

			value += advance();
		}
	}


	if (isAtEnd())
	{
		Error("Unterminated string");
		return;
	}


	advance();
	addToken(TokenType::STRING, value);
}


void Lexer::string()
{


	String value;


	while (peek() != '"' && !isAtEnd())
	{
		if (peek() == '\n')
		{
			line++;
		}

		if (match('\\'))
		{

			char escaped = advance();
			switch (escaped) {
			case 'n': value += '\n'; break;
			case 't': value += '\t'; break;
			case 'r': value += '\r'; break;
			case '\\': value += '\\'; break;
			case '"': value += '"'; break;
			default:
				Error("Unknown escape sequence");
				return;
			}
		}
		else
		{
			value += advance();
		}
	}

	if (isAtEnd())
	{
		Error("Unterminated string");
		return;
	}

	advance();


	addToken(TokenType::STRING, value);



	// while (peek() != '"' && !isAtEnd())
	// {
	//   if (peek() == '\n')
	//     line++;
	//   advance();
	// }

	// if (isAtEnd())
	// {
	//   Error("Unterminated string");
	//   return;
	// }

	// advance();

	//  size_t length = (current - start) - 2;
	//  String text = input.substr(start + 1, length);
	//  addToken(TokenType::STRING, text);
}

void Lexer::number()
{
	String text = "";
	bool hasDot = false;

	while (isDigit(peek()))
		advance();

	if (peek() == '.' && isDigit(peekNext()))
	{
		hasDot = true;
		advance();
		while (isDigit(peek()))
			advance();
	}

	text = input.substr(start, current - start);
	addToken(TokenType::NUMBER, text);
}

void Lexer::addToken(TokenType type, const String& literal)
{
	String text = input.substr(start, current - start);
	Token token = Token(type, text, literal, line);
	tokens.push_back(token);
}

void Lexer::Error(const String& message)
{

	String text = "Lexer: " + message + " at line: " + String(line);
	Log(2, text.c_str());
	panicMode = true;
}

void Lexer::Warning(const String& message)
{

	String text = "Lexer: " + message + " at line: " + String(line);
	Log(1, text.c_str());
}
