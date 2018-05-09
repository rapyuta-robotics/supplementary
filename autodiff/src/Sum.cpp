
#include "Sum.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{

Sum::Sum(TermPtr first, TermPtr second, TermHolder* owner)
    : Term(owner)
    , _left(first)
    , _right(second)
{
}

int Sum::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Sum::aggregateConstants()
{
    _left = _left->aggregateConstants();
    _right = _right->aggregateConstants();
    if (_left->isConstant() && _right->isConstant()) {
        return _owner->constant(static_cast<Constant*>(_left)->getValue() + static_cast<Constant*>(_right)->getValue());
    }
    if (_left->isConstant()) {
        if (static_cast<Constant*>(_left)->getValue() == 0.0) {
            return _right;
        }
    }
    if (_right->isConstant()) {
        if (static_cast<Constant*>(_right)->getValue() == 0.0) {
            return _left;
        }
    }
    return this;
}

TermPtr Sum::derivative(VarPtr v) const
{
    return _left->derivative(v) + _right->derivative(v);
}

std::string Sum::toString() const
{
    std::stringstream str;
    str << "( " << _left->toString() << " + " << _right->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
