#pragma once

#include "Term.h"

namespace autodiff
{

class Cos : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(shared_ptr<Variable> v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    Cos(TermPtr arg, TermHolder* owner);

    TermPtr _arg;
};

} /* namespace autodiff */
