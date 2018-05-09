
#include "Sin.h"

#include "Constant.h"
#include "Cos.h"
#include "TermHolder.h"
#include "Zero.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
Sin::Sin(TermPtr arg, TermHolder* owner)
    : Term(owner)
    , _arg(arg)
{
}

int Sin::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Sin::aggregateConstants()
{
    _arg = _arg->aggregateConstants();
    if (arg->isConstant()) {
        return _owner->constant(sin(static_cast<Constant*>(arg)->getValue()));
    }
    return this;
}

TermPtr Sin::derivative(VarPtr v) const
{
    return _owner->cos(arg) * arg->derivative(v);
}

std::string Sin::toString() const
{
    std::stringstream str;
    str << "sin( " << arg->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
