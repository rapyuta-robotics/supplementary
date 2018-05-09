#pragma once

#include "Term.h"

namespace autodiff
{

class Or : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr* v) const override;
    TermPtr negate() const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    Or(TermPtr left, TermPtr right, TermHolder* owner);

    TermPtr _left;
    TermPtr _right;
};

} /* namespace autodiff */
