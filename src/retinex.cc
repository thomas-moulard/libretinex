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

#include "libretinex/retinex.hh"

namespace libretinex
{
  Retinex::Retinex (const image_t& image)
    : outputImage_ (image)
  {}

  Retinex::~Retinex ()
  {
  }

  const Retinex::image_t&
  Retinex::outputImage ()
  {
    applyLa (sigma_1);
    applyLa (sigma_2);
    applyDoG ();
    applyNormalization ();
    applyPP ();
    return this->outputImage_;
  }

  Retinex::value_t
  Retinex::gaussian (coord_t x, coord_t y, double sigma) const
  {
    return 42.;
  }

  Retinex::value_t
  Retinex::F (coord_t x, coord_t y, double sigma) const
  {
    return 42.;
  }

  void
  Retinex::applyLa (double sigma)
  {
  }

  void
  Retinex::applyDoG ()
  {
  }

  void
  Retinex::applyNormalization ()
  {
  }

  void
  Retinex::applyPP ()
  {
  }
} // end of namespace libretinex.
