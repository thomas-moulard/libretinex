libretinex
==========

This package implements the Retinex algorithm using the ViSP image
processing library.
It is released under LGPL-3. See COPYING.LESSER for licensing information.

Setup
-----

To compile this package, it is recommended to create a separate build
directory:


It is recommended to create a specific directory to install this package.

    mkdir _build
    cd _build
    cmake [options] ..
    make install

### Dependencies

The matrix abstract layer depends on several packages which
have to be available on your machine.

 - Libraries:
   - Boost (>= 1.40)
     Its detection is controled by the `BOOST_ROOT` variable.
   - ViSP (>= 2.6)
     Make sure the visp-config binary is in your path.
 - System tools:
   - CMake (>=2.6)
   - pkg-config
   - usual compilation tools (GCC/G++, make, etc.)

### Options

- `-DCMAKE_INSTALL_PREFIX=<path>` defines the installation prefix to `<path>`.
