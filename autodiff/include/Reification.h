#pragma once
#include "Term.h"

#include <iostream>

namespace autodiff
{

class Reification : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    Reification(TermPtr condition, double min, double max, TermHolder* owner);

    TermPtr condition;
    TermPtr negatedCondition;
    double min;
    double max;
};

} /* namespace autodiff */
