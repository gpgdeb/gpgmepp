/*
    run-createkey.cpp
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

#include <context.h>
#include <keygenerationresult.h>

#include <iostream>

using namespace GpgME;

static int show_usage(int ex)
{
    auto stream = (ex == 0) ? &std::cout : &std::cerr;
    if (ex == 0) {
        *stream << "Create an OpenPGP key or add a subkey to an existing key\n\n";
    }
    *stream << "usage: run-createkey [options] USERID\n"
               "   or: run-createkey --addkey [options] FPR\n"
               "Options:\n"
               "  --cert[ify]       create a certification key\n"
               "  --sign            create a signing key\n"
               "  --encr[ypt]       create an encryption key\n"
               "  --auth[enticate]  create an authentication key\n"
               "  --group           flag created key as group key\n"
               "  --unprotected     do not use a passphrase\n"
               "  --force           do not check for a duplicated user id\n";
    exit(ex);
}

enum Command
{
    CreateKey,
    CreateSubkey,
};

struct Options
{
    Command command = Command::CreateKey;
    std::string userid;
    std::string fingerprint;
    Context::CreationFlags flags = Context::CreateUseDefaults;
};

static Options parseCommandLine(int argc, char **argv)
{
    Options options;

    if (argc) {
        argc--; argv++;
    }

    int last_argc = -1;
    while (argc && last_argc != argc ) {
        last_argc = argc;
        if (!strcmp(*argv, "--")) {
            argc--; argv++;
            break;
        } else if (!strcmp(*argv, "--help")) {
            show_usage(0);
        } else if (!strcmp(*argv, "--addkey")) {
            options.command = CreateSubkey;
            argc--; argv++;
        } else if (!strcmp(*argv, "--certify")) {
            options.flags |= Context::CreateCertify;
            argc--; argv++;
        } else if (!strcmp(*argv, "--sign")) {
            options.flags |= Context::CreateSign;
            argc--; argv++;
        } else if (!strcmp(*argv, "--encrypt")) {
            options.flags |= Context::CreateEncrypt;
            argc--; argv++;
        } else if (!strcmp(*argv, "--authenticate")) {
            options.flags |= Context::CreateAuthenticate;
            argc--; argv++;
        } else if (!strcmp(*argv, "--group")) {
            options.flags |= Context::CreateGroupKey;
            argc--; argv++;
        } else if (!strcmp(*argv, "--unprotected")) {
            options.flags |= Context::CreateNoPassword;
            argc--; argv++;
        } else if (!strcmp(*argv, "--force")) {
            options.flags |= Context::CreateForce;
            argc--; argv++;
        } else if (!strncmp(*argv, "--", 2)) {
            show_usage(1);
        }
    }
    if (argc != 1) {
        show_usage(1);
    }

    switch (options.command) {
    case CreateKey:
        options.userid = *argv;
        break;
    case CreateSubkey:
        options.fingerprint = *argv;
        break;
    }

    return options;
}

int main(int argc, char **argv)
{
    Options options = parseCommandLine(argc, argv);

    GpgME::initializeLibrary();
    auto ctx = Context::create(OpenPGP);
    if (!ctx) {
        std::cerr << "Failed to create Context";
        return -1;
    }

    switch (options.command) {
        case CreateKey: {
            const auto result = ctx->createKey(options.userid.c_str(), std::string{}, 0, options.flags);
            if (result.error().isError()) {
                std::cerr << "Error: Failed to create key: " << result.error().asStdString() << std::endl;
            } else if (!result.error().isCanceled()) {
                std::cout << "Created key with fingerprint " << result.fingerprint() << std::endl;
                Error err;
                const Key key = ctx->key(result.fingerprint(), err, true);
                if (err.isError()) {
                    std::cerr << "Error: Failed to read key: " << err.asStdString() << std::endl;
                }
                if (!key.isNull()) {
                    std::cout << key << std::endl;
                }
            }
            break;
        }
        case CreateSubkey: {
            Error err;
            Key key = ctx->key(options.fingerprint.c_str(), err);
            if (err.isError()) {
                std::cerr << "Error: Failed to read key: " << err.asStdString() << std::endl;
                return 1;
            }
            const auto result = ctx->createSubkey(key, std::string{}, 0, options.flags);
            if (result.error()) {
                std::cerr << "Error: Failed to create subkey: " << result.error().asStdString() << std::endl;
            } else {
                std::cout << "Created subkey with fingerprint " << result.fingerprint() << std::endl;
                key.update();
                for (const auto &subkey : key.subkeys()) {
                    if (!strcmp(subkey.fingerprint(), result.fingerprint())) {
                        std::cout << subkey;
                        break;
                    }
                }
            }
            break;
        }
        default: ;
    }

    return 0;
}
