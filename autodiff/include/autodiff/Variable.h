#pragma once

#include <alica_solver_interface/SolverVariable.h>

#include "Term.h"

namespace autodiff
{
class Variable : public Term, public alica::SolverVariable
{
  public:
    int accept(ITermVisitor* visitor) override;
    void acceptRecursive(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;
    static void Eval(const Tape& tape, const Parameter* params, double* result, const double* vars, int dim);

    void setVarId(int id) { _varId = id; }

    virtual EvalFunction getEvalFunction() const override { return &Eval; }
    virtual void fillParameters(Parameter* params) const override { params[0].asIdx = _varId; }

    double getGlobalMin() const { return _globalMin; }
    double getGlobalMax() const { return _globalMax; }

    void setGlobalMin(double m) { _globalMin = m; }
    void setGlobalMax(double m) { _globalMax = m; }

  private:
    friend TermHolder;
    Variable(TermHolder* owner);
    double _globalMin;
    double _globalMax;
    int _varId;
};
} /* namespace autodiff */
