#pragma once
#include "Term.h"

namespace autodiff
{

class Abs : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    Abs(TermPtr arg, TermHolder* owner);

    TermPtr _arg;
};

} /* namespace autodiff */
