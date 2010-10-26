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

#ifndef LIBRETINEX_RETINEX_HH
# define LIBRETINEX_RETINEX_HH
# include <visp/vpImage.h>

# include <libretinex/config.hh>
# include <libretinex/fwd.hh>

namespace libretinex
{
  /// \brief This class implements the retinex library: it takes an image a
  ///        gray image as input and produces a gray image as output.
  ///        There is no parameters to set.
  ///
  /// This class implements the Retinex algorithm as stated in the
  /// ``Normalisation d'illumination basée sur un modèle de rétine:
  ///   application à la reconnaissance de visage.''
  /// by Ngoc-Son Vu and Alice Caplier.
  ///
  /// This algorithm normalizes the illumination of a gray image
  /// through the use of two non-linear functions and a difference of
  /// Gaussians filter.
  ///
  class LIBRETINEX_DLLAPI Retinex
  {
  public:
    /// \brief Instantiate the algorithm using an input image.
    ///
    /// \param image the input image (will not be modified).
    explicit Retinex (const image_t& image);
    ~Retinex ();

    /// \brief The output image.
    ///
    /// The first call to this method will trigger the image processing.
    /// The next calls will just return a reference to the processed image.
    const image_t& outputImage ();

  private:
    /// \brief Standard deviation for the first logarithmic compression.
    static const double sigma_1 = 1.;
    /// \brief Standard deviation of the second logarithmic compression.
    static const double sigma_2 = 3.;

    /// \brief Standard deviation used for the difference of Gaussian.
    ///
    /// Ph stands for Photoreceptor.
    static const double sigma_ph = 0.5;

    /// \brief Standard deviation used for the difference of Gaussian.
    ///
    /// H stands for horizontal (cells).
    static const double sigma_h = 4;

    /// \brief False if processing has not been already done, true
    /// otherwise.
    bool done_;

    /// \brief The processed image.
    ///
    /// At start-up it contains a copy of the input image, then the
    /// image is processed in place by the different steps of the
    /// algorithm.
    image_t outputImage_;

    /// \brief Compute the Gaussian function.
    double gaussian (coord_t x, coord_t y, double sigma) const;

    /// \brief Compute the Difference of Gaussian function.
    double DoG (coord_t x, coord_t y) const;

    /// \brief Compute the matrix of coefficients for vpImageFilter::filter.
    vpMatrix buildGaussianCoeff (double sigma) const;
    /// \brief Compute the matrix of coefficients for vpImageFilter::filter.
    vpMatrix buildDoGCoeff () const;

    /// \brief Generic processing function for the logarithmic compressions.
    void applyLa (double sigma);
    /// \brief Apply the difference of Gaussians filter.
    void applyDoG ();
    /// \brief Apply normalization and post-processing steps.
    void applyNormalization ();
  };
} // end of namespace libretinex.

#endif // LIBRETINEX_RETINEX_HH

//  LocalWords:  retinex Gaussians
