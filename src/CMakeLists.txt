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

# Add required definitions.
ADD_DEFINITIONS(${VISP_CFLAGS})

# The shared library is being built right now.
# Required for dllimport/dllexport mechanisms in
# the generated header config.hh.
ADD_DEFINITIONS(-DBUILDING_LIBRETINEX)

# The main library.
ADD_LIBRARY(retinex
  SHARED
  retinex.cc
  )

# Set shared library version.
SET_TARGET_PROPERTIES(retinex PROPERTIES SOVERSION ${PROJECT_VERSION})

# Link against libraries.
TARGET_LINK_LIBRARIES(retinex ${VISP_LIBS})
TARGET_LINK_LIBRARIES(retinex ${Boost_LIBRARIES})

INSTALL(TARGETS retinex DESTINATION lib)
