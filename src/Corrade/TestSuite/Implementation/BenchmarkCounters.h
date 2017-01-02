#ifndef Corrade_TestSuite_Implementation_BenchmarkCounters_h
#define Corrade_TestSuite_Implementation_BenchmarkCounters_h
/*
    This file is part of Corrade.

    Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <cstdint>

/* For wall clock */
#include <chrono>

/* For RDTSC */
#ifdef CORRADE_TARGET_X86
#ifdef __GNUC__
#include <x86intrin.h>
#elif defined(_MSC_VER)
#include <intrin.h>
#endif
#endif

#include "Corrade/configure.h"

namespace Corrade { namespace TestSuite { namespace Implementation {

/* Wall clock in nanoseconds */
inline std::uint64_t wallClock() {
    /* OH GOD WHY SO COMPLICATED */
    return  std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

/* RDTSC */
inline std::uint64_t rdtsc() {
    #ifdef CORRADE_TARGET_X86
    return __rdtsc();
    #else
    return 0;
    #endif
}

}}}

#endif
