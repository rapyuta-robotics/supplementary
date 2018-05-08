#pragma once
#include "Term.h"

namespace autodiff
{

class Exp : public Term
{
  public:
    int accept(ITermVisitor* visitor);

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    Exp(TermPtr arg, TermHolder* owner);
    TermPtr _arg;
};

} /* namespace autodiff */
