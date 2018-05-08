#pragma once

#include "Term.h"

namespace autodiff
{

class ConstraintUtility : public Term
{
  public:
    int accept(shared_ptr<ITermVisitor> visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(shared_ptr<Variable> v) const override;
    TermPtr negate() const override;

    std::string toString() override;

  private:
    friend TermHolder;
    ConstraintUtility(TermPtr constraint, TermPtr utility, TermHolder* owner);

    TermPtr _constraint;
    TermPtr _utility;
};

} /* namespace autodiff */
