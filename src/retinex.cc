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

#include <cmath>
#include "libretinex/retinex.hh"

namespace libretinex
{
  namespace
  {
    double imageMean (const image_t& image)
    {
      if (!image.getHeight () || !image.getWidth ())
	return 0.;

      double sum = 0.;
      for (coord_t i = 0; i < image.getHeight (); ++i)
	for (coord_t j = 0; j < image.getWidth (); ++j)
	  sum += image (i, j);
      return sum / (image.getWidth () * image.getHeight ());
    }

    value_t imageMax (const image_t& image)
    {
      value_t res = 0;
      for (coord_t i = 0; i < image.getHeight (); ++i)
	for (coord_t j = 0; j < image.getWidth (); ++j)
	  res = std::max (res, image (i, j));
      return res;
    }
  } // end of anonymous namespace.

  Retinex::Retinex (const image_t& image)
    : outputImage_ (image)
  {}

  Retinex::~Retinex ()
  {
  }

  const image_t&
  Retinex::outputImage ()
  {
    applyLa (sigma_1);
    applyLa (sigma_2);
    applyDoG ();
    applyNormalization ();
    return this->outputImage_;
  }

  value_t
  Retinex::gaussian (coord_t x, coord_t y, double sigma) const
  {
    return 42.;
  }

  value_t
  Retinex::F (coord_t x, coord_t y, double sigma) const
  {
    return 42.;
  }

  void
  Retinex::applyLa (double sigma)
  {
    double mean = imageMean (outputImage_);
    value_t max = imageMax (outputImage_);

    for (coord_t i = 0; i < outputImage_.getHeight (); ++i)
      for (coord_t j = 0; j < outputImage_.getWidth (); ++j)
	{
	  double F = 42. + mean / 2.; //FIXME:

	  double value = outputImage_ (i, j) / (outputImage_ (i, j) + F);
	  value *= max + F;

	  outputImage_ (i, j, value);
	}
  }

  void
  Retinex::applyDoG ()
  {
    for (coord_t i = 0; i < outputImage_.getHeight (); ++i)
      for (coord_t j = 0; j < outputImage_.getWidth (); ++j)
	{
	  double value = outputImage_ (i, j); //FIXME:
	  outputImage_ (i, j, value);
	}
  }

  void
  Retinex::applyNormalization ()
  {
    static const double Th = 5.;

    double mean = imageMean (outputImage_);
    for (coord_t i = 0; i < outputImage_.getHeight (); ++i)
      for (coord_t j = 0; j < outputImage_.getWidth (); ++j)
	{
	  // Normalization
	  double value = outputImage_ (i, j) - mean;
	  value /= 42.; //FIXME: sigma_i?

	  // Post-processing.
	  if (value >= 0)
	    value = std::max (Th, value);
	  else
	    value = -std::max (Th, -value);

	  outputImage_ (i, j, value);
	}
  }
} // end of namespace libretinex.
