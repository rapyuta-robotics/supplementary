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
        double e = exp(_steepness * (-static_cast<Constant*>(_arg)->getValue() + static_cast<Constant*>(_mid)->getValue()));
        if (e == std::numeric_limits<double>::infinity()) {
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

TermPtr Sigmoid::derivative(VarPtr v) const
{
    TermPtr t = _steepness * (_arg->derivative(v) - _mid->derivative(v)) * _owner->exp(_steepness * (-1 * _arg + _mid));
    return t / _owner->constPower(_owner->exp(_steepness * _arg) + _owner->exp(_steepness * _mid), 2);
}

std::string Sigmoid::toString() const
{
    std::stringstream str;
    str << "sigmoid( " << _arg->toString() << ", " << _mid->toString() << ", " << _steepness << " )";
    return str.str();
}
} /* namespace autodiff */
