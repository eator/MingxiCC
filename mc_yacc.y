%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stack>
#include "../syntax.h"

#define YYSTYPE char*

int yyparse(void);
int yylex();

void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
	return 1;
}

extern FILE *yyin;

stack<Syntax*> syntax_stack;

%}

%token INCLUDE HEADER_NAME
%token TYPE IDENTIFIER RETURN NUMBER
%token OPEN_BRACE CLOSE_BRACE
%token IF WHILE
%token LESS_OR_EQUAL

/* Operator associativity, least precedence first.
 * See http://en.cppreference.com/w/c/language/operator_precedence
 */
%left '='
%left '<'
%left '+'
%left '-'
%left '*'
%nonassoc '!'
%nonassoc '~'

%%

program
    : function program
      {
          Syntax *top_level_syntax;
          if (syntax_stack.empty() || syntax_stack.top()->GetType() != SyntaxType::TOP_LEVEL) {
              top_level_syntax = new TopLevel();
          } else {
              top_level_syntax = syntax_stack.top();
              syntax_stack.pop();
          }

          TopLevel* top_level = dynamic_cast<TopLevel*>(top_level_syntax);
          top_level->declarations.push_front(syntax_stack.top());
          syntax_stack.pop();

          syntax_stack.push(top_level_syntax);
      }
    |
    ;

function
    : TYPE IDENTIFIER '(' parameter_list ')' OPEN_BRACE block CLOSE_BRACE
      {
          Syntax *current_syntax = syntax_stack.top();
          syntax_stack.pop();
          // TODO: assert current_syntax has type BLOCK.
          Function* f = new Function(string($2), current_syntax); 
          syntax_stack.push(f);
      }
    ;

parameter_list
    : nonempty_parameter_list
    |
    ;

nonempty_parameter_list
    : TYPE IDENTIFIER ',' parameter_list
    | TYPE IDENTIFIER
    ;

block
    : statement block
      {
          /* Append to the current block, or start a new block. */
          Syntax *block_syntax;
          if (syntax_stack.empty() || syntax_stack.top()->GetType() != SyntaxType::BLOCK) {
              block_syntax = new Block(list<Syntax*>());
          } else {
              block_syntax = syntax_stack.top();
              syntax_stack.pop();
          }

          Block* block = dynamic_cast<Block*>(block_syntax);
          block->statements.push_front(syntax_stack.top());
          syntax_stack.pop();

          syntax_stack.push(block_syntax);
      }
    |
    ;

argument_list
    : nonempty_argument_list
    | // Empty argument list.
      {
          FunctionArguments*  function_arguments = new FunctionArguments();
          syntax_stack.push(function_arguments);
      }
    ;

nonempty_argument_list
    : expression ',' nonempty_argument_list
      {
          Syntax *arguments_syntax;
          if (syntax_stack.empty()) {
              // This should be impossible, we shouldn't be able to
              // parse this on its own.
              assert(false);
          } else if (syntax_stack.top()->GetType() != SyntaxType::FUNCTION_ARGUMENTS) {
              arguments_syntax = new FunctionArguments();
          } else {
              arguments_syntax = syntax_stack.top();
              syntax_stack.pop();
          }

          FunctionArguments*  arguments = dynamic_cast<FunctionArguments*>(arguments_syntax);
          arguments->arguments.push_front(syntax_stack.top());
          syntax_stack.pop();

          syntax_stack.push(arguments_syntax);
      }

    | expression
      {
          // TODO: find a way to factor out the duplication with the above.
          if (syntax_stack.empty()) {
              // This should be impossible, we shouldn't be able to
              // parse this on its own.
              assert(false);
          }

          FunctionArguments *arguments_syntax = new FunctionArguments();
          arguments_syntax->arguments.push_front(syntax_stack.top());
          syntax_stack.pop();

          syntax_stack.push(arguments_syntax);
      }
    ;

statement
    : RETURN expression ';'
      {
          Syntax *current_syntax = syntax_stack.top();
          syntax_stack.pop();
          ReturnStatement* return_statement = dynamic_cast<ReturnStatement*>(current_syntax);
          syntax_stack.push(return_statement);
      }

    | IF '(' expression ')' OPEN_BRACE block CLOSE_BRACE
      {
          Syntax *then = syntax_stack.top();
          syntax_stack.pop();
          Syntax *condition = syntax_stack.top();
          syntax_stack.pop();
          IfStatement* if_statement = new IfStatement(condition, then);
          syntax_stack.push(if_statement);
          // TODO: else statements.
      }

    | WHILE '(' expression ')' OPEN_BRACE block CLOSE_BRACE
      {
          Syntax *body =  syntax_stack.top();
          syntax_stack.pop();
          Syntax *condition =  syntax_stack.top();
          syntax_stack.pop();
          WhileStatement * while_statement = new WhileStatement(condition, body);
          syntax_stack.push(while_statement);
      }

    | TYPE IDENTIFIER '=' expression ';'
      {
          Syntax *init_value =  syntax_stack.top();
          syntax_stack.pop();
          DefineVarStatement * def_statement = new DefineVarStatement(string($2), init_value); 
          syntax_stack.push(def_statement);
      }

    | expression ';'
      {
          // Nothing to do, we have the AST node already.
      }
    ;

expression
    : NUMBER
      {
          Immediate * imm = new Immediate(atoi( (char*)$1 ));
          syntax_stack.push(imm);
      }

    | IDENTIFIER
      {
          Variable * var = new Variable(string($1));
          syntax_stack.push(var);
      }

    | IDENTIFIER '=' expression
      {
          Syntax *expression = syntax_stack.top();
          syntax_stack.pop();
          Assignment * assignment = new Assignment(string($1), expression);
          syntax_stack.push(assignment);
      }

    | '~' expression
      {
          Syntax *current_syntax =syntax_stack.top(); 
          syntax_stack.pop();
          BitNegation* bitwise_negation = new BitNegation(current_syntax);
          syntax_stack.push(bitwise_negation);
      }

    | '!' expression
      {
          Syntax *current_syntax =syntax_stack.top(); 
          syntax_stack.pop();
          LogicNegation * logic_negation = new LogicNegation(current_syntax);
          syntax_stack.push(logic_negation);
      }

    | expression '+' expression
      {
          Syntax *right = syntax_stack.top(); 
          syntax_stack.pop();
          Syntax *left = syntax_stack.top(); 
          syntax_stack.pop();
          Addition * add = new Addition(left, right);
          syntax_stack.push(add);
      }

    | expression '-' expression
      {
          Syntax *right = syntax_stack.top(); 
          syntax_stack.pop();
          Syntax *left = syntax_stack.top(); 
          syntax_stack.pop();
          Subtraction * sub = new Subtraction(left, right);
          syntax_stack.push(sub);
      }

    | expression '*' expression
      {
          Syntax *right = syntax_stack.top(); 
          syntax_stack.pop();
          Syntax *left = syntax_stack.top(); 
          syntax_stack.pop();
          Multiplication * mul = new Multiplication(left, right);
          syntax_stack.push(mul);
      }

    | expression '<' expression
      {
          Syntax *right = syntax_stack.top(); 
          syntax_stack.pop();
          Syntax *left = syntax_stack.top(); 
          syntax_stack.pop();
           LessThan * lt = new LessThan(left, right);
          syntax_stack.push(lt);
      }

    | expression LESS_OR_EQUAL expression
      {
          Syntax *right = syntax_stack.top(); 
          syntax_stack.pop();
          Syntax *left = syntax_stack.top(); 
          syntax_stack.pop();
          LessThanOrEqual * ltoq = new LessThanOrEqual(left, right);
          syntax_stack.push(ltoq);
      }

    | IDENTIFIER '(' argument_list ')'
      {
          Syntax *arguments = syntax_stack.top();  
          syntax_stack.pop();
          FunctionCall* func_call = new FunctionCall(string($1), arguments); 
          syntax_stack.push(func_call);
      }
    ;
