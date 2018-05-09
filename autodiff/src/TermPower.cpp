/*
 * TermPower.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: psp
 */

#include "TermPower.h"

#include "ConstPower.h"
#include "Constant.h"
#include "Log.h"
#include "TermHolder.h"
#include "Zero.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
TermPower::TermPower(TermPtr baseTerm, TermPtr exponent, TermHolder* owner)
    : Term(owner)
    , _base(baseTerm)
    , _exponent(exponent)
{
}

int TermPower::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr TermPower::aggregateConstants()
{
    _base = _base->aggregateConstants();
    _exponent = _exponent->aggregateConstants();
    TermPower* basetp = dynamic_cast<TermPower*>(_base);

    if (_base.isConstant() && _exponent.isConstant()) {
        return _owner->constant(pow(static_cast<Constant*>(_base)->getValue(), static_cast<Constant*>(_exponent)->getValue()));
    } else if (_exponent.isConstant()) {
        return _owner->constPower(_base, static_cast<Constant*>(_exponent)->getValue());
    } else if (basetp != nullptr) {
        _exponent = _exponent * basetp->_exponent;
        _base = basetp->_base;
    }
    return this;
}

TermPtr TermPower::derivative(VarPtr v) const
{
    return _owner->termPower(base, exponent - 1) * (exponent * base->derivative(v) + base * _owner->log(base) * exponent->derivative(v));
}

std::string TermPower::toString() const
{
    std::stringstream str;
    str << "termPower( " << base->toString() << ", " << exponent->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
