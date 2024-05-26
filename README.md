# safeMultiplyInt64
This repository demonstrates short portable C++ code for computing the
product of two 64-bit integers and checking whether overflow ocurred,
*without using any division or branching*. Hence the approach is
desirable for superscalar processors.

The code computes the product $x \times y$ two ways:

* Using 64-bit modulo arithmetic.

* Using floating-point.

It then checks the absolute difference of the results, which will be significantly
larger if overflow happens.

For the case of signed arithmetic:

* When overflow does not occur, the two products will differ by not more than
  about $3 \times 2^{63-23}$.

* When overflow occurs, the two products will differ by at least about $2^{63}$,
  because bits from the high-order 64 bits of the 128-bit product are missing.

Hence a threshold chosen somewhere in between suffices to distinguish whether
overlow happened.

The case of unsigned arithmetic is similar, except that the differences cited
above can be twice as high, owing to the factors having 64 bits instead of 63
bits for their magnitudes.

See header `safeMultiplyInt64.h` for details.

