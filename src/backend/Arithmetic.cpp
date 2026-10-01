#include "backend/Arithmetic.h"

using namespace registers;

namespace {

unsigned long long magnitude(long long value) {
    unsigned long long bits = static_cast<unsigned long long>(value);
    return value < 0 ? 0 - bits : bits;
}

int bitLength(unsigned long long value) {
    return value == 0 ? 0 : 64 - __builtin_clzll(value);
}

// a = a * 2^exponent, by repeated ADD a or by one SHIFT, whichever is cheaper. Clobbers c.
void multiplyByPowerOfTwo(Code& code, int exponent) {
    long long byAdd = exponent * cost(Opcode::ADD);
    long long byShift = cost(Opcode::RESET) + exponent * cost(Opcode::INC) + cost(Opcode::SHIFT);

    if (byAdd <= byShift) {
        for (int i = 0; i < exponent; i++) {
            code.add(A);
        }
    }
    else {
        code.reset(C);
        code.inc(C, exponent);
        code.shift(C);
    }
}

// Shared by divide and modulo. Computes |x| / |y| by binary long division; the caller then
// fixes the signs so that the quotient is rounded down. Jumps to `end` with a = 0 when
// dividing by zero. Afterwards:
//
//   b - remainder |x| mod |y|      f - constant -1 (shift amount for halving)
//   c - divisor |y|                g - sign code: 1 if x < 0, plus 2 if y < 0
//   d - quotient |x| / |y|         h - constant 1 (shift amount for doubling)
//   e - loop counter
void divideMagnitudes(Code& code, Label end) {
    Label xReady = code.newLabel(), yReady = code.newLabel();
    Label grow = code.newLabel(), grown = code.newLabel(), shrink = code.newLabel();
    Label skipSubtraction = code.newLabel(), done = code.newLabel();

    // b = |x|
    code.reset(G);
    code.jpos(xReady);
    code.jzero(xReady);
    code.swap(B);
    code.reset(A);
    code.sub(B);
    code.inc(G);
    code.bind(xReady);
    code.swap(B);

    // c = |y|
    code.reset(A);
    code.add(C);
    code.jzero(end);
    code.jpos(yReady);
    code.reset(A);
    code.sub(C);
    code.inc(G);
    code.inc(G);
    code.bind(yReady);
    code.swap(C);

    code.reset(F);
    code.dec(F);
    code.reset(H);
    code.inc(H);

    // double c until it exceeds |x|, counting the doublings in e; a tracks c - |x|
    code.reset(E);
    code.reset(A);
    code.add(C);
    code.sub(B);
    code.bind(grow);
    code.jpos(grown);
    code.shift(H);
    code.add(B);            // c - r  ->  2c - r
    code.inc(E);
    code.jump(grow);
    code.bind(grown);
    code.add(B);
    code.swap(C);

    // halve c as many times; each halving yields one bit of the quotient
    code.reset(D);
    code.swap(E);           // a = number of halvings left
    code.bind(shrink);
    code.jzero(done);
    code.dec(A);
    code.swap(E);
    code.swap(C);
    code.shift(F);
    code.swap(C);           // c /= 2
    code.swap(D);
    code.shift(H);
    code.swap(D);           // quotient *= 2
    code.reset(A);
    code.add(B);
    code.sub(C);            // a = remainder - c
    code.jneg(skipSubtraction);
    code.swap(B);           // remainder -= c
    code.inc(D);            // quotient += 1
    code.bind(skipSubtraction);
    code.swap(E);
    code.jump(shrink);

    code.bind(done);
}

}

namespace arithmetic {

void negate(Code& code) {
    code.swap(B);
    code.reset(A);
    code.sub(B);
}

// Shift-and-add over the bits of the smaller factor (in magnitude):
//   b - the larger factor, doubled every step      f - constant -1
//   c - the smaller factor, halved every step      g - number of negative factors
//   d - product                                    h - constant 1
void multiply(Code& code) {
    Label xReady = code.newLabel(), yReady = code.newLabel(), ordered = code.newLabel();
    Label loop = code.newLabel(), even = code.newLabel(), done = code.newLabel();
    Label negative = code.newLabel(), end = code.newLabel();

    code.swap(B);
    code.reset(G);

    // b = |x|; a product with 0 is 0
    code.reset(A);
    code.add(B);
    code.jzero(end);
    code.jpos(xReady);
    code.reset(A);
    code.sub(B);
    code.swap(B);
    code.inc(G);
    code.bind(xReady);

    // c = |y|
    code.reset(A);
    code.add(C);
    code.jzero(end);
    code.jpos(yReady);
    code.reset(A);
    code.sub(C);
    code.swap(C);
    code.inc(G);
    code.bind(yReady);

    // make c the smaller factor
    code.reset(A);
    code.add(C);
    code.sub(B);
    code.jneg(ordered);
    code.jzero(ordered);
    code.add(B);
    code.swap(B);
    code.swap(C);
    code.bind(ordered);

    code.reset(D);
    code.reset(F);
    code.dec(F);
    code.reset(H);
    code.inc(H);

    code.bind(loop);
    code.reset(A);
    code.add(C);
    code.jzero(done);
    code.shift(F);
    code.swap(C);           // c /= 2, a = old c
    code.sub(C);
    code.sub(C);            // a = lowest bit of old c
    code.jzero(even);
    code.swap(D);
    code.add(B);
    code.swap(D);           // product += b
    code.bind(even);
    code.swap(B);
    code.shift(H);
    code.swap(B);           // b *= 2
    code.jump(loop);

    code.bind(done);
    code.reset(A);
    code.add(G);
    code.dec(A);
    code.jzero(negative);   // exactly one factor was negative
    code.swap(D);
    code.jump(end);
    code.bind(negative);
    code.reset(A);
    code.sub(D);
    code.bind(end);
}

// Doubles and adds along the binary representation of |factor|.
void multiplyByConstant(Code& code, long long factor) {
    if (factor == 0) {
        code.reset(A);
        return;
    }

    const unsigned long long m = magnitude(factor);
    const int length = bitLength(m);

    if ((m & (m - 1)) == 0) {
        multiplyByPowerOfTwo(code, length - 1);
    }
    else {
        code.swap(D);
        code.reset(A);
        code.add(D);        // a = d = x
        code.reset(C);
        code.inc(C);        // c = 1

        for (int bit = length - 2; bit >= 0; bit--) {
            code.shift(C);
            if ((m >> bit) & 1) {
                code.add(D);
            }
        }
    }

    if (factor < 0) {
        negate(code);
    }
}

void divide(Code& code) {
    Label quotient = code.newLabel(), negateQuotient = code.newLabel(), end = code.newLabel();

    divideMagnitudes(code, end);

    code.reset(A);
    code.add(G);
    code.jzero(quotient);   // both non-negative
    code.dec(A, 3);
    code.jzero(quotient);   // both negative
    code.reset(A);          // signs differ: -(q + 1) if there is a remainder, -q otherwise
    code.add(B);
    code.jzero(negateQuotient);
    code.inc(D);
    code.bind(negateQuotient);
    code.reset(A);
    code.sub(D);
    code.jump(end);
    code.bind(quotient);
    code.swap(D);
    code.bind(end);
}

void modulo(Code& code) {
    Label remainder = code.newLabel(), divisorMinusRemainder = code.newLabel();
    Label remainderMinusDivisor = code.newLabel(), end = code.newLabel();

    divideMagnitudes(code, end);

    code.reset(A);
    code.add(B);
    code.jzero(end);        // no remainder
    code.reset(A);
    code.add(G);
    code.jzero(remainder);                                              // x >= 0, y > 0:  r
    code.dec(A);
    code.jzero(divisorMinusRemainder);                                  // x < 0,  y > 0:  |y| - r
    code.dec(A);
    code.jzero(remainderMinusDivisor);                                  // x >= 0, y < 0:  r - |y|
    code.reset(A);                                                      // x < 0,  y < 0:  -r
    code.sub(B);
    code.jump(end);
    code.bind(remainder);
    code.swap(B);
    code.jump(end);
    code.bind(divisorMinusRemainder);
    code.reset(A);
    code.add(C);
    code.sub(B);
    code.jump(end);
    code.bind(remainderMinusDivisor);
    code.reset(A);
    code.add(B);
    code.sub(C);
    code.bind(end);
}

// SHIFT by a negative amount rounds toward minus infinity, exactly like DIV.
void divideByPowerOfTwo(Code& code, int exponent) {
    code.reset(C);
    code.dec(C, exponent);
    code.shift(C);
}

// x mod 2^k = x - (x / 2^k) * 2^k
void moduloByPowerOfTwo(Code& code, int exponent) {
    code.swap(D);
    code.reset(A);
    code.add(D);            // a = d = x
    code.reset(C);
    code.dec(C, exponent);
    code.shift(C);
    code.reset(C);
    code.inc(C, exponent);
    code.shift(C);          // a = (x / 2^k) * 2^k
    code.swap(D);
    code.sub(D);
}

}
