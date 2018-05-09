#pragma once
#include "Term.h"

namespace autodiff
{

class TermPower : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    TermPower(TermPtr baseTerm, TermPtr exponent, TermHolder* owner);

    TermPtr _base;
    TermPtr _exponent;
};

} /* namespace autodiff */
