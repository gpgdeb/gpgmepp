# CMakeLists.txt for GpgME++
# Copyright 2025 g10 Code GmbH
# Software engineering by Carl Schwan <carl.schwan@gnupg.com>
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

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(FULL_VERSION "2.0.0")

string(REGEX MATCH "^[0-9]+.[0-9]+.[0-9]+" cmake_compat_version ${FULL_VERSION})
project(gpgmepp VERSION ${cmake_compat_version})

set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_MICRO ${PROJECT_VERSION_PATCH})

# LT Version numbers, remember to change them just *before* a release.
#   (Code changed:			REVISION++)
#   (Interfaces added/removed/changed:	CURRENT++, REVISION=0)
#   (Interfaces added:			AGE++)
#   (Interfaces removed:		AGE=0)
set(LIBGPGMEPP_LT_CURRENT "7")
set(LIBGPGMEPP_LT_AGE "0")
set(LIBGPGMEPP_LT_REVISION "0")
math(EXPR LIBGPGMEPP_SOVERSION "${LIBGPGMEPP_LT_CURRENT} - ${LIBGPGMEPP_LT_AGE}")

set(GPG_ERROR_REQUIRED_VERSION "1.47")
set(GPGME_REQUIRED_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.0")

option(ENABLE_SHARED "Build shared libraries" ON)
option(ENABLE_STATIC "Build static libraries" OFF)
# The following option is used in gpg4win to distinguish the 32-bit build from
# the 64-bit build
option(PKGCONFIG_HOST "The host value to write in the pkgconfig file" "")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

include(G10CMakeSettings)

include(G10AddDistTargets)
include(G10AddGenChangeLogTarget)
include(G10AddReleaseTargets)
include(G10CompilerSettings)
include(G10GetFullVersion)
include(ECMUninstallTarget)
include(FeatureSummary)
include(GNUInstallDirs)

find_package(LibGpgError ${GPG_ERROR_REQUIRED_VERSION})
set_package_properties(LibGpgError PROPERTIES TYPE REQUIRED)
find_package(Gpgme ${GPGME_REQUIRED_VERSION})
set_package_properties(Gpgme PROPERTIES TYPE REQUIRED)

g10_get_full_version()

# create the VERSION file
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    # get the current commit ID
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --verify HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE commit_id
    )
    string(STRIP "${commit_id}" commit_id) # strip trailing whitespace
    file(WRITE "${CMAKE_BINARY_DIR}/VERSION" "${G10_FULL_VERSION}\n${commit_id}\n")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/VERSION")
    configure_file("${CMAKE_SOURCE_DIR}/VERSION" "VERSION" COPYONLY)
else()
    file(WRITE "${CMAKE_BINARY_DIR}/VERSION" "${G10_FULL_VERSION}\nunknown\n")
endif()

if(UNIX)
    # start ChangeLog at import from KDE in gpgme repo
    g10_add_gen_changelog_target(
        SINCE "2016-03-08T00:00:00"
        FOOTER "${CMAKE_SOURCE_DIR}/build-aux/git-log-footer"
    )

    set(EXTRA_DIST
        "${CMAKE_BINARY_DIR}/ChangeLog"
        "${CMAKE_BINARY_DIR}/VERSION"
    )
    g10_add_dist_targets(
        VERSION "${G10_FULL_VERSION}"
        ARCHIVE_FORMAT "tar.xz"
        EXTRA_FILES ${EXTRA_DIST}
    )
    add_dependencies(dist gen-ChangeLog)

    g10_add_release_targets(
        VERSION "${G10_FULL_VERSION}"
        ARCHIVE_FORMAT "tar.xz"
        RELEASE_ARCHIVE_SUFFIX "gpgme"
    )
endif()

include(G10GitHooks)
g10_configure_git_hooks()

add_subdirectory(src)

if(BUILD_TESTING AND ENABLE_SHARED)
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
