#pragma once

namespace autodiff
{
class Term;
class TermPtr;
class Variable;
class Tape;

class Abs;
class And;
class Constant;
class ConstPower;
class Cos;
class Exp;
class LinSigmoid;
class Log;
class LTEConstraint;
class LTConstraint;
class Max;
class Min;
class Or;
class Product;
class Reification;
class Sigmoid;
class Sin;
class Sum;
class TermPower;

// using TermPtr = Term*;
using VarPtr = Variable*;

union Parameter
{
    double asDouble;
    int asIdx;
};

typedef void (*EvalFunction)(const Tape&, const Parameter*, double*, const double*, int);
}