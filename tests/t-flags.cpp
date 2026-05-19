/*
    t-flags.cpp

    This file is part of GPGME++'s test suite.
    Copyright (c) 2025 g10 Code GmbH
    Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>

    GPGME++ is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    GPGME++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with GPGME++; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "flags.h"

#include <iostream>
#include <sstream>

static void
print_error_and_exit(std::string_view message, std::string_view file, int line)
{
    std::cerr << file << ':' << line << ": " << message << std::endl;
    exit(1);
}

template<typename T>
static void
compare_helper(T actualValue, T expectedValue, std::string_view expression, std::string_view expected, std::string_view filename, int line)
{
    if (actualValue != expectedValue) {
        std::stringstream stream;
        stream << "Expression is not equal to " << expected << ": " << expression << std::endl;
        stream << "expected: " << expectedValue << std::endl;
        stream << "actual  : " << actualValue << std::endl;
        print_error_and_exit(stream.str(), filename, line);
    }
}

#define ASSERT_TRUE(expression) do { if (!(expression)) { print_error_and_exit("Expression is not true: " #expression, __FILE__, __LINE__); } } while (false)
#define ASSERT_FALSE(expression) do { if ((expression)) { print_error_and_exit("Expression is not false: " #expression, __FILE__, __LINE__); } } while (false)
#define ASSERT_EQUAL(expression, expected) do { compare_helper(expression, expected, #expression, #expected, __FILE__, __LINE__); } while (false)

enum class TestFlag : unsigned int {
    A = 1 << 0,
    B = 1 << 1,
    C = 1 << 2,
};
using TestFlags = GpgME::Flags<TestFlag, 3>;

static void
test_construction()
{
    {
        const TestFlags flags;
        ASSERT_EQUAL(flags.toUnderlyingType(), 0u);
        ASSERT_TRUE(!flags);
        ASSERT_FALSE(flags.test(TestFlag::A));
        ASSERT_FALSE(flags.test(TestFlag::B));
        ASSERT_FALSE(flags.test(TestFlag::C));
    }
    {
        const TestFlags flags{TestFlag::A};
        ASSERT_EQUAL(flags.toUnderlyingType(), 1u);
        ASSERT_TRUE(flags);
        ASSERT_TRUE(flags.test(TestFlag::A));
        ASSERT_FALSE(flags.test(TestFlag::B));
        ASSERT_FALSE(flags.test(TestFlag::C));
    }
    {
        const TestFlags flags{TestFlag::B, TestFlag::C};
        ASSERT_EQUAL(flags.toUnderlyingType(), 6u);
        ASSERT_TRUE(flags);
        ASSERT_FALSE(flags.test(TestFlag::A));
        ASSERT_TRUE(flags.test(TestFlag::B));
        ASSERT_TRUE(flags.test(TestFlag::C));
    }
    {
        const TestFlags flags = TestFlags::fromUnderlyingType(3u);
        ASSERT_EQUAL(flags.toUnderlyingType(), 3u);
        ASSERT_TRUE(flags);
        ASSERT_TRUE(flags.test(TestFlag::A));
        ASSERT_TRUE(flags.test(TestFlag::B));
        ASSERT_FALSE(flags.test(TestFlag::C));
    }
}

static void
test_set_reset()
{
    {
        TestFlags flags;
        flags.set(TestFlag::A);
        ASSERT_TRUE(flags.test(TestFlag::A));
        ASSERT_FALSE(flags.test(TestFlag::B));
        ASSERT_FALSE(flags.test(TestFlag::C));
        flags.set(TestFlag::C);
        ASSERT_TRUE(flags.test(TestFlag::A));
        ASSERT_FALSE(flags.test(TestFlag::B));
        ASSERT_TRUE(flags.test(TestFlag::C));
        flags.set(TestFlag::B);
        ASSERT_TRUE(flags.test(TestFlag::A));
        ASSERT_TRUE(flags.test(TestFlag::B));
        ASSERT_TRUE(flags.test(TestFlag::C));
        flags.set(TestFlag::A, false);
        ASSERT_FALSE(flags.test(TestFlag::A));
        ASSERT_TRUE(flags.test(TestFlag::B));
        ASSERT_TRUE(flags.test(TestFlag::C));
    }
    {
        TestFlags flags{TestFlag::A, TestFlag::B, TestFlag::C};
        flags.reset(TestFlag::B);
        ASSERT_TRUE(flags.test(TestFlag::A));
        ASSERT_FALSE(flags.test(TestFlag::B));
        ASSERT_TRUE(flags.test(TestFlag::C));
        flags.reset();
        ASSERT_TRUE(!flags);
        ASSERT_FALSE(flags.test(TestFlag::A));
        ASSERT_FALSE(flags.test(TestFlag::B));
        ASSERT_FALSE(flags.test(TestFlag::C));
    }
    {
        TestFlags flags;
        flags.set(TestFlag::A).set(TestFlag::B);
        ASSERT_TRUE(flags.test(TestFlag::A));
        ASSERT_TRUE(flags.test(TestFlag::B));
        ASSERT_FALSE(flags.test(TestFlag::C));
    }
    {
        TestFlags flags;
        flags.set(TestFlag::A).set(TestFlag::B).reset(TestFlag::A).set(TestFlag::B, false).set(TestFlag::C);
        ASSERT_FALSE(flags.test(TestFlag::A));
        ASSERT_FALSE(flags.test(TestFlag::B));
        ASSERT_TRUE(flags.test(TestFlag::C));
    }
    {
        TestFlags flags;
        flags.set(TestFlag::A).set(TestFlag::B).reset().set(TestFlag::C);
        ASSERT_FALSE(flags.test(TestFlag::A));
        ASSERT_FALSE(flags.test(TestFlag::B));
        ASSERT_TRUE(flags.test(TestFlag::C));
    }
}

static void
test_operators()
{
    const TestFlags flagsA{TestFlag::A};
    const TestFlags flagsB{TestFlag::B};
    const TestFlags flagsC{TestFlag::C};
    const TestFlags flagsAB{TestFlag::A, TestFlag::B};
    const TestFlags flagsAC{TestFlag::A, TestFlag::C};
    const TestFlags flagsBC{TestFlag::B, TestFlag::C};
    const TestFlags flagsABC{TestFlag::A, TestFlag::B, TestFlag::C};
    {
        ASSERT_EQUAL(flagsA | flagsB, flagsAB);
        ASSERT_EQUAL(flagsA | TestFlag::B, flagsAB);
        ASSERT_EQUAL(TestFlag::A | flagsB, flagsAB);
        TestFlags flags = flagsA;
        flags |= flagsB;
        ASSERT_EQUAL(flags, flagsAB);
        flags |= TestFlag::C;
        ASSERT_EQUAL(flags, flagsABC);
    }
    {
        ASSERT_EQUAL(flagsAB & flagsB, flagsB);
        ASSERT_EQUAL(flagsAB & TestFlag::B, flagsB);
        ASSERT_EQUAL(TestFlag::B & flagsAB, flagsB);
        TestFlags flags = flagsABC;
        flags &= flagsBC;
        ASSERT_EQUAL(flags, flagsBC);
        flags &= TestFlag::C;
        ASSERT_EQUAL(flags, flagsC);
    }
    {
        ASSERT_EQUAL(flagsAB ^ flagsAC, flagsBC);
        ASSERT_EQUAL(flagsAB ^ TestFlag::B, flagsA);
        ASSERT_EQUAL(TestFlag::B ^ flagsAB, flagsA);
        TestFlags flags = flagsAB;
        flags ^= flagsAC;
        ASSERT_EQUAL(flags, flagsBC);
        flags ^= TestFlag::C;
        ASSERT_EQUAL(flags, flagsB);
    }
    {
        TestFlags flags = ~flagsAB;
        ASSERT_FALSE(flags & flagsA);
        ASSERT_FALSE(flags & flagsB);
        ASSERT_TRUE(flags & flagsC);
    }
    {
        TestFlags flags = ~flagsABC;
        ASSERT_FALSE(flags & flagsA);
        ASSERT_FALSE(flags & flagsB);
        ASSERT_FALSE(flags & flagsC);
        ASSERT_FALSE(flags);
    }
    {
        ASSERT_TRUE(flagsAB != flagsBC);
        ASSERT_FALSE(flagsAB != flagsAB);
    }
}

GPGMEPP_DEFINE_ENUM_FLAG_OPERATORS(TestFlags)

static void
test_additional_flag_operators()
{
    const TestFlags flagsA{TestFlag::A};
    const TestFlags flagsAB{TestFlag::A, TestFlag::B};
    ASSERT_EQUAL(TestFlag::A | TestFlag::A, flagsA);
    ASSERT_EQUAL(TestFlag::A | TestFlag::B, flagsAB);
    ASSERT_EQUAL(TestFlag::A & TestFlag::A, flagsA);
    ASSERT_EQUAL(TestFlag::A & TestFlag::B, TestFlags());
    ASSERT_EQUAL(TestFlag::A ^ TestFlag::A, TestFlags());
    ASSERT_EQUAL(TestFlag::A ^ TestFlag::B, flagsAB);
}

int
main (int argc, char **argv)
{
    (void)argc;
    (void)argv;

    test_construction();
    test_set_reset();
    test_operators();
    test_additional_flag_operators();

    return 0;
}
