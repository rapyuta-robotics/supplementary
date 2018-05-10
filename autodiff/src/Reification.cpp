
#include "Reification.h"
#include <sstream>

namespace autodiff
{
Reification::Reification(TermPtr condition, double min, double max, TermHolder* owner)
    : Term(owner)
    , _condition(condition)
    , _negatedCondition(condition->negate())
    , _min(min)
    , _max(max)
{
}

int Reification::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr Reification::aggregateConstants()
{
    return this;
}

TermPtr Reification::derivative(VarPtr v) const
{
    throw "Symbolic Derivation of Discretizer not supported.";
    return nullptr;
}

std::string Reification::toString() const
{
    std::stringstream str;
    str << "Discretizer( " << _condition->toString() << ", " << _min << ", " << _max << " )";
    return str.str();
}
} /* namespace autodiff */
