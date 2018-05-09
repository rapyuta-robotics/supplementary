#pragma once

#include "Term.h"

namespace autodiff
{

class Product : public Term
{
  public:
    int accept(ITermVisitor* visitor) override;

    TermPtr aggregateConstants() override;
    TermPtr derivative(VarPtr v) const override;

    std::string toString() const override;

  private:
    friend TermHolder;
    Product(TermPtr left, TermPtr right, TermHolder* owner);

    TermPtr _left;
    TermPtr _right;
};

} /* namespace autodiff */

#endif /* PRODUCT_H_ */
