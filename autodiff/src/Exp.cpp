/*
 * Exp.cpp
 *
 *  Created on: Jun 12, 2014
 *      Author: psp
 */

#include "Exp.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{

Exp::Exp(TermPtr arg, TermHolder* owner)
    : Term(owner)
    , _arg(arg)
{
}

int Exp::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Exp::aggregateConstants()
{
    _arg = _arg->aggregateConstants();
    if (_arg->isConstant()) {
        return _owner->constant(exp(static_cast<Constant*>(_arg)->getValue()));
    }
    return this;
}

TermPtr Exp::derivative(VarPtr v) const
{
    return this * _arg->derivative(v);
}

std::string Exp::toString() const
{
    std::stringstream str;
    str << "exp( " << _arg->toString() << " )";
    return str.str();
}

} /* namespace autodiff */
