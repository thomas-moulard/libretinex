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

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <visp/vpImageIo.h>

#include <libretinex/retinex.hh>

struct Options
{
  std::string input;
  std::string output;
  bool allSteps;
  unsigned verbosity;
};

void
parseOptions (Options& options, int argc, char* argv[])
{
  namespace po = boost::program_options;
  po::options_description desc ("Allowed options");

  desc.add_options ()
    ("help,h", "produce help message")

    ("input,i",
     po::value<std::string> (&options.input),
     "set the input image")

    ("output,o",
     po::value<std::string> (&options.output),
     "set the output image")

    ("all,a",
     po::value<bool> (&options.allSteps)->default_value (false),
     "write images for all the steps of the algorithm")

    ("verbosity,v",
     po::value<unsigned> (&options.verbosity)->default_value (0),
     "control the library verbosity")
    ;

  po::variables_map vm;
  try
    {
      po::store (po::parse_command_line (argc, argv, desc), vm);
      po::notify (vm);
    }
  catch (po::error& error)
    {
      std::cerr << "Error while parsing argument: "
		<< error.what () << std::endl;
      exit (1);
    }

  if (vm.count ("help"))
    {
      std::cout << desc << std::endl;
      exit (0);
    }
}

int main (int argc, char* argv[])
{
  Options options;

  parseOptions (options, argc, argv);

  libretinex::image_t image;
  try
    {
      vpImageIo::read (image, options.input.c_str ());
    }
  catch (vpImageException& exception)
    {
      std::cout << "Failed to read the input image." << std::endl;
      exit (1);
    }

  libretinex::Retinex retinex (image, options.verbosity);
  libretinex::image_t outputImage = image;

  for (int step = libretinex::Retinex::NOTHING;
       step <= libretinex::Retinex::DONE; step += 1)
    {
      outputImage =
	retinex.outputImage (static_cast<libretinex::Retinex::Steps> (step));
      if (options.allSteps)
	{
	  boost::format fmt ("/tmp/retinex-me-%d.pgm");
	  fmt % step;
	  vpImageIo::write (outputImage, fmt.str ().c_str ());
	}
    }

  try
    {
      vpImageIo::write (outputImage, options.output.c_str ());
    }
  catch (vpImageException& exception)
    {
      std::cout << "Failed to write the output image." << std::endl;
      exit (1);
    }
  return 0;
}
