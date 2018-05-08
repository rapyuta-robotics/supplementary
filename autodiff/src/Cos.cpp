/*
 * Cos.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: psp
 */

#include "Cos.h"

#include "Constant.h"
#include "Sin.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
Cos::Cos(TermPtr arg, TermHolder* owner)
    : Term(owner)
    , _arg(arg)
{
}

int Cos::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Cos::aggregateConstants()
{
    _arg = _arg->aggregateConstants();
    if (arg->isConstant()) {
        return _owner->constant(cos(static_cast<Constant*>(arg)->getValue()));
    }
    return this;
}

TermPtr Cos::derivative(VarPtr v) const
{
    return _owner->sin(arg) * -arg->derivative(v);
}

std::string Cos::toString() const
{
    std::stringstream str;
    str << "cos( " << arg->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
