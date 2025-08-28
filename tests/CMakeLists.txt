# CMakeLists.txt for GpgME++ tests
# Copyright 2025 g10 Code GmbH
# Software engineering by Ingo Klöcker <dev@ingo-kloecker.de>
#
# This file is part of GPGME++.
#
# GPGME++ is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# GPGME++ is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
# Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, see <https://gnu.org/licenses/>.
# SPDX-License-Identifier: LGPL-2.1-or-later

macro(_g10_add_testprogram _source)
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(${_name} ${_source})
    target_link_libraries(${_name} Gpgmepp)
    set_target_properties(${_target} PROPERTIES
                          WIN32_EXECUTABLE FALSE # don't build as GUI app on Windows
                          MACOSX_BUNDLE FALSE # don't build as GUI app on macOS
    )
endmacro()

_g10_add_testprogram(run-createkey.cpp)
_g10_add_testprogram(run-genrandom.cpp)
_g10_add_testprogram(run-getkey.cpp)
_g10_add_testprogram(run-keylist.cpp)
_g10_add_testprogram(run-verify.cpp)
_g10_add_testprogram(run-wkdlookup.cpp)
