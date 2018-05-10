#pragma once

#include <engine/constraintmodul/SolverVariable.h>

#include "Term.h"

namespace autodiff
{
class Variable : public Term, public alica::SolverVariable
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    Variable(TermHolder* owner);
    double _globalMin;
    double _globalMax;
    int _ownId;
    static int var_id;
};
} /* namespace autodiff */
