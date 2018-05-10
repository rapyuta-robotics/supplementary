/*
 * Or.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: psp
 */

#include "Or.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
Or::Or(TermPtr left, TermPtr right, TermHolder* owner)
    : Term(owner)
    , _left(left)
    , _right(right)
{
}

int Or::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Or::aggregateConstants()
{
    _left = _left->aggregateConstants();
    _right = _right->aggregateConstants();
    if (_left->isConstant() && _right->isConstant()) {
        if (static_cast<Constant*>(_left)->getValue() > 0.75 || static_cast<Constant*>(_right)->getValue() > 0.75) {
            return _owner->trueConstant();
        } else {
            return _owner->falseConstant();
        }
    } else {
        return this;
    }
}

TermPtr Or::derivative(VarPtr v) const
{
    throw "Symbolic Derivation of Or not supported.";
    return nullptr;
}

TermPtr Or::negate() const
{
    return _left->negate() & _right->negate();
}

std::string Or::toString() const
{
    std::stringstream str;
    str << "or( " << _left->toString() << ", " << _right->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
