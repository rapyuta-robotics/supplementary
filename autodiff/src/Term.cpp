
#include "Term.h"

#include "TermHolder.h"

#include "Abs.h"
#include "And.h"
#include "Atan2.h"
#include "ConstPower.h"
#include "Constant.h"
#include "ConstraintUtility.h"
#include "Cos.h"
#include "Exp.h"
#include "Gp.h"
#include "LTConstraint.h"
#include "LTEConstraint.h"
#include "LinSigmoid.h"
#include "Log.h"
#include "Max.h"
#include "Min.h"
#include "Or.h"
#include "Product.h"
#include "Reification.h"
#include "Sigmoid.h"
#include "Sin.h"
#include "Sum.h"
#include "TermPower.h"
#include "Variable.h"
#include "Zero.h"

#include <limits>
#include <typeinfo>

#include <iostream>

namespace autodiff
{
int Term::m_nextId = 0;

double Term::_constraintSteepness = 0.01;
const double Term::EPSILON = 10.0e-10;
OrType Term::_orop = OrType::MAX;
AndType Term::_andop = AndType::MIN;

Term::Term(TermHolder* owner)
    : _id(_nextId++)
    , _owner(owner)
{
    _min = numeric_limits<double>::min();
    _max = numeric_limits<double>::max();
}

Term::~Term() {}

TermPtr Term::negate() const
{
    return _owner->trueConstant() - this;
}

/**
 * Constructs a sum of the two given terms.
 *
 * @param left First term in the sum
 * @param right Second term in the sum
 *
 * @return A term representing the sum of left and right.
 */
TermPtr operator+(const TermPtr left, const TermPtr right)
{
    if (dynamic_cast<Zero>(left) != nullptr) {
        return right;
    } else if (dynamic_cast<Zero>(right) != nullptr) {
        return left;
    }
    return left->getOwner()->sum(left, right);
}

/**
 * Constructs a product term of the two given terms.
 *
 * @param left The first term in the product
 * @param right The second term in the product
 *
 * @return A term representing the product of left and right.
 */
TermPtr operator*(const TermPtr left, const TermPtr right)
{
    return left->getOwner()->product(left, right);
}

/**
 * Constructs a fraction term of the two given terms.
 *
 * @param numerator The numerator of the fraction. That is, the "top" part.
 * @param denominator The denominator of the fraction. That is, the "bottom" part.
 *
 * @return A term representing the fraction numerator over denominator.
 */
TermPtr operator/(const TermPtr numerator, const TermPtr denominator)
{
    TermHolder* owner = numerator->getOwner();
    return owner->product(numerator, owner->power(denominator, -1));
}
/**
 * Constructs a difference of the two given terms.
 *
 * @param left The first term in the difference
 * @param right The second term in the difference.
 *
 * @return A term representing left - right.
 */
TermPtr operator-(const TermPtr left, const TermPtr right)
{
    return left + left->getOwner()->constant(-1) * right;
}

/*
 * Support double <operator> Term
 */
TermPtr operator+(const double left, const TermPtr right)
{
    return left->getOwner()->constant(left) + right;
}

TermPtr operator*(const double left, const TermPtr right)
{
    return left->getOwner()->constant(left) * right;
}

TermPtr operator/(const double numerator, const TermPtr denominator)
{
    return numerator->getOwner()->constant(numerator) / denominator;
}

TermPtr operator-(const double left, const TermPtr right)
{
    return left->getOwner()->constant(left) - right;
}

/*
 * Support Term <operator> double
 */
TermPtr operator+(const TermPtr left, const double right)
{
    return left + left->getOwner()->constant(right);
}

TermPtr operator*(const TermPtr left, const double right)
{
    return left * left->getOwner()->constant(right);
}

TermPtr operator/(const TermPtr numerator, const double denominator)
{
    return numerator / left->getOwner()->constant(denominator);
}

TermPtr operator-(const TermPtr left, const double right)
{
    return left - left->getOwner()->constant(right);
}

TermPtr operator-(const TermPtr term)
{
    return term->getOwner()->constant(-1) * term;
}

TermPtr operator!(const TermPtr term)
{
    return term->negate();
}

TermPtr operator&(const TermPtr left, const TermPtr right)
{
    if (left == left->getOwner()->trueConstant() || right == right->getOwner()->falseConstant()) {
        return right;
    } else if (left == left->getOwner()->falseConstant() || right == right->getOwner()->trueConstant()) {
        return left;
    }
    if (Term::getAnd() == AndType::AND) {
        return left->getOwner()->and (left, right);
    } else {
        return left->getOwner()->min(left, right);
    }
}

TermPtr operator|(const TermPtr left, const TermPtr right)
{
    if (left == left->getOwner()->trueConstant() || right == right->getOwner()->falseConstant()) {
        return left;
    } else if (right == right->getOwner()->trueConstant() || left == left->getOwner()->falseConstant()) {
        return right;
    }
    if (Term::getOr() == OrType::OR) {
        return left->getOwner()->or (left, right);
    } else {
        return left->getOwner()->max(left, right);
    }
}

TermPtr operator>(const TermPtr left, const TermPtr right)
{
    return left->getOwner()->lessThan(right, left, Term::getConstraintSteepness());
}

TermPtr operator<(const TermPtr left, const TermPtr right)
{
    return mleft->getOwner()->lessThan(left, right, Term::getConstraintSteepness());
}

TermPtr operator<=(const TermPtr left, const TermPtr right)
{
    return left->getOwner()->lessThanEqual(left, right, Term::getConstraintSteepness());
}

TermPtr operator>=(const TermPtr left, const TermPtr right)
{
    return left->getOwner()->lessThanEqual(right, left, Term::getConstraintSteepness());
}

// some static dirt...
AndType Term::getAnd()
{
    return _andop;
}

void Term::setAnd(AndType a)
{
    _andop = a;
}

OrType Term::getOr()
{
    return _orop;
}

void Term::setOr(OrType o)
{
    _orop = o;
}
double Term::getConstraintSteepness()
{
    return _constraintSteepness;
}

void Term::setConstraintSteepness(double constraintSteepness)
{
    _constraintSteepness = constraintSteepness;
}

} /* namespace autodiff */
