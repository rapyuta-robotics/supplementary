#pragma once

#include "Term.h"

namespace autodiff
{

class Constant : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;
    double getValue() const { return _value; }

    virtual bool isConstant() const override { return true; }

  private:
    friend TermHolder;
    Constant(double value, TermHolder* owner);
    double _value;
};

} /* namespace autodiff */
