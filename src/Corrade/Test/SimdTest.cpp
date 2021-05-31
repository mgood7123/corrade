/*
    This file is part of Corrade.

    Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
                2017, 2018, 2019, 2020, 2021
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

#include <sstream>

#include "Corrade/Simd.h"
#include "Corrade/TestSuite/Tester.h"
#include "Corrade/Utility/DebugStl.h" /** @todo remove when <sstream> is gone */

namespace Corrade { namespace Test { namespace {

struct SimdTest: TestSuite::Tester {
    explicit SimdTest();

    /* Most of the operator tests is inherited from EnumSetTest, just replacing
       Feature::Fast with Simd::Sse2, Feature::Cheap with Simd::Sse3,
       Feature::Tested with Simd::Ssse3 and Feature::Popular with Simd::Sse41,
       and adjusting numeric values for inverse because there's no fullValue
       specified */

    void featuresConstructScalar();
    void featuresConstruct();
    void featuresOperatorOr();
    void featuresOperatorAnd();
    void featuresOperatorXor();
    void featuresOperatorBoolScalar();
    void featuresOperatorBool();
    void featuresOperatorInverse();
    void featuresCompare();

    void detectCompileTime();
    void detectRuntime();

    void tagDispatch();

    void debug();
    void debugPacked();
};

SimdTest::SimdTest() {
    addTests({&SimdTest::featuresConstructScalar,
              &SimdTest::featuresConstruct,
              &SimdTest::featuresOperatorOr,
              &SimdTest::featuresOperatorAnd,
              &SimdTest::featuresOperatorXor,
              &SimdTest::featuresOperatorBoolScalar,
              &SimdTest::featuresOperatorBool,
              &SimdTest::featuresOperatorInverse,
              &SimdTest::featuresCompare,

              &SimdTest::detectCompileTime,
              &SimdTest::detectRuntime,

              &SimdTest::tagDispatch,

              &SimdTest::debug,
              &SimdTest::debugPacked});
}

void SimdTest::featuresConstructScalar() {
    Simd::Features noFeatures = Simd::Scalar;
    constexpr Simd::Features cNoFeatures = Simd::Scalar;
    CORRADE_COMPARE(std::uint32_t(noFeatures), 0);
    CORRADE_COMPARE(std::uint32_t(cNoFeatures), 0);

    CORRADE_VERIFY(std::is_nothrow_constructible<Simd::Features, Simd::ScalarT>::value);
}

void SimdTest::featuresConstruct() {
    #ifdef CORRADE_TARGET_X86
    Simd::Features features = Simd::Sse3;
    constexpr Simd::Features cFeatures = Simd::Sse3;
    CORRADE_COMPARE(std::uint32_t(features), 2);
    CORRADE_COMPARE(std::uint32_t(cFeatures), 2);
    CORRADE_VERIFY(std::is_nothrow_constructible<Simd::Features, Simd::Sse3T>::value);
    #elif defined(CORRADE_TARGET_ARM)
    Simd::Features features = Simd::Neon;
    constexpr Simd::Features cFeatures = Simd::Neon;
    CORRADE_COMPARE(std::uint32_t(features), 1);
    CORRADE_COMPARE(std::uint32_t(cFeatures), 1);
    CORRADE_VERIFY(std::is_nothrow_constructible<Simd::Features, Simd::NeonT>::value);
    #elif defined(CORRADE_TARGET_WASM)
    Simd::Features features = Simd::Simd128;
    constexpr Simd::Features cFeatures = Simd::Simd128;
    CORRADE_COMPARE(std::uint32_t(features), 1);
    CORRADE_COMPARE(std::uint32_t(cFeatures), 1);
    CORRADE_VERIFY(std::is_nothrow_constructible<Simd::Features, Simd::Simd128T>::value);
    #else
    CORRADE_SKIP("No Simd tags available on this platform.");
    #endif
}

void SimdTest::featuresOperatorOr() {
    #ifdef CORRADE_TARGET_X86
    Simd::Features features = Simd::Sse3|Simd::Sse2;
    CORRADE_COMPARE(std::uint32_t(features), 3);

    CORRADE_COMPARE(std::uint32_t(features|Simd::Ssse3), 7);
    CORRADE_COMPARE(std::uint32_t(Simd::Ssse3|features), 7);

    features |= Simd::Ssse3;
    CORRADE_COMPARE(std::uint32_t(features), 7);

    constexpr Simd::Features cFeatures = Simd::Sse3|Simd::Sse2;
    constexpr Simd::Features cFeatures1 = cFeatures|Simd::Ssse3;
    constexpr Simd::Features cFeatures2 = Simd::Ssse3|cFeatures;
    CORRADE_COMPARE(std::uint32_t(cFeatures), 3);
    CORRADE_COMPARE(std::uint32_t(cFeatures1), 7);
    CORRADE_COMPARE(std::uint32_t(cFeatures2), 7);
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

void SimdTest::featuresOperatorAnd() {
    #ifdef CORRADE_TARGET_X86
    CORRADE_COMPARE(std::uint32_t(Simd::Sse3 & Simd::Sse2), 0);

    Simd::Features features = Simd::Sse41|Simd::Sse2|Simd::Sse3;
    CORRADE_COMPARE(std::uint32_t(features & Simd::Sse41), 8);
    CORRADE_COMPARE(std::uint32_t(Simd::Sse41 & features), 8);

    CORRADE_COMPARE(std::uint32_t(features & Simd::Ssse3), 0);

    Simd::Features features2 = Simd::Sse41|Simd::Sse2|Simd::Ssse3;
    CORRADE_COMPARE(std::uint32_t(features & features2), 9);

    features &= features2;
    CORRADE_COMPARE(std::uint32_t(features), 9);

    constexpr Simd::Features cFeatures = Simd::Sse41|Simd::Sse2|Simd::Sse3;
    constexpr Simd::Features cFeatures1 = cFeatures & Simd::Sse41;
    constexpr Simd::Features cFeatures2 = Simd::Sse41 & cFeatures;
    CORRADE_COMPARE(std::uint32_t(cFeatures1), 8);
    CORRADE_COMPARE(std::uint32_t(cFeatures2), 8);
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

void SimdTest::featuresOperatorXor() {
    #ifdef CORRADE_TARGET_X86
    CORRADE_COMPARE(std::uint32_t(Simd::Sse3 ^ Simd::Sse3), 0);
    CORRADE_COMPARE(std::uint32_t(Simd::Sse3 ^ Simd::Sse2), 3);

    Simd::Features features = Simd::Sse41|Simd::Sse2|Simd::Sse3;
    CORRADE_COMPARE(std::uint32_t(features ^ Simd::Ssse3), 15);
    CORRADE_COMPARE(std::uint32_t(Simd::Ssse3 ^ features), 15);

    CORRADE_COMPARE(std::uint32_t(features ^ Simd::Sse41), 3);

    Simd::Features features2 = Simd::Sse41|Simd::Sse2|Simd::Ssse3;
    CORRADE_COMPARE(std::uint32_t(features ^ features2), 6);

    features ^= features2;
    CORRADE_COMPARE(std::uint32_t(features), 6);

    constexpr Simd::Features cFeatures = Simd::Sse41|Simd::Sse2|Simd::Sse3;
    constexpr Simd::Features cFeatures1 = cFeatures ^ Simd::Ssse3;
    constexpr Simd::Features cFeatures2 = Simd::Ssse3 ^ cFeatures;
    CORRADE_COMPARE(std::uint32_t(cFeatures1), 15);
    CORRADE_COMPARE(std::uint32_t(cFeatures2), 15);
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

void SimdTest::featuresOperatorBoolScalar() {
    CORRADE_COMPARE(!!Simd::Features{Simd::Scalar}, false);

    constexpr bool cFeatures = !!Simd::Features{Simd::Scalar};
    CORRADE_VERIFY(!cFeatures);
}

void SimdTest::featuresOperatorBool() {
    #ifdef CORRADE_TARGET_X86
    Simd::Features features = Simd::Sse3|Simd::Sse2;
    CORRADE_COMPARE(!!(features & Simd::Sse41), false);
    CORRADE_COMPARE(!!(features & Simd::Sse3), true);

    constexpr Simd::Features cFeatures = Simd::Sse3|Simd::Sse2;
    constexpr bool cFeatures1 = !!(cFeatures & Simd::Sse41);
    constexpr bool cFeatures2 = !!(cFeatures & Simd::Sse3);
    CORRADE_VERIFY(!cFeatures1);
    CORRADE_VERIFY(cFeatures2);
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

void SimdTest::featuresOperatorInverse() {
    #ifdef CORRADE_TARGET_X86
    CORRADE_COMPARE(std::uint32_t(~Simd::Scalar), 0xffffffffu);
    CORRADE_COMPARE(std::uint32_t(~(Simd::Sse41|Simd::Sse3)), 4294967285u);
    CORRADE_COMPARE(std::uint32_t(~Simd::Sse41), 4294967287u);

    constexpr Simd::Features cFeatures1 = ~Simd::Scalar;
    constexpr Simd::Features cFeatures2 = ~(Simd::Sse41|Simd::Sse3);
    CORRADE_COMPARE(std::uint32_t(cFeatures1), 0xffffffffu);
    CORRADE_COMPARE(std::uint32_t(cFeatures2), 4294967285u);
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

void SimdTest::featuresCompare() {
    #ifdef CORRADE_TARGET_X86
    Simd::Features features = Simd::Sse41|Simd::Sse2|Simd::Sse3;
    CORRADE_VERIFY(features == features);
    CORRADE_VERIFY(!(features != features));
    CORRADE_VERIFY(Simd::Sse3 == Simd::Sse3);
    CORRADE_VERIFY(Simd::Sse3 != Simd::Sse41);

    CORRADE_VERIFY(Simd::Scalar <= Simd::Sse41);
    CORRADE_VERIFY(Simd::Sse41 >= Simd::Scalar);
    CORRADE_VERIFY(Simd::Sse41 <= Simd::Sse41);
    CORRADE_VERIFY(Simd::Sse41 >= Simd::Sse41);
    CORRADE_VERIFY(Simd::Sse41 <= features);
    CORRADE_VERIFY(features >= Simd::Sse41);
    CORRADE_VERIFY(features <= features);
    CORRADE_VERIFY(features >= features);

    CORRADE_VERIFY(features <= (Simd::Sse41|Simd::Sse2|Simd::Sse3|Simd::Ssse3));
    CORRADE_VERIFY(!(features >= (Simd::Sse41|Simd::Sse2|Simd::Sse3|Simd::Ssse3)));

    constexpr Simd::Features cFeatures = Simd::Sse41|Simd::Sse2|Simd::Sse3;
    constexpr bool cFeaturesEqual = cFeatures == cFeatures;
    constexpr bool cFeaturesNonEqual = cFeatures != cFeatures;
    constexpr bool cFeaturesLessEqual = cFeatures <= cFeatures;
    constexpr bool cFeaturesGreaterEqual = cFeatures >= cFeatures;
    CORRADE_VERIFY(cFeaturesEqual);
    CORRADE_VERIFY(!cFeaturesNonEqual);
    CORRADE_VERIFY(cFeaturesLessEqual);
    CORRADE_VERIFY(cFeaturesGreaterEqual);
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

void SimdTest::detectCompileTime() {
    Debug{} << "Highest compile-time-detected feature:" << Simd::Default;

    /* There should be at least something if we have any of the defines
       present */
    #if defined(CORRADE_TARGET_SSE2) || defined(CORRADE_TARGET_SSE3) || defined(CORRADE_TARGET_SSSE3) || defined(CORRADE_TARGET_SSE41) || defined(CORRADE_TARGET_SSE42) || defined(CORRADE_TARGET_AVX) || defined(CORRADE_TARGET_AVX2) || defined(CORRADE_TARGET_NEON) || defined(CORRADE_TARGET_SIMD128)
    CORRADE_VERIFY(Simd::Features{Simd::Default});

    /* And nothing if we don't */
    #else
    CORRADE_VERIFY(!Simd::Features{Simd::Default});
    #endif
}

void SimdTest::detectRuntime() {
    Simd::Features features;
    Debug{} << "All runtime-detected features:" << Debug::packed << features;

    /* The compile-time feature should be listed among these as well, otherwise
       we wouldn't even be able to run the code. */
    CORRADE_VERIFY(features >= Simd::Default);

    #ifdef CORRADE_TARGET_X86
    /* Test that for every feature, the subset is present as well */
    if(features & Simd::Avx2) CORRADE_VERIFY(features & Simd::Avx);
    if(features & Simd::Avx) CORRADE_VERIFY(features & Simd::Sse42);
    if(features & Simd::Sse42) CORRADE_VERIFY(features & Simd::Sse41);
    if(features & Simd::Sse41) CORRADE_VERIFY(features & Simd::Ssse3);
    if(features & Simd::Ssse3) CORRADE_VERIFY(features & Simd::Sse3);
    if(features & Simd::Sse3) CORRADE_VERIFY(features & Simd::Sse2);
    #else
    /* ARM and WebAssembly currently have just one feature, so no subset
       testing applies on those */
    #endif
}

#ifdef CORRADE_TARGET_X86
#ifdef CORRADE_TARGET_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
void foo(Simd::ScalarT) { Utility::Debug{} << "scalar code :("; }
void foo(Simd::Sse3T) { Utility::Debug{} << "SSE3!"; }
void foo(Simd::Avx2T) { Utility::Debug{} << "AVX2!"; }

void bar(Simd::ScalarT) { Utility::Debug{} << "scalar code :!"; }
#ifdef CORRADE_TARGET_GCC
#pragma GCC diagnostic pop
#endif
#endif

void SimdTest::tagDispatch() {
    #ifdef CORRADE_TARGET_X86
    std::ostringstream out;
    Debug redirectOutput{&out};
    foo(Simd::Sse42);
    bar(Simd::Sse42);
    CORRADE_COMPARE(out.str(),
        "SSE3!\n"
        "scalar code :!\n");
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

void SimdTest::debug() {
    #ifdef CORRADE_TARGET_X86
    std::ostringstream out;
    Debug{&out} << Simd::Scalar << (Simd::Avx2|Simd::Ssse3|Simd::Sse41);
    CORRADE_COMPARE(out.str(), "Simd::Scalar Simd::Ssse3|Simd::Sse41|Simd::Avx2\n");
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

void SimdTest::debugPacked() {
    #ifdef CORRADE_TARGET_X86
    std::ostringstream out;
    Debug{&out} << Debug::packed << Simd::Scalar << Debug::packed << (Simd::Avx2|Simd::Ssse3|Simd::Sse41);
    CORRADE_COMPARE(out.str(), "Scalar Ssse3|Sse41|Avx2\n");
    #else
    CORRADE_SKIP("Only one Simd tag available on this platform, can't test.");
    #endif
}

}}}

CORRADE_TEST_MAIN(Corrade::Test::SimdTest)
