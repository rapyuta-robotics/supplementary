#pragma once

#include "Term.h"

namespace autodiff
{

class LinSigmoid : public Term
{
  public:
    int accept(shared_ptr<ITermVisitor> visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(shared_ptr<Variable> v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    LinSigmoid(TermPtr arg, TermHolder* owner);

    TermPtr _arg;
};

} /* namespace autodiff */
