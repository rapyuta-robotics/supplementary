#include "AutoDiff.h"

#include <gtest/gtest.h>

using namespace autodiff;

TEST(AutoDiffTest, ABS)
{
    TermHolder h;
    VarPtr x = h.createVariable();

    TermPtr func = h.abs(x);

    std::vector<double> point{-13};

    h.compile(func);

    std::vector<double> gradientAndValue(2);
    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];

    ASSERT_DOUBLE_EQ(13, eval);
    ASSERT_DOUBLE_EQ(-1, gradientAndValue[1]);
}

TEST(AutoDiffTest, AND)
{
    TermHolder h;
    TermPtr x = h.createVariable();
    TermPtr y = h.createVariable();

    TermPtr func = h.and_(x > h.zeroConstant(), y > h.zeroConstant());

    std::vector<double> point{13, 37};

    h.compile(func);
    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];

    ASSERT_DOUBLE_EQ(1, eval);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[1]);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[2]);
}

TEST(AutoDiffTest, ATAN2)
{
    TermHolder h;
    TermPtr x = h.createVariable();
    TermPtr y = h.createVariable();

    TermPtr func = h.atan2(x, y);

    std::vector<double> point{13, 37};
    h.compile(func);

    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];

    ASSERT_NEAR(0.337878, eval, 10E-6);
    ASSERT_NEAR(-0.024057, gradientAndValue[1], 10E-6);
    ASSERT_NEAR(0.008453, gradientAndValue[2], 10E-6);
}

TEST(AutoDiffTest, CONSTPOWER)
{
    TermHolder h;
    TermPtr x = h.createVariable();

    TermPtr func = h.constPower(x, 2);

    std::vector<double> point{13};
    h.compile(func);
    std::vector<double> gradientAndValue(2);

    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];

    ASSERT_DOUBLE_EQ(169, eval);
    ASSERT_DOUBLE_EQ(26, gradientAndValue[1]);
}

TEST(AutoDiffTest, CONSTRAINTUTILITY)
{
    TermHolder h;
    TermPtr x = h.createVariable();
    TermPtr y = h.createVariable();

    TermPtr func = h.constraintUtility(x < h.constant(42), y);

    std::vector<double> point{13, 37};

    h.compile(func);
    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];

    ASSERT_DOUBLE_EQ(37, eval);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[1]);
    ASSERT_DOUBLE_EQ(1, gradientAndValue[2]);
}

TEST(AutoDiffTest, COS)
{
    TermHolder h;
    TermPtr x = h.createVariable();

    TermPtr func = h.cos(x);

    std::vector<double> point{13};

    h.compile(func);
    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];
    ASSERT_NEAR(0.907447, eval, 10E-6);
    ASSERT_NEAR(-0.420167, gradientAndValue[1], 10E-6);
}

TEST(AutoDiffTest, EXP)
{
    TermHolder h;
    TermPtr x = h.createVariable();

    TermPtr func = h.exp(x);

    std::vector<double> point{13};

    h.compile(func);
    std::vector<double> gradientAndValue(2);

    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];

    ASSERT_NEAR(442413, eval, 10E-1);
    ASSERT_NEAR(442413, gradientAndValue[0], 10E-1);
}

TEST(AutoDiffTest, LINSIGMOID)
{
    TermHolder h;
    TermPtr x = h.createVariable();

    TermPtr func = h.linSigmoid(x);

    std::vector<double> point{0};
    h.compile(func);
    std::vector<double> gradientAndValue(2);

    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];

    ASSERT_DOUBLE_EQ(0.5, eval);
    ASSERT_DOUBLE_EQ(1.0, gradientAndValue[1]);
}

TEST(AutoDiffTest, LOG)
{
    TermHolder h;
    TermPtr x = h.createVariable();

    TermPtr func = h.log(x);

    std::vector<double> point{13};

    h.compile(func);
    std::vector<double> gradientAndValue(2);

    h.evaluate(&point[0], &gradientAndValue[0]);

    double eval = gradientAndValue[0];

    ASSERT_NEAR(2.56495, eval, 10E-5);
    ASSERT_NEAR(0, gradientAndValue[1], 10E-1);
}

TEST(AutoDiffTest, LTCONSTRAINT)
{
    TermHolder h;
    TermPtr x = h.createVariable();
    TermPtr y = h.createVariable();

    TermPtr func = h.lessThan(x, y);
    std::vector<double> point{13, 37};
    h.compile(func);
    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);

    ASSERT_DOUBLE_EQ(1, gradientAndValue[0]);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[1]);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[2]);

    std::swap(point[0], point[1]);

    h.evaluate(&point[0], &gradientAndValue[0]);

    ASSERT_NEAR(-0.24, gradientAndValue[0], 10E-2);
    ASSERT_DOUBLE_EQ(-0.01, gradientAndValue[1]);
    ASSERT_DOUBLE_EQ(0.01, gradientAndValue[2]);
}

TEST(AutoDiffTest, LTECONSTRAINT)
{
    TermHolder h;
    TermPtr x = h.createVariable();
    TermPtr y = h.createVariable();

    TermPtr func = h.lessThanEqual(x, y);

    std::vector<double> point{13, 37};
    h.compile(func);
    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);

    ASSERT_DOUBLE_EQ(1, gradientAndValue[0]);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[1]);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[2]);

    std::swap(point[0], point[1]);

    h.evaluate(&point[0], &gradientAndValue[0]);

    ASSERT_NEAR(-0.24, gradientAndValue[0], 10E-2);
    ASSERT_DOUBLE_EQ(-0.01, gradientAndValue[1]);
}

TEST(AutoDiffTest, MAX)
{
    TermHolder h;
    TermPtr x = h.createVariable();
    TermPtr y = h.createVariable();

    TermPtr func = h.max(x, y);

    std::vector<double> point{13, 37};

    h.compile(func);
    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);

    ASSERT_DOUBLE_EQ(37, gradientAndValue[0]);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[1]);
}

TEST(AutoDiffTest, MIN)
{
    TermHolder h;
    TermPtr x = h.createVariable();
    TermPtr y = h.createVariable();

    TermPtr func = h.min(x, y);

    std::vector<double> point{13, 37};

    h.compile(func);
    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);
    ASSERT_DOUBLE_EQ(13, gradientAndValue[0]);
    ASSERT_DOUBLE_EQ(1, gradientAndValue[1]);
}

TEST(AutoDiffTest, OR)
{
    TermHolder h;
    TermPtr x = h.createVariable();
    TermPtr y = h.createVariable();

    TermPtr func = h.or_(x, y);

    std::vector<double> point{13, 37};

    h.compile(func);
    std::vector<double> gradientAndValue(3);

    h.evaluate(&point[0], &gradientAndValue[0]);

    ASSERT_DOUBLE_EQ(1, gradientAndValue[0]);
    ASSERT_DOUBLE_EQ(0, gradientAndValue[1]);
}

TEST(AutoDiffTest, REIFICATION)
{
    TermHolder h;
    TermPtr x = h.createVariable();

    TermPtr func = h.reify(x > h.constant(3));

    std::vector<double> point{13};

    h.compile(func);
    std::vector<double> gradientAndValue(2);

    h.evaluate(&point[0], &gradientAndValue[0]);

    ASSERT_DOUBLE_EQ(1, gradientAndValue[0]);
    ASSERT_DOUBLE_EQ(0.01, gradientAndValue[1]);
}

TEST(AutoDiffTest, SIN)
{
    TermHolder h;
    TermPtr x = h.createVariable();

    TermPtr func = h.sin(x);

    std::vector<double> point{13};

    h.compile(func);
    std::vector<double> gradientAndValue(2);

    h.evaluate(&point[0], &gradientAndValue[0]);

    ASSERT_NEAR(0.420167, gradientAndValue[0], 10E-6);
    ASSERT_NEAR(0.907447, gradientAndValue[1], 10E-6);
}
/*
TEST(AutoDiffTest, TERMPOWER) {
    VarPtr x = make_shared<Variable>();
    VarPtr y = make_shared<Variable>();

    TermPtr func = TermBuilder::power(x, y);

    vector<VarPtr> vars{x, y};
    vector<double> point{13, 2};

    shared_ptr<ICompiledTerm> compiledFunc = TermUtils::compile(func, make_shared<vector<VarPtr>>(vars));
    pair<shared_ptr<vector<double>>, double> gradientAndValue =
            compiledFunc->differentiate(make_shared<vector<double>>(point));

    double eval = gradientAndValue.second;
    shared_ptr<vector<double>> gradient = gradientAndValue.first;

    ASSERT_DOUBLE_EQ(169, eval);
    ASSERT_DOUBLE_EQ(26, gradient->at(0));
    ASSERT_NEAR(433.476441, gradient->at(1), 10E-6);
}

#include <limits>
#include <memory>

TEST(AutoDiffTest, EQUALITY) {
    TermPtr TRUE = TermBuilder::constant(1);

    ASSERT_TRUE(TRUE == TRUE);
}

TEST(AutoDiffTest, COMPILED) {
    // we will use a function of two variables
    VarPtr x = make_shared<Variable>();
    VarPtr y = make_shared<Variable>();

    // Define our function: func(x, y) = (x + y) * exp(x - y)
    TermPtr func = (x + y) * TermBuilder::exp(x - y);

    // define the ordering of variables, and a point where
    // the function will be evaluated/differentiated
    vector<VarPtr> vars{x, y};
    vector<double> point{1, -2};

    // create a compiled version of func.
    shared_ptr<ICompiledTerm> compiledFunc = TermUtils::compile(func, make_shared<vector<VarPtr>>(vars));

    // calculate value and gradient at (x, y) = (1, -2)
    pair<shared_ptr<vector<double>>, double> gradientAndValue =
            compiledFunc->differentiate(make_shared<vector<double>>(point));

    double eval = gradientAndValue.second;
    shared_ptr<vector<double>> gradient = gradientAndValue.first;

    ASSERT_NEAR(-20.0855369231877, eval, 10E-10);
    ASSERT_NEAR(0, gradient->at(0), 10E-10);
    ASSERT_NEAR(40.1710738463753, gradient->at(1), 10E-10);
}

TEST(AutoDiffTest, UNCOMPILED) {
    // we will use a function of two variables
    VarPtr x = make_shared<Variable>();
    VarPtr y = make_shared<Variable>();

    // Define our function: func(x, y) = (x + y) * exp(x - y)
    TermPtr func = (x + y) * TermBuilder::exp(x - y);

    // define the ordering of variables, and a point where
    // the function will be evaluated/differentiated
    vector<VarPtr> vars{x, y};
    vector<double> point{1, -2};

    // calculate the value and the gradient at the point (x, y) = (1, -2)
    double eval = TermUtils::evaluate(
            func, make_shared<vector<VarPtr>>(vars), make_shared<vector<double>>(point));
    shared_ptr<vector<double>> gradient = TermUtils::differentiate(
            func, make_shared<vector<VarPtr>>(vars), make_shared<vector<double>>(point));

    ASSERT_NEAR(-20.0855369231877, eval, 10E-10);
    ASSERT_NEAR(0, gradient->at(0), 10E-10);
    ASSERT_NEAR(40.1710738463753, gradient->at(1), 10E-10);
}
*/
// Run all the tests that were declared with TEST()
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
