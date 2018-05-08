#pragma once

#include "Term.h"

namespace autodiff
{

class ConstPower : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const overrides;

  private:
    friend TermHolder;
    ConstPower(TermPtr baseTerm, double exponent, TermHolder* owner);

    TermPtr _base;
    double _exponent;
};

} /* namespace autodiff */
