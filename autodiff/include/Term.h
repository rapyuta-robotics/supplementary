/*
 * Term.h
 *
 *  Created on: Jun 5, 2014
 *      Author: psp
 */

#ifndef TERM_H_
#define TERM_H_

#include "ITermVisitor.h"
#include "Types.h"
#include <engine/constraintmodul/SolverTerm.h>

#include <string>
#include <vector>

namespace autodiff
{

class TermHolder;

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
    virtual int accept(ITermVisitor* visitor) = 0;

    int getId() const { return _id; }

    static AndType getAnd();
    static void setAnd(AndType a);
    static OrType getOr();
    static void setOr(OrType o);
    static double getConstraintSteepness();
    static void setConstraintSteepness(double constraintSteepness);

    virtual TermPtr aggregateConstants() = 0;
    virtual TermPtr derivative(VarPtr v) const = 0;
    virtual TermPtr negate() const;

    virtual std::string toString() const = 0;
    virtual bool isConstant() const { return false; }

    TermHolder* getOwner() const { return _owner; }

  protected:
    Term(TermHolder* owner);

    TermHolder* _owner;
    TermPtr _prev;
    TermPtr _next;

    std::vector<TermPtr> _parents;
    double _min;
    double _max;

    static constexpr double EPSILON;

  private:
    const int _id;

    static int _nextId;
    static OrType _orop;
    static AndType _andop;
    static double _constraintSteepness;
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
