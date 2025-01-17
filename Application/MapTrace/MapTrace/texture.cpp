/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#include "texture.h"

#ifdef _WIN32_
#include <Windows.h>
#endif
#include <cstdio>

#include <iostream>
using namespace std;

Texture::Texture(string & texture_file) :
  m_texture_file(texture_file),
  m_texture(0),
  m_size(-1),
  m_image(0)
{
  cout << "Reading texture from " << m_texture_file << endl;
  read_from_file();
  
  generate_texture();
}

void Texture::generate_texture()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, 
                  GL_TEXTURE_MAG_FILTER, 
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  
  gluBuild2DMipmaps(GL_TEXTURE_2D,
                    GL_RGBA,
                    m_size, m_size,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE, m_image);

}


void Texture::read_from_file()
{
  int rv;

  TIFF * tiff = TIFFOpen(m_texture_file.c_str(), "r");
  if (tiff == 0)
  {
    cerr << "Unable to read " << m_texture_file << endl;
    exit(-1);
  }

  uint32 height, width;
  rv = TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
  rv = TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
  
  /* validate this info */
  if (width != height)
  {
    cerr << "width = " << width << ", height = " << 
      height << ": must be square!\n";;
    exit(-1);
  }
  else
    cout << "square - image width = " << width << endl;

  // some stuff for debugging
//    uint16 tmp16;
//    uint32 tmp32;
//    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &tmp16);
//    cout << tmp16 << endl;
//    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &tmp16);
//    cout << tmp16 << endl;
//    TIFFGetField(tiff, TIFFTAG_ROWSPERSTRIP, &tmp32);
//    cout << tmp32 << endl;
//    TIFFGetField(tiff, TIFFTAG_COMPRESSION, &tmp32);
//    cout << tmp32 << endl;
//    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &tmp32);
//    cout << tmp32 << endl;
//    TIFFGetField(tiff, TIFFTAG_FILLORDER, &tmp32);
//    cout << tmp32 << endl;
//    TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &tmp32);
//    cout << tmp32 << endl;
  
  
  m_size = width;
  switch (m_size)
  {
  case 8: case 16: case 32: case 64: case 128:
  case 256: case 512: case 1024: case 2048:
    break;
  default:
    ///////////////////////////////MessageBox ( NULL , "Invalid image size - must be 2^n on each side", "Error" , MB_OK);
    cout << "Invalid image size - must be 2^n on each side" << endl;
    exit(-1);
  }

  // the raw image
  m_raster.resize(height * width);
  
  if (0 == TIFFReadRGBAImage(tiff, 
                             width,
                             height,
                             &m_raster[0], 
                             0))
  {
    cerr << "Unable to extract image from TIFF\n";
    exit(-1);
  }
  else
    cout << "Read " << m_texture_file << endl;
  
  // now put into the format we want the image in
  
  // aha - it's already in the right format!
  
  m_image = (GLubyte *) &m_raster[0];
}


