/*
    t-utils.cpp

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

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include "global.h"
#include "util.h"

#include <iostream>

using namespace _gpgmepp;

static void
print_error_and_exit(std::string_view message, std::string_view file, int line)
{
    std::cerr << file << ':' << line << ": " << message << std::endl;
    exit(1);
}

static void
test_split_into_string_views()
{
    {
        const auto result = split_into_string_views(nullptr, ',');
        if (!result.empty()) {
            print_error_and_exit("result not empty", __FILE__, __LINE__);
        }
    }
    {
        const auto result = split_into_string_views("", ',');
        if (!result.empty()) {
            print_error_and_exit("result not empty", __FILE__, __LINE__);
        }
    }
    {
        const auto result = split_into_string_views("abc", ',');
        if ((result.size() != 1) || (result[0] != "abc")) {
            print_error_and_exit("unexpected result", __FILE__, __LINE__);
        }
    }
    {
        const auto result = split_into_string_views("abc,def", ',');
        if ((result.size() != 2) || (result[0] != "abc") || (result[1] != "def")) {
            print_error_and_exit("unexpected result", __FILE__, __LINE__);
        }
    }
    {
        const auto result = split_into_string_views(",abc", ',');
        if ((result.size() != 1) || (result[0] != "abc")) {
            print_error_and_exit("unexpected result", __FILE__, __LINE__);
        }
    }
    {
        const auto result = split_into_string_views("abc,", ',');
        if ((result.size() != 1) || (result[0] != "abc")) {
            print_error_and_exit("unexpected result", __FILE__, __LINE__);
        }
    }
    {
        const auto result = split_into_string_views("abc,,def", ',');
        if ((result.size() != 2) || (result[0] != "abc") || (result[1] != "def")) {
            print_error_and_exit("unexpected result", __FILE__, __LINE__);
        }
    }
    {
        const auto result = split_into_string_views(",", ',');
        if (!result.empty()) {
            print_error_and_exit("result not empty", __FILE__, __LINE__);
        }
    }
}

int
main (int argc, char **argv)
{
    (void)argc;
    (void)argv;

    GpgME::initializeLibrary();

    test_split_into_string_views();

    return 0;
}
