/*
  util.cpp - some internal helpers
  Copyright (c) 2022 g10 Code GmbH
  Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>

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

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include "util.h"

#include <functional>

#include <cstring>

std::vector<std::string_view> _gpgmepp::split_into_string_views(const char *s, char delimiter)
{
    std::vector<std::string_view> result;
    if (!s) {
        return result;
    }
    while (const char *segment_end = std::strchr(s, delimiter)) {
        if (const auto segment_size = segment_end - s) {
            result.emplace_back(s, segment_size);
        }
        s = segment_end + 1;
    };
    if (const auto segment_size = std::strlen(s)) {
        result.emplace_back(s, segment_size);
    }
    return result;
}

StringsToCStrings::StringsToCStrings(const std::vector<std::string>& v)
    : m_strings{v}
{
}

const char **StringsToCStrings::c_strs() const
{
    if (m_cstrings.empty()) {
        m_cstrings.reserve(m_strings.size() + 1);
        std::transform(std::begin(m_strings), std::end(m_strings),
                       std::back_inserter(m_cstrings),
                       std::mem_fn(&std::string::c_str));
        m_cstrings.push_back(nullptr);
    }
    return m_cstrings.data();
}
