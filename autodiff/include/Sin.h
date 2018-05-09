#pragma once

#include "Term.h"

namespace autodiff
{

class Sin : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    string toString() const override;

  private:
    friend TermHolder;
    Sin(TermPtr arg, TermHolder* owner);

    TermPtr _arg;
};

} /* namespace autodiff */
