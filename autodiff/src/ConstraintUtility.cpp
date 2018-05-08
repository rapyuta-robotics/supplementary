/*
 * ConstraintUtility.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: psp
 */

#include "ConstraintUtility.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{
ConstraintUtility::ConstraintUtility(TermPtr constraint, TermPtr utility, TermHolder* owner)
    : Term(owner)
    , _constraint(constraint)
    , _utility(utility)
{
}

int ConstraintUtility::accept(ITermVisitor* visitor)
{
    return visitor->visit(this);
}

TermPtr ConstraintUtility::aggregateConstants()
{
    _constraint = constraint->aggregateConstants();
    _utility = utility->aggregateConstants();
    return this;
}

TermPtr ConstraintUtility::derivative(VarPtr v) const
{
    throw "Symbolic derivation of ConstraintUtility not supported.";
    return nullptr;
}

TermPtr ConstraintUtility::negate() const
{
    throw "Cannot negate a Constraint Utility";
    return nullptr;
}

std::string ConstraintUtility::toString() const
{
    std::stringstream str;
    str << "[ConstraintUtility: Constraint=" << constraint->toString());
    str << ", Utility=" << utility->toString() << "]";
    return str.str();
}
} /* namespace autodiff */
