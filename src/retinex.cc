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

#include <cassert>
#include <cmath>
#include <limits>
#include <boost/format.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <visp/vpImageFilter.h>
#include "libretinex/retinex.hh"

namespace libretinex
{
  // This converter convert a double into value_t (i.e. unsigned char)
  // without throwing when bounds are overflowed.
  // Default behavior of Boost.numeric_conversion is this case
  // is to emit an exception which is not required here.
  // Do *not* use this converter if too low/high values truncation are
  // an issue.
  typedef boost::numeric::converter
  <value_t,
   double,
   boost::numeric::conversion_traits<value_t, double>,
   boost::numeric::silent_overflow_handler> toValueType;

  typedef boost::numeric::converter<unsigned int,double> toUnsignedInt;

  namespace
  {
    /// \brief Compute the image mean value.
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

    /// \brief Compute the image maximum value.
    value_t imageMax (const image_t& image)
    {
      value_t res = 0;
      for (coord_t i = 0; i < image.getHeight (); ++i)
	for (coord_t j = 0; j < image.getWidth (); ++j)
	  res = std::max (res, image (i, j));
      return res;
    }

    /// \brief Compute the image minimum value.
    value_t imageMin (const image_t& image)
    {
      value_t res = std::numeric_limits<value_t>::max ();
      for (coord_t i = 0; i < image.getHeight (); ++i)
	for (coord_t j = 0; j < image.getWidth (); ++j)
	  res = std::min (res, image (i, j));
      return res;
    }
  } // end of anonymous namespace.

  Retinex::Retinex (const image_t& image, unsigned verbosity)
    : verbosity_ (verbosity),
      step_ (NOTHING),
      outputImage_ (image)
  {
    if (verbosity_ > 1)
      std::cout << "Default constructor of Retinex." << std::endl;

    if (verbosity_ > 1)
      {
	std::cout << "\tImage information:" << std::endl;
	std::cout << "\tWidth = " << outputImage_.getWidth () << std::endl;
	std::cout << "\tHeight = " << outputImage_.getHeight () << std::endl;
      }
  }

  Retinex::~Retinex ()
  {
  }

  const image_t&
  Retinex::outputImage (Steps stopAfter)
  {
    if (verbosity_ > 1)
      {
	std::cout << "Retinex::outputImage" << std::endl;
	std::cout << "\tStop after: " << stopAfter << std::endl;
      }

    if (stopAfter <= NOTHING)
      return this->outputImage_;

    applyLa (sigma_1);
    if (stopAfter <= LA1)
      return this->outputImage_;

    applyLa (sigma_2);
    if (stopAfter <= LA2)
      return this->outputImage_;

    applyDoG ();
    if (stopAfter <= DOG)
      return this->outputImage_;

    applyNormalization ();
    if (stopAfter <= NORMALIZE)
      return this->outputImage_;

    step_ = DONE;
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
    const unsigned filterSize =
      toUnsignedInt::convert (std::ceil (6. * sigma)) + 1;

    // We add one to make sure the filterSize is uneven so G(0, 0) appears
    // in the center of the coefficient matrix.
    //
    // (6 + 1) / 3 = 2 (for integers)
    //    0         1         2         3        4        5        6
    // 0  G(-3, -3) G(-2, -3) G(-1, -3) G(0, -3) G(1, -3) G(2, -3) G(3, -3)
    // 1  G(-3, -2) G(-2, -2) G(-1, -2) G(0, -2) G(1, -2) G(2, -2) G(3, -2)
    // 2  G(-3, -1) G(-2, -1) G(-1, -1) G(0, -1) G(1, -1) G(2, -1) G(3, -1)
    // 3  G(-3,  0) G(-2,  0) G(-1,  0) G(0,  0) G(1,  0) G(2,  0) G(3,  0)
    // 4  G(-3, +1) G(-2, +1) G(-1, +1) G(0, +1) G(1, +1) G(2, +1) G(3, +1)
    // 5  G(-3, +2) G(-2, +2) G(-1, +2) G(0, +2) G(1, +2) G(2, +2) G(3, +2)
    // 6  G(-3, -3) G(-2, +3) G(-1, +3) G(0, +3) G(1, +3) G(2, +3) G(3, +3)

    const unsigned hw = filterSize / 2;
    const unsigned hs = filterSize / 2;

    vpMatrix res (filterSize, filterSize);

    for (coord_t i = 0; i < filterSize; ++i)
      for (coord_t j = 0; j < filterSize; ++j)
	res[j][i] = gaussian (i - hw, j - hs, sigma);
    return res;
  }

  vpMatrix
  Retinex::buildDoGCoeff () const
  {
    // FIXME: I don't have any idea about what's the correct size here...
    unsigned filterSize =
      toUnsignedInt::convert (std::ceil (6. * 1.)) + 1;

    vpMatrix res (filterSize, filterSize);

    const unsigned hw = filterSize / 2;
    const unsigned hs = filterSize / 2;

    for (coord_t i = 0; i < filterSize; ++i)
      for (coord_t j = 0; j < filterSize; ++j)
	res[j][i] = DoG (i - hw, j - hs);
    return res;
  }

  void
  Retinex::applyLa (double sigma)
  {
    if (sigma == sigma_1)
      {
	if (step_ >= LA1)
	  return;
      }
    else if (sigma == sigma_2)
      {
	if (step_ >= LA2)
	  return;
      }
    else
      assert (0 && "should never happen");

    if (verbosity_ > 0)
      {
	boost::format fmt ("Apply logarithmic compression (sigma = %1%)");
	fmt % sigma;
	std::cout << fmt.str () << std::endl;
      }

    double mean = imageMean (outputImage_);
    value_t max = imageMax (outputImage_);

    if (verbosity_ > 1)
      {
	std::cout << "\tMean = " << mean << std::endl;
	std::cout << "\tMax = " << static_cast<int> (max) << std::endl;
      }

    vpImage<double> filteredImage;
    vpMatrix G_coeffs = buildGaussianCoeff (sigma);
    vpImageFilter::filter (outputImage_, filteredImage, G_coeffs);

    coord_t G_coeffs_w = G_coeffs.getCols ();
    coord_t G_coeffs_h = G_coeffs.getRows ();

    for (coord_t i = 0; i < outputImage_.getHeight (); ++i)
      for (coord_t j = 0; j < outputImage_.getWidth (); ++j)
	{
	  double F = filteredImage (i, j) + mean / 2.;

	  if ((i < G_coeffs_h / 2)
	      || (j < G_coeffs_w / 2)
	      || (i + G_coeffs_h / 2 >= outputImage_.getHeight ())
	      || (j + G_coeffs_w / 2 >= outputImage_.getWidth ()))
	    F = outputImage_ (i, j) + mean / 2.;

	  double value = outputImage_ (i, j) / (outputImage_ (i, j) + F);
	  value *= max + F;

	  outputImage_ (i, j, toValueType::convert (value));
	}


    if (sigma == sigma_1)
      {
	if (step_ < LA1)
	  step_ = LA1;
      }
    else if (sigma == sigma_2)
      {
	if (step_ < LA2)
	  step_ = LA2;
      }
    else
      assert (0 && "should never happen");
  }

  void
  Retinex::applyDoG ()
  {
    if (step_ >= DOG)
      return;

    if (verbosity_ > 0)
      std::cout << "Apply the difference of Gaussians filter." << std::endl;

    vpImage<double> filteredImage;
    vpMatrix DoG_coeffs = buildDoGCoeff ();
    vpImageFilter::filter (outputImage_, filteredImage, DoG_coeffs);

    for (coord_t i = 0; i < outputImage_.getHeight (); ++i)
      for (coord_t j = 0; j < outputImage_.getWidth (); ++j)
	{
	  double value = filteredImage (i, j);
	  outputImage_ (i, j, toValueType::convert (value));
	}

    if (step_ < DOG)
      step_ = DOG;
  }

  void
  Retinex::applyNormalization ()
  {
    if (step_ >= NORMALIZE)
      return;

    static const double Th = 5.;

    if (verbosity_ > 0)
      std::cout << "Apply normalization and post-processing." << std::endl;

    double mean = imageMean (outputImage_);

    // FIXME: is it really this?
    const double sigma_i_bip =
      std::fabs (imageMax (outputImage_) - imageMin (outputImage_));

    if (verbosity_ > 1)
      {
	std::cout << "\tTh = " << Th << std::endl;
	std::cout << "\tMean = " << mean << std::endl;
	std::cout << "\tsigma_i_bip = " << sigma_i_bip << std::endl;
      }

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

    if (step_ < NORMALIZE)
      step_ = NORMALIZE;
  }
} // end of namespace libretinex.

//  LocalWords:  Gaussians
