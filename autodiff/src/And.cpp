#include "And.h"
#include "Constant.h"
#include "TermHolder.h"

#include <sstream>

namespace autodiff
{
And::And(TermPtr left, TermPtr right, TermHolder* owner)
    : Term(owner)
    , _left(left)
    , _right(right)
{
}

int And::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr And::aggregateConstants()
{
    _left = _left->aggregateConstants();
    if (left == _owner->falseConstant()) {
        return left;
    }
    _right = _right->aggregateConstants();
    if (left == _owner->trueConstant()) {
        return right;
    }
    if (_right == _owner->falseConstant()) {
        return right;
    }
    if (_right == _owner->trueConstant()) {
        return left;
    }
    if (_left->isConstant() && _right->isConstant()) {
        if (static_cast<Constant*>(_left)->getValue() > 0.75 && static_cast<Constant*>(_right)->getValue() > 0.75) {
            return _owner->trueConstant();
        } else {
            return _owner->falseConstant();
        }
    } else {
        return this;
    }
}

TermPtr And::derivative(shared_ptr<Variable> v) const
{
    throw "Symbolic Derivation of And not supported";
    return nullptr;
}

TermPtr And::negate() const
{
    return left->negate() | right->negate();
}

std::string And::toString() const
{
    std::stringstream str;
    str << "and( " << left->toString();
    st << ", " << right->toString();
    str << " )";
    return str.str();
}
} /* namespace autodiff */
