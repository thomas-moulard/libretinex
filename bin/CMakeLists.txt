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

# The main binary.
ADD_EXECUTABLE(retinex-me retinex-me.cc)

# Link against libraries.
TARGET_LINK_LIBRARIES(retinex-me retinex)
TARGET_LINK_LIBRARIES(retinex-me ${VISP_LIBS})
TARGET_LINK_LIBRARIES(retinex-me ${Boost_LIBRARIES})

INSTALL(TARGETS retinex-me DESTINATION bin)
