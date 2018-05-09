

#include "Min.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
Min::Min(TermPtr left, TermPtr right, TermHolder* owner)
    : Term(owner)
    , _left(left)
    , _right(right)
{
}

int Min::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Min::aggregateConstants()
{
    _left = _left->aggregateConstants();
    _right = _right->aggregateConstants();
    if (_left->isConstant() && _right->isConstant()) {
        return _owner->constant(std::min(static_cast<Constant*>(_left)->getValue(), static_cast<Constant*>(_right)->getValue()));
    } else {
        return this;
    }
}

TermPtr Min::derivative(VarPtr v) const
{
    throw "Symbolic Derivation of Min not supported.";
    return nullptr;
}

TermPtr Min::negate() const
{
    return _left->negate() | _right->negate();
}

std::string Min::toString() const
{
    std::stringstream str;
    str << "min( " << _left->toString() << ", " << _right->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
