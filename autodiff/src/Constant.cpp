/*
 * Constant.cpp
 *
 *  Created on: Jun 12, 2014
 *      Author: psp
 */

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>

namespace autodiff
{

Constant::Constant(double value, TermHolder* owner)
    : Term(owner)
    , _value(value)
{
}

int Constant::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Constant::aggregateConstants()
{
    return this;
}

TermPtr Constant::derivative(VarPtr v) const
{
    return _owner->zeroConstant();
}

std::string Constant::toString() const
{
    if (this == _owner->trueConstant()) {
        return "true";
    } else if (this == _owner->falseConstant()) {
        return "false";
    }

    return std::to_string(_value);
}
} /* namespace autodiff */
