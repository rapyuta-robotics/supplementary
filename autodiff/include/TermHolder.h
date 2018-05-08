#pragma once

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
    TermPtr and (TermPtr left, TermPtr right);
    TermPtr or (TermPtr left, TermPtr right);
    TermPtr lessThan(TermPtr left, TermPtr right);
    TermPtr lessThanEqual(TermPtr left, TermPtr right);

    TermPtr constant(double value);
    TermPtr abs(TermPtr arg);
    TermPtr constPower(TermPtr arg, double exponent);
    TermPtr sin(TermPtr arg);
    TermPtr cos(TermPtr arg);

  private:
    void handleNewTerm(TermPtr t);
    std::vector<TermPtr> _terms;
    std::vector<VarPtr> _vars;
    TermPtr _true;
    TermPtr _false;
    TermPtr _zero;
}
}