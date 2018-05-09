#pragma once

#include "Term.h"

namespace autodiff
{

class Sum : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    Sum(TermPtr first, TermPtr second, TermHolder* owner);

    TermPtr _left;
    TermPtr _right;
};

} /* namespace autodiff */
