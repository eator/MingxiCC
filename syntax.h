#pragma once

#include <list>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

enum class SyntaxType {
    IMMEDIATE,
    VARIABLE,
    UNARY_OPERATOR,
    BINARY_OPERATOR,
    BLOCK,
    FUNCTION,
    FUNCTION_ARGUMENTS,
    FUNCTION_CALL,
    DEFINE_VAR,
    ASSIGNMENT,
    IF_STATEMENT,
    WHILE_SYNTAX,
    RETURN_STATEMENT,
    TOP_LEVEL
};

enum class UnaryExpressionType { BITWISE_NEGATION, LOGICAL_NEGATION };

enum class BinaryExpressionType {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,

    LESS_THAN,
    LESS_THAN_OR_EQUAL,
};

class Syntax {
  protected:
    SyntaxType type;

  public:
    SyntaxType GetType() { return type; }

    void print_indent(int indent) {
        string spaces(indent, ' ');
        cout << spaces;
    }
    virtual void print_syntax_indented(int indent) = 0;
    void print_syntax() { print_syntax_indented(0); }
};

class UnaryExpression : public Syntax {
  public:
    Syntax *expression;
    UnaryExpressionType utype;

  public:
    UnaryExpressionType GetUtype() { return utype; }
    UnaryExpression(Syntax *expression) {
        type = SyntaxType::UNARY_OPERATOR;
        this->expression = expression;
    }
};

class BinaryExpression : public Syntax {
  public:
    Syntax *left;
    Syntax *right;
    BinaryExpressionType btype;

  public:
    BinaryExpression(Syntax *left, Syntax *right) {
        type = SyntaxType::BINARY_OPERATOR;
        this->left = left;
        this->right = right;
    }
};

class Immediate : public Syntax {
  public:
    int value;

  public:
    Immediate(int v) : value(v) { type = SyntaxType::IMMEDIATE; }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "IMMEDIATE " << value << endl;
    }
};

class Variable : public Syntax {
  public:
    string var_name;

  public:
    Variable(const string &vn) : var_name(vn) { type = SyntaxType::VARIABLE; }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "VARIABLE " << var_name << endl;
    }
};

class BitNegation : public UnaryExpression {
  public:
    BitNegation(Syntax *expression) : UnaryExpression(expression) {
        this->utype = UnaryExpressionType::BITWISE_NEGATION;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "UNARY BITWISE_NEGATION" << endl;
        expression->print_syntax_indented(indent + 4);
    }
};

class LogicNegation : public UnaryExpression {
  public:
    LogicNegation(Syntax *expression) : UnaryExpression(expression) {
        this->utype = UnaryExpressionType::LOGICAL_NEGATION;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "UNARY LOGICAL_NEGATION" << endl;
        expression->print_syntax_indented(indent + 4);
    }
};

class Addition : public BinaryExpression {
  public:
    Addition(Syntax *left, Syntax *right) : BinaryExpression(left, right) {
        btype = BinaryExpressionType::ADDITION;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "ADDITION LEFT" << endl;
        left->print_syntax_indented(indent + 4);

        print_indent(indent);
        cout << "ADDITION RIGHT" << endl;
        right->print_syntax_indented(indent + 4);
    }
};

class Subtraction : public BinaryExpression {
  private:
    BinaryExpressionType btype;

  public:
    Subtraction(Syntax *left, Syntax *right) : BinaryExpression(left, right) {
        btype = BinaryExpressionType::SUBTRACTION;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "SUBTRACTION LEFT" << endl;
        left->print_syntax_indented(indent + 4);

        print_indent(indent);
        cout << "SUBTRACTION RIGHT" << endl;
        right->print_syntax_indented(indent + 4);
    }
};

class Multiplication : public BinaryExpression {
  private:
    BinaryExpressionType btype;

  public:
    Multiplication(Syntax *left, Syntax *right)
        : BinaryExpression(left, right) {
        btype = BinaryExpressionType::MULTIPLICATION;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "MULTIPLICATION LEFT" << endl;
        left->print_syntax_indented(indent + 4);

        print_indent(indent);
        cout << "MULTIPLICATION RIGHT" << endl;
        right->print_syntax_indented(indent + 4);
    }
};

class LessThan : public BinaryExpression {
  private:
    BinaryExpressionType btype;

  public:
    LessThan(Syntax *left, Syntax *right) : BinaryExpression(left, right) {
        btype = BinaryExpressionType::LESS_THAN;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "LESS THAN LEFT" << endl;
        left->print_syntax_indented(indent + 4);

        print_indent(indent);
        cout << "LESS THAN RIGHT" << endl;
        right->print_syntax_indented(indent + 4);
    }
};

class LessThanOrEqual : public BinaryExpression {
  private:
    BinaryExpressionType btype;

  public:
    LessThanOrEqual(Syntax *left, Syntax *right)
        : BinaryExpression(left, right) {
        this->btype = BinaryExpressionType::LESS_THAN_OR_EQUAL;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "LESS THAN OR EQUAL LEFT" << endl;
        left->print_syntax_indented(indent + 4);

        print_indent(indent);
        cout << "LESS THAN OR EQUAL RIGHT" << endl;
        right->print_syntax_indented(indent + 4);
    }
};

class Block : public Syntax {
  public:
    list<Syntax *> statements;

  public:
    Block(const list<Syntax *> &statements) {
        type = SyntaxType::BLOCK;
        this->statements = statements;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "BLOCK" << endl;
        for (auto &s : statements) {
            s->print_syntax_indented(indent + 4);
        }
    }
};

class Function : public Syntax {
  public:
    string func_name;
    list<Syntax *> parameters;
    Syntax *root_block;

  public:
    Function(const string &func_name, Syntax *root_block) {
        type = SyntaxType::FUNCTION;
        this->func_name = func_name;
        this->root_block = root_block;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "FUNCTION " << func_name << endl;
        root_block->print_syntax_indented(indent + 4);
    }
};

class FunctionArguments : public Syntax {
  public:
    list<Syntax *> arguments;

  public:
    // TODO
    FunctionArguments() { type = SyntaxType::FUNCTION_ARGUMENTS; }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "FUNCTION ARGUMENTS" << endl;
        for (auto &a : arguments) {
            a->print_syntax_indented(indent + 4);
        }
    }
};

class FunctionCall : public Syntax {
  public:
    string func_name;
    Syntax *function_arguments;

  public:
    FunctionCall(const string &func_name, Syntax *func_args) {
        type = SyntaxType::FUNCTION_CALL;
        this->func_name = func_name;
        function_arguments = func_args;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "FUNCTION CALL" << func_name << endl;
        function_arguments->print_syntax_indented(indent);
    }
};

class DefineVarStatement : public Syntax {
  public:
    string var_name;
    Syntax *init_value;

  public:
    DefineVarStatement(const string &var_name, Syntax *init_value) {
        type = SyntaxType::DEFINE_VAR;
        this->var_name = var_name;
        this->init_value = init_value;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "DEFINE VARIABLE" << var_name << endl;

        print_indent(indent);
        cout << var_name << " INITIAL VALUE" << endl;
        init_value->print_syntax_indented(indent + 4);
    }
};

class Assignment : public Syntax {
  public:
    string var_name;
    Syntax *expression;

  public:
    Assignment(const string &var_name, Syntax *expression) {
        type = SyntaxType::ASSIGNMENT;
        this->var_name = var_name;
        this->expression = expression;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "ASSIGNMENT '" << var_name << "'" << endl;
        expression->print_syntax_indented(indent + 4);
    }
};

class IfStatement : public Syntax {
  public:
    Syntax *condition;
    Syntax *then;

  public:
    IfStatement(Syntax *condition, Syntax *then) {
        type = SyntaxType::IF_STATEMENT;
        this->condition = condition;
        this->then = then;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "IF CONDITION" << endl;
        condition->print_syntax_indented(indent + 4);

        print_indent(indent);
        cout << "IF THEN" << endl;
        then->print_syntax_indented(indent + 4);
    }
};

class WhileStatement : public Syntax {
  public:
    Syntax *condition;
    Syntax *body;

  public:
    WhileStatement(Syntax *condition, Syntax *body) {
        type = SyntaxType::WHILE_SYNTAX;
        this->condition = condition;
        this->body = body;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "WHILE CONDITION" << endl;
        condition->print_syntax_indented(indent + 4);

        print_indent(indent);
        cout << "WHILE BODY" << endl;
        body->print_syntax_indented(indent + 4);
    }
};

class ReturnStatement : public Syntax {
  private:
    Syntax *expression;

  public:
    ReturnStatement(Syntax *expression) {
        type = SyntaxType::RETURN_STATEMENT;
        this->expression = expression;
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "RETURN" << endl;
        expression->print_syntax_indented(indent + 4);
    }
};

class TopLevel : public Syntax {
  public:
    list<Syntax *> declarations;

  public:
    TopLevel() {
        type = SyntaxType::TOP_LEVEL;
        this->declarations = list<Syntax *>();
    }
    void print_syntax_indented(int indent) override {
        print_indent(indent);
        cout << "TOP LEVEL" << endl;
        for (auto &d : declarations) {
            d->print_syntax_indented(indent + 4);
        }
    }
};
