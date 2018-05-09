/*
 * TermBuilder.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: psp
 */

#include "TermBuilder.h"

#include "ConstPower.h"
#include "Constant.h"
#include "Cos.h"
#include "Exp.h"
#include "LTConstraint.h"
#include "LTEConstraint.h"
#include "Log.h"
#include "Product.h"
#include "Sigmoid.h"
#include "Sin.h"
#include "Sum.h"
#include "TVec.h"
#include "Term.h"
#include "TermPower.h"
#include "Zero.h"

#include <cmath>

#include <iostream>

namespace autodiff
{

/**
 * Builds a power terms given a base and a constant exponent
 *
 * @param t The power base term
 * @param power The exponent
 *
 * @return A term representing t^power.
 */
shared_ptr<Term> TermBuilder::power(shared_ptr<Term> t, double power)
{
    return make_shared<ConstPower>(t, power);
}

/**
 * Builds a power term given a base term and an exponent term.
 *
 * @param baseTerm The base term
 * @param exponent The exponent term
 *
 * @return
 */
shared_ptr<Term> TermBuilder::power(shared_ptr<Term> baseTerm, shared_ptr<Term> exponent)
{
    return make_shared<TermPower>(baseTerm, exponent);
}

/**
 * Constructs a 2D quadratic form given the vector components x1, x2 and the matrix coefficients a11, a12, a21, a22.
 *
 * @param x1 First vector component
 * @param x2 Second vector component
 * @param a11 First row, first column matrix component
 * @param a12 First row, second column matrix component
 * @param a21 Second row, first column matrix component
 * @param a22 Second row, second column matrix component
 *
 * @return A term describing the quadratic form
 */
shared_ptr<Term> TermBuilder::quadform(shared_ptr<Term> x1, shared_ptr<Term> x2, shared_ptr<Term> a11, shared_ptr<Term> a12, shared_ptr<Term> a21,
                                       shared_ptr<Term> a22)
{
    vector<shared_ptr<Term>> rest = {a22 * power(x2, 2)};
    return make_shared<Sum>(a11 * power(x1, 2), (a12 + a21) * x1 * x2, rest);
}

shared_ptr<Term> TermBuilder::normalDistribution(shared_ptr<TVec> args, shared_ptr<TVec> mean, double variance)
{
    return exp((args - mean)->normSquared() * (-0.5 / variance)) * (1 / sqrt(2.0 * M_PI * variance));
}

shared_ptr<Term> TermBuilder::gaussian(shared_ptr<TVec> args, shared_ptr<TVec> mean, double variance)
{
    return exp((args - mean)->normSquared() * (-0.5 / variance));
}

shared_ptr<Term> TermBuilder::sigmoid(shared_ptr<Term> arg, shared_ptr<Term> upperBound, shared_ptr<Term> lowerBound, shared_ptr<Term> mid, double steepness)
{
    return (upperBound - lowerBound) * (make_shared<Sigmoid>(arg, mid, steepness)) + lowerBound;
}

shared_ptr<Term> TermBuilder::boundedValue(shared_ptr<Term> arg, shared_ptr<Term> leftBound, shared_ptr<Term> rightBound, double steepness)
{
    return make_shared<LTConstraint>(leftBound, arg, steepness) & make_shared<LTConstraint>(arg, rightBound, steepness);
}

shared_ptr<Term> TermBuilder::boundedRectangle(shared_ptr<TVec> arg, shared_ptr<TVec> rightLower, shared_ptr<TVec> leftUpper, double steepness)
{
    return boundedValue(arg->getX(), rightLower->getX(), leftUpper->getX(), steepness) &
           boundedValue(arg->getY(), rightLower->getY(), leftUpper->getY(), steepness);
}

shared_ptr<Term> TermBuilder::euclidianDistanceSqr(shared_ptr<TVec> one, shared_ptr<TVec> two)
{
    return (one - two)->normSquared();
}

shared_ptr<Term> TermBuilder::euclidianDistance(shared_ptr<TVec> one, shared_ptr<TVec> two)
{
    return power(euclidianDistanceSqr(one, two), 0.5);
}

shared_ptr<Term> TermBuilder::polynom(vector<shared_ptr<Term>> input, int degree, vector<shared_ptr<Term>> param)
{
    shared_ptr<Term> ret = constant(0);
    for (int i = 0; i < input.size(); ++i) {
        shared_ptr<Term> t = constant(1);
        for (int j = 1; j < degree; ++j) {
            t = t * input[i];
        }
        ret = ret + t * param[i];
    }
    return ret;
}

} /* namespace autodiff */
