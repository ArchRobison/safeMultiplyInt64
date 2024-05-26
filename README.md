# safeMultiplyInt64
This repository demonstrates short portable C++ code for computing the
product of two 64-bit signed integers and whether overflow ocurred,
*without using any division* and no branching. Hence the approach is
desirable for superscalar processors.

The code computes the product $x \times y$ two ways:

* Using 64-bit modulo arithmetic.

* Using floating-point.

It then checks the absolute difference of the results, which will be significantly
larger if overflow happened.

* When overflow does not occur, the two products will differ by not more than
  about $3 \times 2^{23}$.

* When overflow occurs, the two products will differ by at least about $2^{63}$.

Hence a threshold somewhere in between suffices to distinguish whether
overlow happened.

See header `safeMultiplyInt64.h` for details.

