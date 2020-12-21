// half - IEEE 754-based half-precision floating point library.
//
// Copyright (c) 2012-2017 Christian Rau <rauy@users.sourceforge.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// This code has been adapted from half library found in https://github.com/suruoxi/half

#pragma once

#include <cstdint>
#include <limits>

template <std::float_round_style Rounding = std::round_to_nearest>
uint16_t fp32_to_fp16(float value)
{
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));

    uint16_t hbits = (bits >> 16) & 0x8000;
    bits &= 0x7FFFFFFF;

    uint32_t exp = bits >> 23;
    if (exp == 255) {
        return hbits | 0x7C00 | (0x3FF & -static_cast<uint32_t>((bits & 0x7FFFFF) != 0));
    }
    if (exp > 142) {
        if (Rounding == std::round_toward_infinity) {
            return (hbits | 0x7C00) - (hbits >> 15);
        }
        if (Rounding == std::round_toward_neg_infinity) {
            return (hbits | 0x7BFF) + (hbits >> 15);
        }
        return (hbits | 0x7BFF) + (Rounding != std::round_toward_zero ? 1 : 0);
    }
    uint32_t g, s;
    if (exp > 112) {
        g = (bits >> 12) & 1;
        s = (bits & 0xFFF) != 0;
        hbits |= ((exp - 112) << 10) | ((bits >> 13) & 0x3FF);
    } else if (exp > 101) {
        int i = 125 - exp;
        bits = (bits & 0x7FFFFF) | 0x800000;
        g = (bits >> i) & 1;
        s = (bits & ((1L << i) - 1)) != 0;
        hbits |= bits >> (i + 1);
    } else {
        g = 0;
        s = bits != 0;
    }
    if constexpr (Rounding == std::round_to_nearest) {
        hbits += g & (s | hbits);
    } else if constexpr (Rounding == std::round_toward_infinity) {
        hbits += ~(hbits >> 15) & (s | g);
    } else if constexpr (Rounding == std::round_toward_neg_infinity) {
        hbits += (hbits >> 15) & (g | s);
    }
    return hbits;
}
