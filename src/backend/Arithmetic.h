#ifndef BACKEND_ARITHMETIC_H
#define BACKEND_ARITHMETIC_H

#include "backend/Code.h"

// Instruction sequences for operations the virtual machine has no instruction for.
// All of them leave the result in register a.
//
// Division rounds toward minus infinity and the remainder takes the sign of the divisor
// (as in Python); dividing by zero yields 0 for both the quotient and the remainder.
namespace arithmetic {

// a = -a. Clobbers b.
void negate(Code& code);

// a = a * c. Clobbers b, c, d, f, g, h.
void multiply(Code& code);

// a = a * factor. Clobbers b, c, d.
void multiplyByConstant(Code& code, long long factor);

// a = a / c. Clobbers b, c, d, e, f, g, h.
void divide(Code& code);

// a = a mod c. Clobbers b, c, d, e, f, g, h.
void modulo(Code& code);

// a = a / 2^exponent. Clobbers c.
void divideByPowerOfTwo(Code& code, int exponent);

// a = a mod 2^exponent. Clobbers c, d.
void moduloByPowerOfTwo(Code& code, int exponent);

}

#endif
