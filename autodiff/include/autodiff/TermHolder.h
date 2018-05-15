#pragma once

#include "Tape.h"
#include "TermPtr.h"
#include "Types.h"
#include <assert.h>
#include <memory>
#include <vector>

namespace autodiff
{

class TermHolder
{
  public:
    TermHolder();
    ~TermHolder();
    TermHolder(const TermHolder&) = delete;
    TermHolder& operator=(const TermHolder&) = delete;

    TermHolder(TermHolder&& o);
    TermHolder& operator=(TermHolder&& o);

    void addVariable(VarPtr v);
    VarPtr createVariable();
    TermPtr trueConstant() const { return _true.get(); }
    TermPtr zeroConstant() const { return _zero.get(); }
    TermPtr falseConstant() const { return _false.get(); }
    TermPtr sum(TermPtr left, TermPtr right);
    TermPtr product(TermPtr left, TermPtr right);
    TermPtr min(TermPtr left, TermPtr right);
    TermPtr max(TermPtr left, TermPtr right);
    TermPtr and_(TermPtr left, TermPtr right);
    TermPtr or_(TermPtr left, TermPtr right);
    TermPtr lessThan(TermPtr left, TermPtr right);
    TermPtr lessThanEqual(TermPtr left, TermPtr right);
    TermPtr reify(TermPtr arg);

    TermPtr constant(double value);
    TermPtr abs(TermPtr arg);
    TermPtr constPower(TermPtr arg, double exponent);
    TermPtr termPower(TermPtr arg, TermPtr exponent);
    TermPtr power(TermPtr arg, double exponent) { return constPower(arg, exponent); }
    TermPtr power(TermPtr arg, TermPtr exponent) { return termPower(arg, exponent); }
    TermPtr sigmoid(TermPtr arg, TermPtr mid, double steepness);
    TermPtr sigmoid(TermPtr arg, double steepness);
    TermPtr linSigmoid(TermPtr arg);

    TermPtr sin(TermPtr arg);
    TermPtr cos(TermPtr arg);
    TermPtr exp(TermPtr arg);
    TermPtr log(TermPtr arg);
    TermPtr atan2(TermPtr left, TermPtr right);

    TermPtr constraintUtility(TermPtr constraint, TermPtr utility);

    void compile(TermPtr top) { _tape.createFrom(top, _vars); }
    void evaluate(const double* input, double* output) const { return _tape.evaluate(input, output); }
    template <typename InputIt, typename OutputIt>
    inline void evaluate(InputIt point_begin, InputIt point_end, OutputIt value_begin, OutputIt value_end) const
    {
        assert(std::distance(point_begin, point_end) == static_cast<int>(_vars.size()));
        assert(std::distance(value_begin, value_end) >= static_cast<int>(_vars.size()) + 1);
        return _tape.evaluate(point_begin, point_end, value_begin);
    }

    int getDim() const { return static_cast<int>(_vars.size()); }

  private:
    void handleNewTerm(TermPtr t);
    std::vector<std::unique_ptr<Term>> _terms;
    std::vector<VarPtr> _vars;
    std::unique_ptr<Term> _true;
    std::unique_ptr<Term> _false;
    std::unique_ptr<Term> _zero;
    Tape _tape;
};
}