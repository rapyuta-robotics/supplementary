#include "Sigmoid.h"

#include "ConstPower.h"
#include "Constant.h"
#include "Exp.h"
#include "TermHolder.h"

#include <cmath>
#include <limits>
#include <sstream>

namespace autodiff
{

Sigmoid::Sigmoid(TermPtr arg, TermPtr mid, TermHolder* owner)
    : Term(owner)
    , _arg(arg)
    , _mid(mid)
    , _steepness(1.0)
{
}

Sigmoid::Sigmoid(TermPtr arg, TermPtr mid, double steepness, TermHolder* owner)
    : Term(owner)
    , _arg(arg)
    , _mid(mid)
    , _steepness(steepness)
{
}

int Sigmoid::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Sigmoid::aggregateConstants()
{
    _arg = _arg->aggregateConstants();
    _mid = _mid->aggregateConstants();
    if (_arg->isConstant() && _mid->isConstant()) {
        double e = exp(steepness * (-static_cast<Constant*>(_arg)->getValue() + static_cast<Constant*>(_mid)->getValue()));
        if (e == numeric_limits<double>::infinity()) {
            return _owner->constant(Term::EPSILON);
        } else {
            e = 1.0 / (1.0 + e);
        }
        if (e < Term::EPSILON) {
            return _owner->constant(Term::EPSILON);
        } else {
            return _owner->constant(e);
        }
    } else {
        return this;
    }
}

TermPtr Sigmoid::derivative(shared_ptr<Variable> v) const
{
    TermPtr t = steepness * (arg->derivative(v) - mid->derivative(v)) * _owner->exp(steepness * (-1 * arg + mid));
    return t / _owner->constPower(_owner->exp(steepness * arg) + _owner->exp(steepness * mid), 2);
}

std::string Sigmoid::toString() const
{
    std::stringstream str;
    str << "sigmoid( " << arg->toString() << ", " << mid->toString() << ", " << steepness << " )";
    return str.str();
}
} /* namespace autodiff */
