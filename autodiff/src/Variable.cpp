#include "Variable.h"

#include "TermHolder.h"

#include <limits>
#include <sstream>

namespace autodiff
{
int Variable::var_id = 0;

Variable::Variable(TermHolder* owner)
    : Term(owner)
    , _globalMin(-std::numeric_limits<double>::infinity())
    , _globalMax(std::numeric_limits<double>::infinity())
    , _ownId(var_id++)
{
}

int Variable::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Variable::aggregateConstants()
{
    return this;
}

TermPtr Variable::derivative(VarPtr v)
{
    if (this == v) {
        return _owner->constant(1);
    } else {
        return _owner->zeroConstant(0);
    }
}

std::string Variable::toString() const
{
    std::stringstream str;
    if (_ownId < 0) {
        str << "Var_";
        str << -ownId;
    } else {
        str << "Var";
        str << std::to_string(ownId);
    }
    return str.str();
}
} /* namespace autodiff */
