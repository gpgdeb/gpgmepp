/*
  error.cpp - wraps a gpgme error
  Copyright (C) 2003, 2007 Klarälvdalens Datakonsult AB
                2017, 2018 Intevation GmbH

  This file is part of GPGME++.

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

#include "error.h"

#include <gpgme.h>

#include <ostream>

using namespace GpgME;

static void format_error(gpgme_error_t err, std::string &str)
{
    char buffer[ 1024 ];
    gpgme_strerror_r(err, buffer, sizeof buffer);
    buffer[ sizeof buffer - 1 ] = '\0';
    str = buffer;
}

const char *Error::source() const
{
    return gpgme_strsource((gpgme_error_t)mErr);
}

const char *Error::asString() const
{
    if (mMessage.empty()) {
        format_error(static_cast<gpgme_error_t>(mErr), mMessage);
    }
    return mMessage.c_str();
}

std::string Error::asStdString() const
{
    std::string message;
    format_error(static_cast<gpgme_error_t>(mErr), message);
    return message;
}

int Error::code() const
{
    return gpgme_err_code(mErr);
}

int Error::sourceID() const
{
    return gpgme_err_source(mErr);
}

bool Error::isSuccess() const
{
    return code() == GPG_ERR_NO_ERROR;
}

bool Error::isCanceled() const
{
    return code() == GPG_ERR_CANCELED || code() == GPG_ERR_FULLY_CANCELED;
}

bool Error::isError() const
{
    return !isSuccess() && !isCanceled();
}

int Error::toErrno() const
{
    return gpgme_err_code_to_errno(static_cast<gpgme_err_code_t>(code()));
}

// static
bool Error::hasSystemError()
{
    return gpgme_err_code_from_syserror() != GPG_ERR_MISSING_ERRNO ;
}

// static
void Error::setSystemError(gpg_err_code_t err)
{
    setErrno(gpgme_err_code_to_errno(err));
}

// static
void Error::setErrno(int err)
{
    gpgme_err_set_errno(err);
}

// static
Error Error::fromSystemError(unsigned int src)
{
    return Error(gpgme_err_make(static_cast<gpgme_err_source_t>(src), gpgme_err_code_from_syserror()));
}

// static
Error Error::fromErrno(int err, unsigned int src)
{
    return Error(gpgme_err_make(static_cast<gpgme_err_source_t>(src), gpgme_err_code_from_errno(err)));
}

// static
Error Error::fromCode(unsigned int err, unsigned int src)
{
    return Error(gpgme_err_make(static_cast<gpgme_err_source_t>(src), static_cast<gpgme_err_code_t>(err)));
}

namespace GpgME {

std::ostream &operator<<(std::ostream &os, const Error &err)
{
    return os << "GpgME::Error(" << err.encodedError() << " (" << err.asStdString() << "))";
}

}
