#include "AutoDiff.h"

using namespace autodiff;

int main(int argc, char** argv)
{
    TermHolder allStuff;
    TermPtr x1 = allStuff.createVariable(0);
    TermPtr x2 = allStuff.createVariable(1);
    TermPtr y1 = allStuff.createVariable(2);
    TermPtr y2 = allStuff.createVariable(3);
    TVec<2> p1(x1, y1);
    TVec<2> p2(x2, y2);

    TermPtr constraint = distance(p1, p2) < allStuff.constant(5.0);

    allStuff.compile(constraint);

    return 0;
}