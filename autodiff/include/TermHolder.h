#pragma once

#include "TermPtr.h"
#include "Types.h"
#include <vector>

namespace autodiff
{

class TermHolder
{
  public:
    TermHolder();
    ~TermHolder();
    void addVariable(VarPtr v);
    VarPtr createVariable();
    TermPtr trueConstant() const;
    TermPtr zeroConstant() const;
    TermPtr falseConstant() const;
    TermPtr sum(TermPtr left, TermPtr right);
    TermPtr product(TermPtr left, TermPtr right);
    TermPtr min(TermPtr left, TermPtr right);
    TermPtr max(TermPtr left, TermPtr right);
    TermPtr and_(TermPtr left, TermPtr right);
    TermPtr or_(TermPtr left, TermPtr right);
    TermPtr lessThan(TermPtr left, TermPtr right, double steepness);
    TermPtr lessThanEqual(TermPtr left, TermPtr right, double steepness);

    TermPtr constant(double value);
    TermPtr abs(TermPtr arg);
    TermPtr constPower(TermPtr arg, double exponent);
    TermPtr termPower(TermPtr arg, TermPtr exponent);
    TermPtr power(TermPtr arg, double exponent) { return constPower(arg, exponent); }
    TermPtr power(TermPtr arg, TermPtr exponent) { return termPower(arg, exponent); }
    TermPtr sigmoid(TermPtr arg, TermPtr mid, double steepness);

    TermPtr sin(TermPtr arg);
    TermPtr cos(TermPtr arg);
    TermPtr exp(TermPtr arg);
    TermPtr log(TermPtr arg);

    TermPtr constraintUtility(TermPtr constraint, TermPtr utility);

    // void compile(TermPtr top);

  private:
    void handleNewTerm(TermPtr t);
    std::vector<TermPtr> _terms;
    std::vector<VarPtr> _vars;
    TermPtr _true;
    TermPtr _false;
    TermPtr _zero;
};
}