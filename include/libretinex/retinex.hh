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

namespace libretinex
{
  typedef unsigned int coord_t;
  typedef unsigned char value_t;
  typedef vpImage<value_t> image_t;

  class LIBRETINEX_DLLAPI Retinex
  {
  public:
    explicit Retinex (const image_t& image);
    ~Retinex ();

    const image_t& outputImage ();

  private:
    static const double sigma_1 = 1.;
    static const double sigma_2 = 3.;

    static const double sigma_ph = 0.5;
    static const double sigma_h = 4;

    image_t outputImage_;

    double gaussian (coord_t x, coord_t y, double sigma) const;
    double DoG (coord_t x, coord_t y) const;
    double F (coord_t x, coord_t y, double sigma) const;

    void applyLa (double sigma);
    void applyDoG ();
    void applyNormalization ();
  };
} // end of namespace libretinex.

#endif // LIBRETINEX_RETINEX_HH
