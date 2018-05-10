#pragma once

#include "Term.h"

namespace autodiff
{
class LTConstraint;
class LTEConstraint : public Term
{
  public:
    void setNegation(const LTConstraint* negation) const { _negatedForm = negation; }

    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;
    TermPtr negate() const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    LTEConstraint(TermPtr x, TermPtr y, double steppness, TermHolder* owner);

    TermPtr _left;
    TermPtr _right;
    double _steepness;
    mutable const LTConstraint* _negatedForm;
};

} /* namespace autodiff */
