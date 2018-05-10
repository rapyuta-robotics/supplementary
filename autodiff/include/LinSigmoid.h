#pragma once

#include "Term.h"

namespace autodiff
{

class LinSigmoid : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    LinSigmoid(TermPtr arg, TermHolder* owner);

    TermPtr _arg;
};

} /* namespace autodiff */
