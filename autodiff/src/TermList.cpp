#include <TermList.h>

#include <Term.h>

#include <assert.h>

namespace autodiff
{

TermList::TermList()
    : _first(nullptr)
    , _last(nullptr)
    , _size(0)
{
}

bool TermList::contains(const TermPtr t) const
{
    if (t->_next != nullptr || t->_prev != nullptr) {
        return true;
    }
    return t == _first;
}

TermPtr TermList::dequeue()
{
    TermPtr ret = _first;
    if (ret == nullptr) {
        return ret;
    }
    --_size;
    _first = ret->_next;

    if (_first != nullptr) {
        _first->_prev = nullptr;
    }

    ret->_next = nullptr;
    ret->_prev = nullptr;

    if (ret == _last) {
        _last = nullptr;
        _first = nullptr;
    }
    return ret;
}

void TermList::enqueue(TermPtr t)
{
    assert(t->_next == nullptr);
    assert(t->_prev == nullptr);
    ++_size;
    if (_first == nullptr) {
        _first = t;
        _last = t;
        return;
    }
    _last->_next = t;
    t->_prev = _last;
    _last = t;
}

void TermList::clear()
{
    TermPtr cur = _first;
    TermPtr next = nullptr;
    while (cur != nullptr) {
        cur->_prev = nullptr;
        next = cur->_next;
        cur->_next = nullptr;
        cur = next;
    }
    _first = nullptr;
    _last = nullptr;
    _size = 0;
}

} /* namespace autodiff */
