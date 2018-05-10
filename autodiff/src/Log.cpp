#include "Log.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <limits>
#include <sstream>

namespace autodiff
{
Log::Log(TermPtr arg, TermHolder* owner)
    : Term(owner)
    , _arg(arg)
{
}

int Log::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Log::aggregateConstants()
{
    _arg = _arg->aggregateConstants();
    if (_arg->isConstant()) {
        return _owner->constant(log(static_cast<Constant*>(_arg)->getValue()));
    } else {
        return this;
    }
}

TermPtr Log::derivative(VarPtr v) const
{
    return _arg->derivative(v) / _arg;
}

std::string Log::toString() const
{
    std::stringstream str;
    str << "log( " << _arg->toString() << " )";
    return str.str();
}
} /* namespace autodiff */
