/*
    gen-random.cpp
    Copyright (c) 2025 g10 Code GmbH
    Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>

    This file is part of GPGME++'s test suite.

    GPGME++ is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    GPGME++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; if not, see <https://gnu.org/licenses/>.
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include <context.h>
#include <randomresults.h>

#include <charconv>
#include <iostream>
#include <optional>
#include <string_view>

using namespace GpgME;

static int show_usage(int ex)
{
  std::cerr << "usage: run-genrandom COMMAND\n\n"
               "Commands:\n"
               "  --number LIMIT  generate a random number in the range [0, LIMIT)\n"
               "  --bytes COUNT   generate COUNT random bytes\n"
               "  --zbase32       generate 30 random zbase32 characters\n";
  exit(ex);
}

template<typename IntegerType>
static std::optional<IntegerType> parseNumberArgument(std::string_view s)
{
    IntegerType value{};
    if (std::from_chars(s.data(), s.data() + s.size(), value).ec == std::errc{}) {
        return value;
    }
    return {};
}

enum Command
{
    None,
    Number,
    Bytes,
    ZBase32,
};

int main (int argc, char **argv)
{
    int last_argc = -1;
    Command command = Command::None;
    bool needsNumber = false;

    if (argc) {
        argc--; argv++;
    }

    while (argc && last_argc != argc ) {
        last_argc = argc;
        if (!strcmp (*argv, "--")) {
            argc--; argv++;
            break;
        } else if (!strcmp (*argv, "--help")) {
            show_usage (0);
        } else if (!strcmp (*argv, "--number")) {
            command = Command::Number;
            needsNumber = true;
            argc--; argv++;
        } else if (!strcmp (*argv, "--bytes")) {
            command = Command::Bytes;
            needsNumber = true;
            argc--; argv++;
        } else if (!strcmp (*argv, "--zbase32")) {
            command = Command::ZBase32;
            needsNumber = false;
            argc--; argv++;
        } else if (!strncmp (*argv, "--", 2)) {
            show_usage (1);
        }
    }

    if (command == Command::None) {
        std::cerr << "Error: No command given." << std::endl;
        show_usage(1);
    }
    if ((needsNumber && (argc != 1)) || (!needsNumber && (argc != 0))) {
        show_usage (1);
    }

    unsigned int number{};
    if (needsNumber) {
        const auto parsedNumber = parseNumberArgument<unsigned int>(*argv);
        if (!parsedNumber) {
            std::cerr << "Error: Invalid number " << *argv << std::endl;
            show_usage(1);
        }
        number = parsedNumber.value();
    }

    GpgME::initializeLibrary();
    auto ctx = Context::create(OpenPGP);
    if (!ctx) {
        std::cerr << "Failed to create Context";
        return -1;
    }

    switch (command) {
        case Number: {
            const auto result = ctx->generateRandomValue(number);
            if (result.error()) {
                std::cerr << "Error: Failed to generate a random number: " << result.error().asStdString() << std::endl;
            } else {
                std::cout << result.value() << std::endl;
            }
            break;
        }
        case Bytes: {
            const auto result = ctx->generateRandomBytes(number);
            if (result.error()) {
                std::cerr << "Error: Failed to generate random bytes: " << result.error().asStdString() << std::endl;
            } else {
                for (const auto &b : result.value()) {
                    std::cout << b;
                }
            }
            break;
        }
        case ZBase32: {
            const auto result = ctx->generateRandomZBase32String();
            if (result.error()) {
                std::cerr << "Error: Failed to generate random zbase32 characters: " << result.error().asStdString() << std::endl;
            } else {
                std::cout << result.value() << std::endl;
            }
            break;
        }
        default: ;
    }

    return 0;
}
