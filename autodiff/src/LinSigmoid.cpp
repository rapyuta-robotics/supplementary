#include "LinSigmoid.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <limits>
#include <sstream>

namespace autodiff
{
LinSigmoid::LinSigmoid(TermPtr arg, TermHolder* owner)
    : Term(owner)
    , _arg(arg)
{
}

int LinSigmoid::accept(ITermVisitor* visitor)
{
    _arg->accept(visitor);
    return visitor->visit(this);
}

TermPtr LinSigmoid::aggregateConstants()
{
    _arg = _arg->aggregateConstants();
    if (_arg->isConstant()) {
        double e = exp(-static_cast<Constant*>(_arg)->getValue());
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
    }
    return this;
}

TermPtr LinSigmoid::derivative(VarPtr v) const
{
    // TermPtr e = _owner->exp(-arg);
    // return e / _owner->constantPower(e + 1.0, 2.0);
    return _arg->derivative(v);
}

std::string LinSigmoid::toString() const
{
    std::stringstream str;
    str << "sigmoid( " << _arg->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
