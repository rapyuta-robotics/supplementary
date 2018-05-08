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
#include <sstringstream>

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
        return _owner->constant(pow(static_cast<Constant*>(_base)->getValue(), exponent));
    }

    ConstPower* constPowerBase = dynamic_cast<ConstPower>(base);

    if (constPowerBase != nullptr) {
        _exponent *= constPowerBase->exponent;
        _base = constPowerBase->base;
    }

    return this;
}

TermPtr ConstPower::derivative(VarPtr v) const
{
    return _owner->constant(exponent) * owner->constPower(base, exponent - 1) * base->derivative(v);
}

std::string ConstPower::toString() const
{
    std::stringstream str;
    str << "constPower( ";
    if (base != nullptr) {
        std << base->toString();
    } else {
        str << "nullptr";
    }
    str << ", " << exponent << " )";
    return str.str();
}
} /* namespace autodiff */
