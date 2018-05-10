#include "Tape.h"

#include "AutoDiff.h"
#include "TermPtr.h"
#include <assert.h>
#include <stdlib.h>

#include <algorithm>
#include <cstring>
#include <iostream>

namespace autodiff
{
#define MAXARITY 2

Tape::Tape()
    : _values(nullptr)
    , _params(nullptr)
    , _functions(nullptr)
    , _base(nullptr)
{
}

Tape::~Tape()
{
    free(_base);
}

void Tape::evaluate(const double* input, double* output) const
{
    assert(isSet());
    const int dim = _tapeWidth - 1;
    for (int i = 0; i < _tapeLength; ++i) {
        _functions[i](*this, _params + i * MAXARITY, _values + i * _tapeWidth, input, dim);
    }
    memcpy(output, _values + (_tapeLength - 1), _tapeWidth);
}

void Tape::createFrom(TermPtr top, const std::vector<VarPtr>& vars)
{
    if (_base != nullptr) {
        free(_base);
        _base = nullptr;
        _allTerms.clear();
    }
    prepTerms(top, vars);
}

void Tape::prepTerms(TermPtr top, const std::vector<VarPtr>& vars)
{
    assert(_allTerms.empty());
    top->accept(this);

    // DEBUG::
    /*std::cout << "Number of terms: " << _allTerms.size() <<std::endl;
    for(Term* t : _allTerms) {
        std::cout << t->toString() <<std::endl;
    }*/
    // ENDDEBUG
    int termCount = static_cast<int>(_allTerms.size());
    int varCount = static_cast<int>(vars.size());
    _tapeWidth = varCount + 1;
    _tapeLength = termCount;

    size_t doubleCount = termCount * _tapeWidth;
    size_t paramCount = termCount * MAXARITY;

    _base = malloc(doubleCount * sizeof(double) + paramCount * sizeof(Parameter) + termCount * sizeof(EvalFunction));
    assert(_base != nullptr);
    /*
    _values =  static_cast<double*>(malloc(doubleCount*sizeof(double)));
    _params = static_cast<Parameter*>(malloc(paramCount*sizeof(Parameter)));
    _functions = static_cast<EvalFunction*>(malloc(termCount*sizeof(EvalFunction)));
    */

    _values = static_cast<double*>(_base);
    _params = reinterpret_cast<Parameter*>(_values + doubleCount);
    _functions = reinterpret_cast<EvalFunction*>(_params + paramCount);

    memset(_values, 0, sizeof(double) * doubleCount);

    int idx = 0;

    for (VarPtr v : vars) {
        v->setVarId(idx);
        ++idx;
    }
    idx = 0;
    for (Term* t : _allTerms) {
        t->setTapeIdx(idx);
        _functions[idx] = t->getEvalFunction();
        t->fillParameters(_params + (idx * MAXARITY));
        ++idx;
    }
}

int Tape::visit(Abs* elem)
{
    return visitTerm(elem);
}
int Tape::visit(And* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Atan2* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Constant* elem)
{
    return visitTerm(elem);
}
int Tape::visit(ConstPower* elem)
{
    return visitTerm(elem);
}
int Tape::visit(ConstraintUtility* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Cos* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Exp* elem)
{
    return visitTerm(elem);
}

int Tape::visit(LinSigmoid* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Log* elem)
{
    return visitTerm(elem);
}
int Tape::visit(LTConstraint* elem)
{
    return visitTerm(elem);
}
int Tape::visit(LTEConstraint* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Max* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Min* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Or* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Product* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Reification* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Sigmoid* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Sin* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Sum* elem)
{
    return visitTerm(elem);
}
int Tape::visit(TermPower* elem)
{
    return visitTerm(elem);
}
int Tape::visit(Variable* var)
{
    return visitTerm(var);
}

int Tape::visitTerm(Term* t)
{
    if (std::find(_allTerms.begin(), _allTerms.end(), t) == _allTerms.end()) {
        _allTerms.push_back(t);
    }
    return 0;
}

} // namespace autodiff