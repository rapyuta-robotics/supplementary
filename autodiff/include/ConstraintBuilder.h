#pragma once
#include "Types.h"
namespace autodiff
{

class ConstraintBuilder
{
  public:
    static TermPtr equals(TermPtr t1, TermPtr t2, TermPtr tolerance);
    static TermPtr ifThen(TermPtr tif, TermPtr tthen);
    static TermPtr ifThenElse(TermPtr tif, TermPtr tthen, TermPtr telse);
    static TermPtr equiv(TermPtr a, TermPtr b);
    static TermPtr constraintApply(TermPtr constraint, TermPtr utility);
};

} // namespace autodiff
