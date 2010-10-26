// Copyright 2010 Thomas Moulard.
//
// This file is part of libretinex.
// libretinex is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libretinex is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public License
// along with libretinex.  If not, see <http://www.gnu.org/licenses/>.

#ifndef LIBRETINEX_FWD_HH
# define LIBRETINEX_FWD_HH

template <typename T>
class vpImage;

namespace libretinex
{
  typedef unsigned int coord_t;
  typedef unsigned char value_t;
  typedef vpImage<value_t> image_t;

  class Retinex;
} // end of namespace retinex.

#endif  // LIBRETINEX_FWD_HH
