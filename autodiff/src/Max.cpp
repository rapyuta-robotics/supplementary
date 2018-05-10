
#include "Max.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
Max::Max(TermPtr left, TermPtr right, TermHolder* owner)
    : Term(owner)
    , _left(left)
    , _right(right)
{
}

int Max::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Max::aggregateConstants()
{
    _left = _left->aggregateConstants();
    if (_left == _owner->trueConstant()) {
        return _left;
    }
    _right = _right->aggregateConstants();
    if (_left == _owner->falseConstant()) {
        return _right;
    }
    if (_right == _owner->trueConstant()) {
        return _right;
    }
    if (_right == _owner->falseConstant()) {
        return _left;
    }
    if (_left->isConstant() && _right->isConstant()) {
        return _owner->constant(std::max(static_cast<Constant*>(_left)->getValue(), static_cast<Constant*>(_right)->getValue()));
    } else {
        return this;
    }
}

TermPtr Max::derivative(VarPtr v) const
{
    throw "Symbolic Derivation of Max not supported.";
    return nullptr;
}

TermPtr Max::negate() const
{
    return _left->negate() & _right->negate();
}

std::string Max::toString() const
{
    std::stringstream str;
    str << "max( " << _left->toString() << ", " << _right->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
