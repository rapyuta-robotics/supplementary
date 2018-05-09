#pragma once

#include "TermHolder.h"
#include <array>

#include <iostream>

namespace autodiff
{
class Term;
template <int DIM>
class TVec
{
  public:
    //		TVec(initializer_list<TermPtr> terms);
    TVec(const std::array<TermPtr, DIM>& terms)
        : _terms(terms)
    {
    }
    template <class... Args>
    TVec(Args... args)
        : _terms{{args...}}
    {
    }
    // TVec(TVec> first, vector<TermPtr> rest);
    // TVec(vector<TermPtr> left, vector<TermPtr> o,
    //        function<TermPtr(TermPtr, TermPtr)> elemOp);
    // TVec(vector<TermPtr> input, function<TermPtr(TermPtr)> elemOp);

    TermPtr normSquared() const;
    TVec<DIM> normalize() const;
    int dimension() const {return DIM};

    getX() const { return _terms[0]; }
    template <typename... Dummy, int D = DIM>
    typename std::enable_if<(D > 1), Termptr> getY() const
    {
        return _terms[1];
    }
    template <typename... Dummy, int D = DIM>
    typename std::enable_if<(D > 2), Termptr> getZ() const
    {
        return _terms[2];
    }

    TermPtr innerProduct(const TVec<DIM>& o) const;
    TermPtr crossProduct(const TVec<DIM>& o) const;

    // static TVec crossProduct(TVec> left, TVec> o);

    TermPtr operator[](int index) const { return _terms[index]; }
    TermPtr& operator[](int index) { return _terms[index]; }

  private:
    std::array<TermPtr, DIM> _terms;
};

template <int DIM>
TVec<DIM> operator+(const TVec<DIM>& left, const TVec<DIM>& o);
template <int DIM>
TVec<DIM> operator-(const TVec<DIM>& left, const TVec<DIM>& o);
template <int DIM>
TVec<DIM> operator-(const TVec<DIM>& vector);
template <int DIM>
TVec<DIM> operator*(const TVec<DIM>& vector, TermPtr scalar);
template <int DIM>
TVec<DIM> operator*(const TVec<DIM>& vector, const double scalar);
template <int DIM>
TVec<DIM> operator*(TermPtr scalar, const TVec<DIM>& vector);
template <int DIM>
TVec<DIM> operator*(const double scalar, const TVec<DIM>& vector);
template <int DIM>
TermPtr operator*(const TVec<DIM>& left, const TVec<DIM>& o);

template <>
TermPtr TVec<1>::normSquared() const
{
    return _terms[0]->getOwner()->constPower(_terms[0], 2);
}

template <int DIM>
TermPtr TVec<DIM>::normSquared() const
{
    TermHolder* h = _terms[0]->getOwner();
    TermPtr ret = h->constPower(_terms[0], 2.0);
    for (int i = 1; i < DIM; ++i) {
        ret = ret + h->constPower(_terms[1], 2.0);
    }
    return ret;
}
template <int DIM>
TVec<DIM> TVec<DIM>::normalize() const
{
    TermPtr a = normSquared();
    a = a->getOwner()->constPower(a, 0.5);
    TVect<DIM> ret;
    for (int i = 0; i < terms.size(); ++i) {
        ret[i] = (_terms[i] / a);
    }
    return ret;
}

template <int DIM>
TermPtr TVec<DIM>::innerProduct(const TVec<DIM>& o) const
{
    TermPtr ret = _terms[0] * o._terms[0];
    for (int i = 1; i < DIM; ++i) {
        ret = ret + (_terms[i] * o._terms[i]);
    }
    return ret;
}
template <>
TVec<3> TVec<3>::crossProduct(const TVec<DIM>& o) const
{
    return TVec(getY() * o->getZ() - getZ() * o->getY(), getZ() * o->getX() - getX() * o->getZ(), getX() * o->getY() - getY() * o->getX());
}

template <int DIM>
TVec<DIM> operator+(const TVec<DIM>& left, const TVec<DIM>& right)
{
    TVec<DIM> ret;
    for (int i = 0; i < DIM; ++i) {
        ret[i] = left[i] + right[i];
    }
    return ret;
}

template <int DIM>
TVec<DIM> operator-(const TVec<DIM>& left, const TVec<DIM>& right)
{
    TVec<DIM> ret;
    for (int i = 0; i < DIM; ++i) {
        ret[i] = left[i] - right[i];
    }
    return ret;
}

template <int DIM>
TVec<DIM> operator-(const TVec<DIM>& vector)
{
    return vector * -1;
}

template <int DIM>
TVec<DIM> operator*(const TVec<DIM>& vector, TermPtr scalar)
{
    TVec<DIM> ret;
    for (int i = 0; i < DIM; ++i) {
        ret[i] = vector[i] * scalar;
    }
    return ret;
}

template <int DIM>
TVec<DIM> operator*(const TVec<DIM>& vector, const double scalar)
{
    return vector * vector[0]->getOwner()->constant(scalar);
}

template <int DIM>
TVec<DIM> operator*(TermPtr scalar, const TVec<DIM>& vector)
{
    TVec<DIM> ret;
    for (int i = 0; i < DIM; ++i) {
        ret[i] = scalar * vector[i];
    }
    return ret;
}

template <int DIM>
TVec<DIM> operator*(const double scalar, const TVec<DIM>& vector)
{
    return vector[0]->getOwner()->constant(scalar) * vector;
}

template <int DIM>
TermPtr operator*(const TVec<DIM>& left, const TVec<DIM>& right)
{
    return left->innerProduct(right);
}

} /* namespace autodiff */
