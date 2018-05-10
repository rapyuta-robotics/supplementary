#pragma once

#include "Term.h"

namespace autodiff
{

class ConstraintUtility : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;
    TermPtr negate() const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    ConstraintUtility(TermPtr constraint, TermPtr utility, TermHolder* owner);

    TermPtr _constraint;
    TermPtr _utility;
};

} /* namespace autodiff */
