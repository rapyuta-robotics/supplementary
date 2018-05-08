#pragma once
#include "Term.h"

namespace autodiff
{
class LTEConstraint;

class LTConstraint : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;
    TermPtr negate() const override;

    std::string toString() const override;
    void setNegation(LTEConstraint* negation) { _negatedForm = negation; }

  private:
    friend TermHolder;
    LTConstraint(TermPtr x, TermPtr y, double steepness, TermHolder* owner);

    TermPtr _left;
    TermPtr _right;
    double _steepness;
    mutable LTEConstraint* _negatedform;
};

} /* namespace autodiff */
