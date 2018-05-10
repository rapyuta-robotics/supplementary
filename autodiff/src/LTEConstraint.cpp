
#include "LTEConstraint.h"

#include "Constant.h"
#include "LTConstraint.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
LTEConstraint::LTEConstraint(TermPtr x, TermPtr y, double steepness, TermHolder* owner)
    : Term(owner)
    , _left(x)
    , _right(y)
    , _steepness(steepness)
    , _negatedForm(nullptr)
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
    return nullptr;
}

TermPtr LTEConstraint::negate() const
{
    if (_negatedForm == nullptr) {
        _negatedForm = _owner->lessThan(_right, _left, _steepness);
        _negatedForm->setNegation(this);
    }
    return _negatedForm;
}

std::string LTEConstraint::toString() const
{
    std::stringstream str;
    str << _left->toString() << " <= " << _right->toString();
    return str.str();
}
} /* namespace autodiff */
