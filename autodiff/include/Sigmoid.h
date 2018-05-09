#pragma once

#include "Term.h"

namespace autodiff
{

class Sigmoid : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    Sigmoid(TermPtr arg, TermPtr mid, TermHolder* owner);
    Sigmoid(TermPtr arg, TermPtr mid, double steepness, TermHolder* owner);

    TermPtr _arg;
    TermPtr _mid;
    double _steepness;
};

} /* namespace autodiff */
