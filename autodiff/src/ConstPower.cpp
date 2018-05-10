/*
 * ConstPower.cpp
 *
 *  Created on: Jun 16, 2014
 *      Author: psp
 */

#include "ConstPower.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
ConstPower::ConstPower(TermPtr baseTerm, double exponent, TermHolder* owner)
    : Term(owner)
    , _base(baseTerm)
    , _exponent(exponent)
{
}

int ConstPower::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr ConstPower::aggregateConstants()
{
    _base = _base->aggregateConstants(); // all your base are belong to us

    if (_base->isConstant()) {
        return _owner->constant(pow(static_cast<Constant*>(_base)->getValue(), _exponent));
    }

    ConstPower* constPowerBase = dynamic_cast<ConstPower*>(_base.get());

    if (constPowerBase != nullptr) {
        _exponent *= constPowerBase->_exponent;
        _base = constPowerBase->_base;
    }

    return this;
}

TermPtr ConstPower::derivative(VarPtr v) const
{
    return _owner->constant(_exponent) * _owner->constPower(_base, _exponent - 1) * _base->derivative(v);
}

std::string ConstPower::toString() const
{
    std::stringstream str;
    str << "constPower( ";
    if (_base != nullptr) {
        str << _base->toString();
    } else {
        str << "nullptr";
    }
    str << ", " << _exponent << " )";
    return str.str();
}
} /* namespace autodiff */
