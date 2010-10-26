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
#include <limits>
#include <boost/numeric/conversion/converter.hpp>
#include <visp/vpImageFilter.h>
#include "libretinex/retinex.hh"

namespace libretinex
{
  typedef boost::numeric::converter<value_t,double> toValueType;
  typedef boost::numeric::converter<unsigned int,double> toUnsignedInt;

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

    value_t imageMin (const image_t& image)
    {
      value_t res = std::numeric_limits<value_t>::max ();
      for (coord_t i = 0; i < image.getHeight (); ++i)
	for (coord_t j = 0; j < image.getWidth (); ++j)
	  res = std::min (res, image (i, j));
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

  double
  Retinex::gaussian (coord_t x, coord_t y, double sigma) const
  {
    double twice_sigma_square = 2 * sigma * sigma;
    double value = 1. / (M_PI * twice_sigma_square);
    value *= std::exp (-1. * (x * x + y * y) / twice_sigma_square);
    return value;
  }

  double
  Retinex::DoG (coord_t x, coord_t y) const
  {
    double value = 0.;
    static const double inv_sigma_ph = 1. / sigma_ph;
    static const double inv_sigma_h = 1. / sigma_h;

    static const double sq_inv_sigma_ph = inv_sigma_ph * inv_sigma_ph;
    static const double sq_inv_sigma_h = inv_sigma_h * inv_sigma_h;

    double half_sq_dst = (x * x + y * y) / -2.;

    value = inv_sigma_ph * std::exp (half_sq_dst * sq_inv_sigma_ph);

    value -= inv_sigma_h * std::exp (half_sq_dst * sq_inv_sigma_h);

    value *= 1. / (std::sqrt (2 * M_PI));
    return value;
  }

  vpMatrix
  Retinex::buildGaussianCoeff (double sigma) const
  {
    // From wikipedia: filter size should be 6 * sigma
    // http://en.wikipedia.org/w/index.php?title=Gaussian_blur&oldid=392439061
    unsigned filterSize = toUnsignedInt::convert (std::ceil (6. * sigma));

    vpMatrix res (filterSize, filterSize);

    for (coord_t i = 0; i < filterSize; ++i)
      for (coord_t j = 0; j < filterSize; ++j)
	res[j][i] = gaussian (i, j, sigma);
    return res;
  }

  vpMatrix
  Retinex::buildDoGCoeff () const
  {
    // FIXME: I don't have any idea about what's the correct size here...
    unsigned filterSize = toUnsignedInt::convert (std::ceil (6. * 1.));

    vpMatrix res (filterSize, filterSize);

    for (coord_t i = 0; i < filterSize; ++i)
      for (coord_t j = 0; j < filterSize; ++j)
	res[j][i] = DoG (i, j);
    return res;
  }

  void
  Retinex::applyLa (double sigma)
  {
    double mean = imageMean (outputImage_);
    value_t max = imageMax (outputImage_);

    vpImage<double> filteredImage;
    vpMatrix G_coeffs = buildGaussianCoeff (sigma);
    vpImageFilter::filter (outputImage_, filteredImage, G_coeffs);

    for (coord_t i = 0; i < outputImage_.getHeight (); ++i)
      for (coord_t j = 0; j < outputImage_.getWidth (); ++j)
	{
	  double F = filteredImage (i, j) + mean / 2.;

	  double value = outputImage_ (i, j) / (outputImage_ (i, j) + F);
	  value *= max + F;

	  outputImage_ (i, j, toValueType::convert (value));
	}
  }

  void
  Retinex::applyDoG ()
  {
    vpImage<double> filteredImage;
    vpMatrix DoG_coeffs = buildDoGCoeff ();
    vpImageFilter::filter (outputImage_, filteredImage, DoG_coeffs);

    for (coord_t i = 0; i < outputImage_.getHeight (); ++i)
      for (coord_t j = 0; j < outputImage_.getWidth (); ++j)
	{
	  double value = filteredImage (i, j);
	  outputImage_ (i, j, toValueType::convert (value));
	}
  }

  void
  Retinex::applyNormalization ()
  {
    static const double Th = 5.;

    double mean = imageMean (outputImage_);

    // FIXME: is it really this?
    const double sigma_i_bip =
      std::fabs (imageMax (outputImage_) - imageMin (outputImage_));

    for (coord_t i = 0; i < outputImage_.getHeight (); ++i)
      for (coord_t j = 0; j < outputImage_.getWidth (); ++j)
	{
	  // Normalization
	  double value = (outputImage_ (i, j) - mean) / sigma_i_bip;

	  // Post-processing.
	  if (value >= 0)
	    value = std::max (Th, value);
	  else
	    value = -std::max (Th, -value);

	  outputImage_ (i, j, toValueType::convert (value));
	}
  }
} // end of namespace libretinex.
