/*
 * Term.h
 *
 *  Created on: Jun 5, 2014
 *      Author: psp
 */

#ifndef TERM_H_
#define TERM_H_

#include "ITermVisitor.h"
#include "TermPtr.h"
#include "Types.h"
#include <engine/constraintmodul/SolverTerm.h>

#include <string>
#include <vector>

namespace autodiff
{

class TermHolder;
class TermList;

enum AndType
{
    MIN,
    AND
};
enum OrType
{
    MAX,
    OR
};

class Term /*: public alica::SolverTerm */
{
  public:
    virtual ~Term();

    /**
     * Accepts a term visitor
     *
     * @param visitor The term visitor to accept
     */
    virtual void acceptRecursive(ITermVisitor* visitor) = 0;
    virtual int accept(ITermVisitor* visitor) = 0;

    int getId() const { return _id; }

    static AndType getAnd();
    static void setAnd(AndType a);
    static OrType getOr();
    static void setOr(OrType o);
    static inline constexpr double getConstraintSteepness() { return CONSTRAINTSTEEPNESS; }

    virtual TermPtr aggregateConstants() = 0;
    virtual TermPtr derivative(VarPtr v) const = 0;
    virtual TermPtr negate() const;

    virtual std::string toString() const = 0;
    virtual bool isConstant() const { return false; }

    TermHolder* getOwner() const { return _owner; }

    void setTapeIdx(int idx) { _tapeIdx = idx; }
    int getTapeIdx() const { return _tapeIdx; }
    virtual EvalFunction getEvalFunction() const = 0;
    virtual void fillParameters(Parameter* params) const = 0;

    double getMax() const { return _max; }
    double getMin() const { return _min; }

    void setMax(double m) { _max = m; }
    void setMin(double m) { _min = m; }

    const std::vector<TermPtr>& getParents() const { return _parents; }
    std::vector<TermPtr>& editParents() { return _parents; }

  protected:
    static constexpr double EPSILON = 1e-10;
    static constexpr double CONSTRAINTSTEEPNESS = 0.01;
    friend TermList;
    Term(TermHolder* owner);

    TermHolder* _owner;

    TermPtr _next;

    // Interval propagation:
    std::vector<TermPtr> _parents;
    double _min;
    double _max;

    int _tapeIdx;

  private:
    const int _id;

    static int _nextId;
    static OrType _orop;
    static AndType _andop;
};

TermPtr operator+(const TermPtr left, const TermPtr right);
TermPtr operator*(const TermPtr left, const TermPtr right);
TermPtr operator/(const TermPtr numerator, const TermPtr denominator);
TermPtr operator-(const TermPtr left, const TermPtr right);

TermPtr operator+(const double left, const TermPtr right);
TermPtr operator*(const double left, const TermPtr right);
TermPtr operator/(const double numerator, const TermPtr denominator);
TermPtr operator-(const double left, const TermPtr right);

TermPtr operator+(const TermPtr left, const double right);
TermPtr operator*(const TermPtr left, const double right);
TermPtr operator/(const TermPtr numerator, const double denominator);
TermPtr operator-(const TermPtr left, const double right);

TermPtr operator-(const TermPtr term);

TermPtr operator!(const TermPtr term);
TermPtr operator&(const TermPtr left, const TermPtr right);
TermPtr operator|(const TermPtr left, const TermPtr right);

TermPtr operator>(const TermPtr left, const TermPtr right);
TermPtr operator<(const TermPtr left, const TermPtr right);
TermPtr operator<=(const TermPtr left, const TermPtr right);
TermPtr operator>=(const TermPtr left, const TermPtr right);

} /* namespace autodiff */

#endif /* TERM_H_ */
