
#include "Atan2.h"

#include "ConstPower.h"
#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
Atan2::Atan2(TermPtr left, TermPtr right, TermHolder* owner)
    : Term(owner)
    , _left(left)
    , _right(right)
{
}

int Atan2::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Atan2::aggregateConstants()
{
    _left = _left->aggregateConstants();
    _right = _right->aggregateConstants();
    if (_left->isConstant() && _right->isConstant()) {
        return TermBuilder::constant(atan2(static_cast<Constant*>(_left)->getValue(), static_cast<Constant*>(_right)->getValue()));
    } else {
        return this;
    }
}

TermPtr Atan2::derivative(VarPtr v) const
{
    TermPtr t = left * right->derivative(v) - right * left->derivative(v);
    return t / (_owner->constPower(left, 2) + _owner->constPower(right, 2));
}

std::string Atan2::toString() const
{
    std::stringstream str;
    str << "atan2( " << left->toString();
    str << ", " << right->toString();
    str << " )";
    return str.str();
}
} /* namespace autodiff */
