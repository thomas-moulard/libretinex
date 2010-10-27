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
    /// \brief Describe the different steps of the algorithm.
    ///
    /// Used to ask for a non-complete processing of the input image.
    enum Steps
    {
      /// \brief Nothing has been done.
      NOTHING = 0,
      /// \brief The first logarithmic compression has been applied.
      LA1,
      /// \brief The second logarithmic compression has been applied.
      LA2,
      /// \brief The difference of Gaussians filter has been applied.
      DOG,
      /// \brief The normalization and post-processing steps has been applied.
      NORMALIZE,
      /// \brief Processing is finished (equivalent to NORMALIZE).
      DONE = NORMALIZE
    };

    /// \brief Instantiate the algorithm using an input image.
    ///
    /// \param image the input image (will not be modified).
    /// \param verbosity controls how much information will be displayed
    ///                  (0 means quiet).
    explicit Retinex (const image_t& image, unsigned verbosity = 0);
    ~Retinex ();

    /// \brief The output image.
    ///
    /// The first call to this method will trigger the image processing.
    /// The next calls will just return a reference to the processed image.
    ///
    /// Optionally, the stopAfter parameter can be used to realize an
    /// incomplete processing of the image. In this case, outputImage can
    /// be called with a further step to complete the treatment.
    ///
    /// \warning If outputImage is called with a stopAfter value inferior
    ///          to the previous one, the image will remains the same.
    ///          I.e. it is impossible to go back in the process.
    ///
    /// \param stopAfter can be used to ask for a non-complete processing
    ///                  which useful for debugging.
    ///
    /// \param normalize can be used to call normalizeLuminance after the
    ///                  processing is done to redistribute the luminance
    ///                  into the whole 0..255 range.
    /// \return the processed image
    const image_t& outputImage (Steps stopAfter = DONE, bool normalize = false);

    /// \brief Redistribute luminance to the whole 0..255 range.
    void normalizeLuminance (image_t& image) const;
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

    /// \brief Verbosity level as set by the constructor.
    unsigned verbosity_;

    /// \brief Describe the last applied step.
    Steps step_;

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
