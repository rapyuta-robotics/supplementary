#pragma once

#include <engine/constraintmodul/SolverVariable.h>

#include "Term.h"

namespace autodiff
{
class Variable : public Term, public alica::SolverVariable
{
  public:
    int accept(ITermVisitor* visitor) override;

    std::TermPtr aggregateConstants() override;
    std::TermPtr derivative(std::shared_ptr<Variable> v) const override;

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
