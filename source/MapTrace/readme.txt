This provides two programs:

________________ map_trace _____________________

This allows you to trace contours by hand from a previously scanned
contour map (must be square and size 2^n). These contours can then be
interpolated onto a regular cartesian grid (using a diffusion to
smooth the result). The output is in the form of a digital elevation
map (DEM) called orography.dat, and also an image orography.tif. The
latter is such that the intensity of each pixel represents the height,
and it can be touched-up in an image editor before being subsequently
processed with tiff_to_orog. The console output indicates the parameters
necessary for conversion with tiff_to_orog. Note that the greyscale image 
only allows 256 discrete heights to be represented, which can be a problem 
for high-resolution data.

Keys:

'=' and '-': Increase/decrease the current contour level

'+' and '_': adjust the contour interval for the '=' and '-' keys

']' and '[': adjust the size of the final grid.

'{' and '}: adjust the smoothing value used in the
interolation. I.e. the interpolation is initially done to a grid that
is this many times higher resolution than the final grid.

'.' and ',': zoom in/out

left mouse button (release): mark a contour point

right mouse button (hold down): draw's a "free-hand" contour.

's': save the contours in orography.con

'l': load contours from orography.con

'd': delete the current contour

'D': delete all contours at the curent level

'o': Output to orography.dat and orography.tif

'm': toggles between contour and ridge mode

Usage:

map_trace on its own will prompt for input, or:

map_trace map_file.tif map_width_in_m

This specifies the map image containing the contours (square and a
power of 2), and the width, in meters, of the region represented by
the image.

Note the toggle between contour and ridge mode. In contour mode you draw 
contours (yay!). In ridge mode you draw, well, ridges. For example, to
draw a 10m deep ditch that follows the terrain, draw it as a ridge (with a 
-ve depth). Unfortunately the ridge height/width ratio is hard-coded at the 
moment.

___________________ tiff_to_orog  __________________

Converts a TIFF file (greyscale) into an orography DEM (for the
format, see the source code!) suitable for use in my flight sim.


Usage: ./tiff_to_orog dx zmin zmax input_file.tiff
       Writes output to orography.dat

Note that the height resolution in the output is not brilliant, as it
is restricted to 256 equally spaced levels.

======================================================

Both should build under linux (use the Makefiles) and MSVC++ 6.0 (project thingies
are there). Note that the executables get written to this directory - one
above where they are build. Both programs need libtiff. map_trace can be built with 
plib for faster text rendering (though I haven't done so for the windows exe).

Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk

See license.txt for license details.
