#include "TermHolder.h"

#include "Abs.h"
#include "And.h"
#include "Atan2.h"
#include "ConstPower.h"
#include "Constant.h"
#include "ConstraintUtility.h"
#include "Cos.h"
#include "Exp.h"
#include "LTConstraint.h"
#include "LTEConstraint.h"
#include "LinSigmoid.h"
#include "Log.h"
#include "Max.h"
#include "Min.h"
#include "Or.h"
#include "Product.h"
#include "Reification.h"
#include "Sigmoid.h"
#include "Sin.h"
#include "Sum.h"
#include "TermPower.h"
#include "TermPtr.h"
#include "Variable.h"

#include <limits>

namespace autodiff
{

TermHolder::TermHolder()
    : _true(new Constant(1.0, this))
    , _zero(new Constant(0.0, this))
    , _false(new Constant(std::numeric_limits<double>::min(), this))
{
}

TermHolder::TermHolder(TermHolder&& o)
    : _true(std::move(o._true))
    , _zero(std::move(o._zero))
    , _false(std::move(o._false))
    , _vars(std::move(o._vars))
    , _terms(std::move(o._terms))
    , _tape(std::move(o._tape))
{
}
TermHolder& TermHolder::operator=(TermHolder&& o)
{
    _true = std::move(o._true);
    _zero = std::move(o._zero);
    _false = std::move(o._false);
    _vars = std::move(o._vars);
    _terms = std::move(o._terms);
    _tape = std::move(o._tape);
    return *this;
}

TermHolder::~TermHolder() {}

VarPtr TermHolder::createVariable(int64_t id)
{
    VarPtr ret = new Variable(this, id);
    _vars.push_back(ret);
    handleNewTerm(ret);
    return ret;
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
TermPtr TermHolder::and_(TermPtr left, TermPtr right)
{
    TermPtr ret = new And(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::or_(TermPtr left, TermPtr right)
{
    TermPtr ret = new Or(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::lessThan(TermPtr left, TermPtr right)
{
    TermPtr ret = new LTConstraint(left, right, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::lessThanEqual(TermPtr left, TermPtr right)
{
    TermPtr ret = new LTEConstraint(left, right, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::reify(TermPtr arg)
{
    TermPtr ret = new Reification(arg, this);
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

TermPtr TermHolder::termPower(TermPtr arg, TermPtr exponent)
{
    TermPtr ret = new TermPower(arg, exponent, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::sigmoid(TermPtr arg, double steepness)
{
    TermPtr ret = new Sigmoid(arg, steepness, this);
    handleNewTerm(ret);
    return ret;
}
TermPtr TermHolder::sigmoid(TermPtr arg, TermPtr mid, double steepness)
{
    return sigmoid(arg - mid, steepness);
}

TermPtr TermHolder::linSigmoid(TermPtr arg)
{
    TermPtr ret = new LinSigmoid(arg, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::sin(TermPtr arg)
{
    TermPtr ret = new Sin(arg, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::cos(TermPtr arg)
{
    TermPtr ret = new Cos(arg, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::exp(TermPtr arg)
{
    TermPtr ret = new Exp(arg, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::log(TermPtr arg)
{
    TermPtr ret = new Log(arg, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::atan2(TermPtr left, TermPtr right)
{
    TermPtr ret = new Atan2(left, right, this);
    handleNewTerm(ret);
    return ret;
}

TermPtr TermHolder::constraintUtility(TermPtr constraint, TermPtr utility)
{
    TermPtr ret = new ConstraintUtility(constraint, utility, this);
    handleNewTerm(ret);
    return ret;
}

void TermHolder::handleNewTerm(TermPtr t)
{
    _terms.emplace_back(t.get());
}

void TermHolder::clear()
{
    _terms.clear();
    _vars.clear();
    _tape = Tape();
}
}
