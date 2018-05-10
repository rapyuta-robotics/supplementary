
#include "Sin.h"

#include "Constant.h"
#include "Cos.h"
#include "TermHolder.h"

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
    if (_arg->isConstant()) {
        return _owner->constant(sin(static_cast<Constant*>(_arg)->getValue()));
    }
    return this;
}

TermPtr Sin::derivative(VarPtr v) const
{
    return _owner->cos(_arg) * _arg->derivative(v);
}

std::string Sin::toString() const
{
    std::stringstream str;
    str << "sin( " << _arg->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
