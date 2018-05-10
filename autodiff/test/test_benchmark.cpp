#include "AutoDiff.h"

#include <chrono>
#include <gtest/gtest.h>
#include <math.h>
#include <random>
using namespace autodiff;

double sampleTerm(TermPtr t, std::shared_ptr<std::vector<VarPtr>>& vars, int num, std::mt19937& gen)
{
    double ret = 0.0f;
    std::uniform_real_distribution<> doubleDistri(-1.0, 1.0);
    std::shared_ptr<std::vector<double>> point = std::make_shared<std::vector<double>>(vars->size());
    for (int i = 0; i < num; ++i) {
        for (int j = vars->size() - 1; j >= 0; --j) {
            (*point)[j] = doubleDistri(gen);
        }
        ret += TermUtils::evaluate(t, vars, point);
    }
    return ret / num;
}

std::shared_ptr<Term> makeTest(int64_t seed, std::shared_ptr<std::vector<VarPtr>>& vars, int dim, int numLits, int numClauses)
{
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> doubleDistri(-1.0, 1.0);
    std::uniform_int_distribution<> dimDistri(0, dim - 1);

    vars = std::make_shared<std::vector<VarPtr>>(dim);
    // generate variables
    for (VarPtr& vp : *vars) {
        vp = std::make_shared<Variable>();
    }
    // generate literals:
    std::vector<TermPtr> lits;
    lits.reserve(numLits);
    for (int l = 0; l < numLits; ++l) {
        TermPtr sum = nullptr;
        double k = 0.0;
        for (int i = 0; i < 3; ++i) {
            TermPtr product = nullptr;
            double ks = 1.0;
            for (int j = 0; j < 3; ++j) {
                double a = doubleDistri(gen);
                int idx = dimDistri(gen);
                double b = doubleDistri(gen) * 2.0 * M_PI;
                TermPtr t = TermBuilder::constant(a) * TermBuilder::sin((*vars)[idx] * TermBuilder::constant(2.0 * M_PI) + TermBuilder::constant(b));
                if (product.get() == nullptr) {
                    product = t;
                } else {
                    product = product * t;
                }
                ks *= a;
            }
            if (sum.get() == nullptr) {
                sum = product;
            } else {
                sum = sum + product;
            }
            k += ks;
        }
        TermPtr lit = TermBuilder::constant(1.0 / k) * sum;

        double theta = sampleTerm(lit, vars, 100, gen);
        lits.push_back(lit < TermBuilder::constant(theta));
    }
    std::uniform_int_distribution<> litDistri(0, numLits - 1);
    // generate clauses:
    TermPtr sat = nullptr;
    for (int i = 0; i < numClauses; ++i) {
        TermPtr clause = nullptr;
        for (int j = 0; j < 3; ++j) {
            TermPtr lit = lits[litDistri(gen)];
            if (litDistri(gen) > numLits / 2) {
                lit = lit->negate();
            }
            if (clause.get() == nullptr) {
                clause = lit;
            } else {
                clause = clause & lit;
            }
        }
        if (sat.get() == nullptr) {
            sat = clause;
        } else {
            sat = sat | clause;
        }
    }
    return sat;
}
double toMs(int64_t t1, int64_t t2)
{
    return (t2 - t1) / (1000.0 * 1000.0);
}
// Run all the tests that were declared with TEST()
int main(int argc, char** argv)
{
    if (argc < 5) {
        std::cout << "Usage: test numRuns dimension literals clauses [seed]" << std::endl;
        return 1;
    }
    Term::setAnd(AndType::AND);
    Term::setOr(OrType::MAX);
    // std::cout << argv[0] <<std::endl;
    int numRuns = std::stoi(argv[1]);
    int numDims = std::stoi(argv[2]);
    int numLits = std::stoi(argv[3]);
    int numClauses = std::stoi(argv[4]);
    int seed = 0;
    if (argc > 5) {
        seed = std::stoi(argv[5]);
    }
    std::cout << std::fixed;
    std::cout << std::setprecision(2) << std::endl;
    std::cout << std::setprecision(2) << std::endl;
    std::cout << "Performing " << numRuns << " runs with " << numDims << " dimensions " << numLits << " literals " << numClauses << " clauses." << std::endl;
    std::cout << "Seed is: " << seed << std::endl;

    std::shared_ptr<std::vector<VarPtr>> vars;
    int64_t t1 = (std::chrono::system_clock::now().time_since_epoch().count());
    TermPtr term = makeTest(seed, vars, numDims, numLits, numClauses);
    int64_t t2 = (std::chrono::system_clock::now().time_since_epoch().count());
    std::cout << "Done generating in " << toMs(t1, t2) << "ms" << std::endl;
    shared_ptr<ICompiledTerm> compiled = TermUtils::compile(term, vars);
    t1 = (std::chrono::system_clock::now().time_since_epoch().count());
    std::cout << "Done compiling in " << toMs(t2, t1) << "ms" << std::endl;
    // std::cout << "Term: "<<term->toString()<<std::endl;
    std::shared_ptr<std::vector<double>> point = std::make_shared<std::vector<double>>(numDims);
    std::pair<std::shared_ptr<std::vector<double>>, double> result;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> doubleDistri(-1.0, 1.0);

    t1 = (std::chrono::system_clock::now().time_since_epoch().count());

    for (int i = numRuns - 1; i >= 0; --i) {
        for (int d = numDims - 1; d >= 0; --d) {
            (*point)[d] = doubleDistri(gen);
        }
        result = compiled->differentiate(point);
    }
    t2 = (std::chrono::system_clock::now().time_since_epoch().count());
    std::cout << "Done in " << toMs(t1, t2) << "ms" << std::endl;
    std::cout << numRuns / toMs(t1, t2) * 1000.0 << " evaluations per second" << std::endl;
    std::cout << "Last value: " << result.second << std::endl;
    return 0;
}
