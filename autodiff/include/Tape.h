#pragma once
#include "ITermVisitor.h"
#include "Types.h"
#include <set>
#include <vector>

namespace autodiff
{
class Tape;

class Tape : public ITermVisitor
{
  public:
    Tape();

    ~Tape();

    void evaluate(const double* input, double* output) const;

    void createFrom(TermPtr top, const std::vector<VarPtr>& vars);

    bool isSet() const { return _base != nullptr; }

    virtual int visit(Abs* a) override;
    virtual int visit(And* elem) override;
    virtual int visit(Atan2* elem) override;
    virtual int visit(Constant* elem) override;
    virtual int visit(ConstPower* elem) override;
    virtual int visit(ConstraintUtility* elem) override;
    virtual int visit(Cos* elem) override;
    virtual int visit(Exp* elem) override;
    virtual int visit(LinSigmoid* elem) override;
    virtual int visit(Log* elem) override;
    virtual int visit(LTConstraint* elem) override;
    virtual int visit(LTEConstraint* elem) override;
    virtual int visit(Max* elem) override;
    virtual int visit(Min* elem) override;
    virtual int visit(Or* elem) override;
    virtual int visit(Product* elem) override;
    virtual int visit(Reification* elem) override;
    virtual int visit(Sigmoid* elem) override;
    virtual int visit(Sin* elem) override;
    virtual int visit(Sum* elem) override;
    virtual int visit(TermPower* elem) override;
    virtual int visit(Variable* var) override;

    inline const double* getValues(int idx) const { return _values + (idx * _tapeWidth); }

  private:
    int visitTerm(Term* t);
    void prepTerms(TermPtr top, const std::vector<VarPtr>& vars);
    double* _values;
    Parameter* _params;
    EvalFunction* _functions;
    void* _base;
    std::vector<Term*> _allTerms;
    std::set<Term*> _allTermSet;
    int _tapeWidth;
    int _tapeLength;
};
}