/*
 * LTConstraint.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: psp
 */

#include "LTConstraint.h"

#include "Constant.h"
#include "LTEConstraint.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
LTConstraint::LTConstraint(TermPtr x, TermPtr y, double steepness, TermHolder* owner)
    : Term(owner)
    , _left(x)
    , _right(y)
    , _steepness(steepness)
    , _negatedform(nullptr)
{
}

int LTConstraint::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr LTConstraint::aggregateConstants()
{
    _left = _left->aggregateConstants();
    _right = _right->aggregateConstants();
    if (_left->isConstant() && _right->isConstant()) {
        if (static_cast<Constant*>(_left)->getValue() < static_cast<Constant*>(_right)->getValue()) {
            return _owner->trueConstant();
        } else {
            return _owner->falseConstant();
        }
    } else {
        return this;
    }
}

TermPtr LTConstraint::derivative(VarPtr v) const
{
    throw "Symbolic Derivation of Less-Than not supported.";
    return nullptr;
}

TermPtr LTConstraint::negate() const
{
    if (_negatedform == nullptr) {
        _negatedform = _owner->lessThanEqual(_right, _left, _steepness);
        _negatedform->setNegation(this);
    }
    return negatedform;
}

std::string LTConstraint::toString() const
{
    std::stringstream str;
    str << left->toString() << " < " << right->toString();
    return str.str();
}

} /* namespace autodiff */
