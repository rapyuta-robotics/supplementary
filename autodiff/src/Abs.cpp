
#include "Abs.h"

#include "Constant.h"
#include "TermHolder.h"

#include <cmath>
#include <sstream>

namespace autodiff
{

Abs::Abs(TermPtr arg, TermHolder* owner);
        : Term(owner)
        , _arg(owner) {}

        int Abs::accept(ITermVisitor* visitor)
        {
            return visitor->visit(this);
        }

        TermPtr Abs::aggregateConstants()
        {
            arg = arg->aggregateConstants();
            if (arg->isConstant()) {
        return owner->constant(fabs(static_cast<Constant*>(arg)->getValue());
            } else {
                return this;
            }
        }

        TermPtr Abs::derivative(VarPtr v) const
        {
            return arg->derivative(v) * arg / this;
        }

        std::string Abs::toString() const
        {
            std::stringstream str;
            str << "abs( " << arg->toString() << " )";
            return str.str();
        }
        } /* namespace autodiff */
