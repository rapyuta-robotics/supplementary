
#include "Abs.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{

Abs::Abs(TermPtr arg, TermHolder* owner)
    : Term(owner)
    , _arg(arg)
{
}

int Abs::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Abs::aggregateConstants()
{
    _arg = _arg->aggregateConstants();
    if (_arg->isConstant()) {
        return _owner->constant(fabs(static_cast<Constant*>(_arg)->getValue()));
    } else {
        return this;
    }
}

TermPtr Abs::derivative(VarPtr v) const
{
    return _arg->derivative(v) * _arg / this;
}

std::string Abs::toString() const
{
    std::stringstream str;
    str << "abs( " << _arg->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
