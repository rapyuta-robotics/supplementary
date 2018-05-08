
#include "LTEConstraint.h"

#include "Constant.h"
#include "LTConstraint.h"
#include "TermBuilder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
LTEConstraint::LTEConstraint(TermPtr x, TermPtr y, double steppness, TermHolder* owner)
    : Term(owner)
    , _left(x)
    , _right(y)
    , _steepness(steepness)
    , _negatedform(nullptr)
{
}

int LTEConstraint::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr LTEConstraint::aggregateConstants()
{
    _left = _left->aggregateConstants();
    _right = _right->aggregateConstants();
    if (_left->isConstant() && _right->isConstant()) {
        if (static_cast<Constant*>(_left)->getValue() <= static_cast<Constant*>(_right)->getValue()) {
            return _owner->trueConstant();
        } else {
            return _owner->falseConstant();
        }
    } else {
        return this;
    }
}

TermPtr LTEConstraint::derivative(VarPtr v) const
{
    throw "Symbolic Derivation of Less-Than-Or-Equal not supported.";
}

TermPtr LTEConstraint::negate() const
{
    if (_negatedform == nullptr) {
        _negatedform = _owenr->lessThan(right, left, steepness);
        _negatedform->setNegation(this);
    }
    return _negatedform;
}

std::string LTEConstraint::toString() const
{
    std::stringstream str;
    str << left->toString() << " <= " << right->toString();
    return str.str();
}
} /* namespace autodiff */
