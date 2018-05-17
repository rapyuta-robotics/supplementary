#pragma once

#include <engine/AlicaClock.h>

#include <fstream>
#include <list>
#include <memory>
#include <vector>

#include <autodiff/Types.h>

namespace alica
{

namespace reasoner
{
namespace intervalpropagation
{
class IntervalPropagator;
} // namespace intervalpropagation

namespace cnsat
{
class CNSat;
class FormulaTransform;
class Var;
} // namespace cnsat

class CNSMTGSolver
{
  protected:
    class RpropResult;

  public:
    CNSMTGSolver();
    AlicaTime getTime();
    virtual ~CNSMTGSolver();

    void initLog();
    void log(double util, shared_ptr<vector<double>>& val);
    void logStep();
    void closeLog();
    void solve(autodiff::TermPtr equation, autodiff::TermHolder& holder, const std::vector<double>& limits, double& out_util,
               std::vector<double>& out_solution);
    void solve(autodiff::TermPtr equation, autodiff::TermHolder& holder, const std::vector<double>& limits, const std::vector<double>& seeds,
               double sufficientUtility, double& out_util, std::vector<double>& out_solution);
    void solveTest(autodiff::TermPtr equation, autodiff::TermHolder& holder, const std::vector<double>& limit, std::vector<double>& out_solution);
    bool intervalPropagate(std::shared_ptr<std::vector<std::shared_ptr<cnsat::Var>>> decisions,
                           std::shared_ptr<std::vector<std::shared_ptr<std::vector<double>>>>& curRanges);
    bool probeForSolution(std::shared_ptr<std::vector<std::shared_ptr<cnsat::Var>>> decisions, std::shared_ptr<std::vector<double>> solution);

    long getRuns();
    long getFEvals();
    double getRPropConvergenceStepSize();
    std::shared_ptr<cnsat::CNSat> getCNSatSolver();
    void setUseIntervalProp(bool useIntervalProp);

    double utilitySignificanceThreshold = 1E-22;
    int dim;
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<double>>>> limits;

    double utilityThreshold;
    AlicaTime maxSolveTime;
    AlicaTime begin;

    std::ofstream sw;

    std::vector<std::shared_ptr<RpropResult>> rResults;

    long maxfevals;
    double initialStepSize = 0.005;
    bool optimize;

  protected:
    std::shared_ptr<RpropResult> rPropFindFeasible(std::shared_ptr<std::vector<std::shared_ptr<cnsat::Var>>> constraints,
                                                   std::shared_ptr<std::vector<double>> seed);
    std::shared_ptr<RpropResult> rPropOptimizeFeasible(std::shared_ptr<std::vector<std::shared_ptr<cnsat::Var>>> constraints,
                                                       std::shared_ptr<autodiff::Term> ut,
                                                       std::shared_ptr<std::vector<std::shared_ptr<autodiff::Variable>>> args,
                                                       std::shared_ptr<std::vector<double>>& seed, bool precise);
    void differentiate(std::shared_ptr<std::vector<std::shared_ptr<cnsat::Var>>> constraints, std::shared_ptr<vector<double>>& val,
                       std::shared_ptr<std::vector<double>>& gradient, double& util);
    std::shared_ptr<vector<double>> initialPointFromSeed(std::shared_ptr<vector<std::shared_ptr<cnsat::Var>>> constraints, std::shared_ptr<RpropResult> res,
                                                         std::shared_ptr<vector<double>>& seed);
    std::shared_ptr<vector<double>> initialPoint(std::shared_ptr<vector<shared_ptr<cnsat::Var>>> constraints, std::shared_ptr<RpropResult> res);
    void initializeStepSize();

    long runs;
    long fevals;
    double rPropConvergenceStepSize;

    static int fcounter;
    bool seedWithUtilOptimum;
    shared_ptr<cnsat::CNSat> ss;

    std::shared_ptr<cnsat::FormulaTransform> ft;
    std::shared_ptr<intervalpropagation::IntervalPropagator> ip;
    std::shared_ptr<vector<double>> lastSeed;

    std::shared_ptr<RpropResult> r1 = nullptr;

    int probeCount = 0;
    int successProbeCount = 0;
    int intervalCount = 0;
    int successIntervalCount = 0;
    int fevalsCount = 0;
    int runCount = 0;

    std::vector<double> ranges;
    std::vector<double> rpropStepWidth;
    std::vector<double> rpropStepConvergenceThreshold;
    std::shared_ptr<std::vector<std::shared_ptr<autodiff::Variable>>> currentArgs;

    class RpropResult : public enable_shared_from_this<RpropResult>
    {
      public:
        std::shared_ptr<std::vector<double>> initialValue;
        std::shared_ptr<std::vector<double>> finalValue;
        double initialUtil;
        double finalUtil;
        bool aborted;

        int compareTo(std::shared_ptr<RpropResult> other);
    };

  private:
    bool useIntervalProp;
};

} /* namespace reasoner */
} /* namespace alica */
