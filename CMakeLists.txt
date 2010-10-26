# Copyright (C) 2010 Thomas Moulard, LAAS-CNRS.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Require at least CMake 2.6 to configure the package.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/image/visp.cmake)

SET(PROJECT_NAME libretinex)
SET(PROJECT_VERSION 0.1.0.99)
SET(PROJECT_DESCRIPTION "the Retinex algorithm")
SET(PROJECT_URL "http://github.com/laas/libretinex")

SETUP_PROJECT()

SET(PKG_CONFIG_LIBS "${PKG_CONFIG_LIBS} ${LIBINCL_KW}retinex${LIB_EXT}")

FIND_VISP()

# Search for Boost.
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost 1.40
  COMPONENTS unit_test_framework system program_options REQUIRED)

# Install headers.
SET(${PROJECT_NAME}_HEADERS
    include/libretinex.hh
    include/libretinex/doc.hh
    include/libretinex/fwd.hh
    include/libretinex/retinex.hh
)

# Source files exploration
ADD_SUBDIRECTORY(src)

ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(doc)
# ADD_SUBDIRECTORY(tests)

SETUP_PROJECT_FINALIZE()
