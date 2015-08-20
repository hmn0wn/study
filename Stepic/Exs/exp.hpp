#ifndef EXP_HPP
#define EXP_HPP
#include <cassert> // assert
#include <cstring>
#include <math.h>
#include <string>

struct Transformer;
struct Number;
struct BinaryOperation;
struct FunctionCall;
struct Variable;

struct Scope
{
  virtual ~Scope() {}
  virtual double variableValue(std::string const &name) const = 0;
};

struct SimpleScope : Scope
{
  double variableValue(const std::string &name) const
  {
    if(name=="var1") return 2.5;
        else return 4;
  }
};

struct Expression
{
  virtual ~Expression() { }
  virtual double evaluate(Scope *) const = 0;
  virtual double evaluate() const = 0;
  virtual Expression *transform(Transformer *) const = 0;
};

struct Transformer
{
  virtual ~Transformer() {};
  virtual Expression *transformNumber(Number const *) = 0;
  virtual Expression *transformBinaryOperation(BinaryOperation const *) = 0;
  virtual Expression *transformFunctionCall(FunctionCall const *) = 0;
  virtual Expression *transformVariable(Variable const *) = 0;

};

struct Number : Expression
{
    Number(double value) : value_(value) {}
    double value() const { return value_; }
    double evaluate() const { return value_; }
    double evaluate(Scope *sc) const { return value_; }
    Expression *transform(Transformer *tr) const
    {
      return tr->transformNumber(this);
    }

private:
    double value_;
};

struct BinaryOperation : Expression
{
    enum
    {
        PLUS = '+',
        MINUS = '-',
        DIV = '/',
        MUL = '*'
    };

    BinaryOperation(Expression const *left, int op, Expression const *right)
        : left_(left), op_(op), right_(right)
    { assert(left_ && right_); }

    ~BinaryOperation()
    {
        delete left_;
        delete right_;
    }
    Expression const *left() const { return left_; }
    Expression const *right() const { return right_; }
    int operation() const { return op_; }
    double evaluate(Scope *sc) const
    {
        double left = left_->evaluate(sc);
        double right = right_->evaluate(sc);
        switch (op_)
        {
        case PLUS: return left + right;
        case MINUS: return left - right;
        case DIV: return left / right;
        case MUL: return left * right;
        }
        assert(0);
        return 0.0;
    }
    double evaluate() const
    {
        double left = left_->evaluate();
        double right = right_->evaluate();
        switch (op_)
        {
        case PLUS: return left + right;
        case MINUS: return left - right;
        case DIV: return left / right;
        case MUL: return left * right;
        }
        assert(0);
        return 0.0;
    }
    Expression *transform(Transformer *tr) const
    {
      return tr->transformBinaryOperation(this);
    }

private:
    Expression const *left_;
    Expression const *right_;
    int op_;
};

struct FunctionCall : Expression
{
  FunctionCall(std::string const name, Expression const *arg):name_(name), arg_(arg)
  {
    assert(arg_);
    assert(name_ == "sqrt" || name_ == "abs");
  }
  ~FunctionCall()
  {
    delete arg_;
  }
  std::string const &name() const
  {
    return name_;
  }
  Expression const *arg() const
  {
    return arg_;
  }
  double evaluate(Scope *sc) const
  {
    double arg = arg_->evaluate(sc);
    if (name_ == "sqrt") return sqrt(arg);
    if(name_ == "abs" ) return abs(arg);

    assert(0);
    return 0.0;
  }
  double evaluate() const
  {
    double arg = arg_->evaluate();
    if (name_ == "sqrt") return sqrt(arg);
    if(name_ == "abs" ) return abs(arg);

    assert(0);
    return 0.0;
  }
  Expression *transform(Transformer *tr) const
  {
    return tr->transformFunctionCall(this);
  }

  private:
    std::string const name_;
    Expression const *arg_;

};

struct Variable : Expression
{
  Variable(std::string const name) : name_(name) {}
  std::string const &name() const { return name_; }
  double evaluate(Scope *sc) const
  {
    return sc->variableValue(this->name());
  }
  double evaluate() const
  {
    assert(0);
    return 0.0;
  }
  Expression *transform(Transformer *tr) const
  {
    return tr->transformVariable(this);
  }

private:
  std::string const name_;
};

struct CopySyntaxTree : Transformer
{
  Expression *transformNumber(Number const *number)
  {
    Expression *exp = new Number(number->value());
    return exp;
  }
  Expression *transformBinaryOperation(BinaryOperation const*binop)
  {
    Expression const *new_left = binop->left()->transform(this);
    Expression const *new_right = binop->right()->transform(this);
    Expression *exp = new BinaryOperation(new_left, binop->operation(), new_right);
    return exp;
  }
  Expression *transformFunctionCall(const FunctionCall *fcall)
  {
    Expression const *new_arg = fcall->arg()->transform(this);
    Expression *exp = new FunctionCall(fcall->name(), new_arg);
    return exp;
  }
  Expression *transformVariable(Variable const *var)
  {
    Expression *exp = new Variable(var->name());
    return exp;
  }
};

struct FoldConstants : Transformer
{
  Expression *transformNumber(const Number *number)
  {
    Expression *exp = new Number(number->value());
    return exp;
  }
  Expression *transformBinaryOperation(const BinaryOperation *binop)
  {
    Expression const *new_left = binop->left()->transform(this);
    Expression const *new_right = binop->right()->transform(this);
    Number const* dcst_left = dynamic_cast<Number const*>(new_left);
    Number const* dcst_right = dynamic_cast<Number const*>(new_right);
    if(dcst_left && dcst_right)
      {
        Expression *exp = new Number(binop->evaluate());
        delete dcst_left;
        delete dcst_right;
        return exp;
      }
    else
      {
        Expression *exp = new BinaryOperation(new_left, binop->operation(), new_right);
        return exp;
      }
    assert(0);
    return NULL;
  }
  Expression *transformFunctionCall(const FunctionCall *fcall)
  {
    Expression const *new_arg = fcall->arg()->transform(this);
    Number const*dcst_arg = dynamic_cast<Number const *>(new_arg);
    if(dcst_arg)
      {
        Expression *exp = new Number(fcall->evaluate());
        delete dcst_arg;
        return exp;
      }
    else
      {
        Expression *exp = new FunctionCall(fcall->name(), new_arg);
        return exp;
      }
    assert(0);
    return NULL;
  }
  Expression *transformVariable(const Variable *var)
  {
    Expression *exp = new Variable(var->name());
    return exp;
  }
};

void test_of_exp()
{
  Expression *exp = new BinaryOperation(new Number(12),'+', new Number(23));
  std::cout << exp->evaluate() << std::endl;

  Transformer *copy_tr = new CopySyntaxTree();
  Expression *new_exp = exp->transform(copy_tr);
  std::cout << new_exp->evaluate() << std::endl;

  Transformer *cofol_tr = new FoldConstants();
  delete new_exp;
  new_exp = exp->transform(cofol_tr);
  std::cout << new_exp->evaluate() << std::endl;

  Expression *long_exp = new BinaryOperation(new BinaryOperation(new Number(3),'+', new Variable("var1")),'*',new FunctionCall("sqrt", new Variable("var2")));
  Scope *sc = new SimpleScope();
  std::cout << long_exp->evaluate(sc) << std::endl;

  Expression *exp_num = new Number(15);
  std::cout << exp_num->evaluate() << std::endl;
}

#endif // EXP_HPP

