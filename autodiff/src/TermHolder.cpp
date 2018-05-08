#include "TermHolder.h"
#include <limits>
namespace autodiff
{

TermHolder::TermHolder()
    : _true(new Constant(1.0, this))
    , _zero(new Constant(0.0, this))
    , _false(new Constant(std::numeric_limits<double>::min(), this))
{
}

TermHolder::~TermHolder()
{
    for (TermPtr t : _terms) {
        delete t;
    }
    delete _true;
    delete _zero;
    delete _false;
}

VarPtr TermHolder::createVariable()
{
    VarPtr ret = new Variable(this) _vars.push_back(ret);
    _term.push_back(ret);
    return ret;
}

TermPtr TermHolder::trueConstant() const
{
    return _true;
}

TermPtr TermHolder::zeroConstant() const
{
    return _zero;
}
TermPtr TermHolder::falseConstant() const
{
    return _false;
}

TermPtr TermHolder::constant(double v)
{
    TermPtr ret = new Constant(v, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::sum(TermPtr left, TermPtr right)
{
    TermPtr ret = new Sum(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::product(TermPtr left, TermPtr right)
{
    TermPtr ret = new Product(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::min(TermPtr left, TermPtr right)
{
    TermPtr ret = new Min(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::max(TermPtr left, TermPtr right)
{
    TermPtr ret = new Max(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::and(TermPtr left, TermPtr right)
{
    TermPtr ret = new And(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder:: or (TermPtr left, TermPtr right)
{
    TermPtr ret = new Or(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::lessThan(TermPtr left, TermPtr right, double steepness)
{
    TermPtr ret = new LTConstraint(left, right, steepness, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::lessThanEqual(TermPtr left, TermPtr right, double steepness)
{
    TermPtr ret = new LTEConstraint(left, right, steepness, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::abs(TermPtr arg)
{
    TermPtr ret = new Abs(arg, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::constPower(TermPtr arg, double exponent)
{
    TermPtr ret = new ConstPower(arg, exponent, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::sin(TermPtr arg)
{
    TermPtr ret = new sin(arg, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::cos(TermPtr arg)
{
    TermPtr ret = new cos(arg, this);
    handleNewTerm(ret);
    return ret;
}

void TermHolder::handleNewTerm(TermPtr t)
{
    _terms.push_back(t);
}
}