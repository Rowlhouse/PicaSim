/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glut.h>
#include <tiffio.h>

#include <string>
#include <vector>

class Texture
{
public:
  Texture(std::string & texture_file);
  void generate_texture();
  GLuint get_texture() const {return m_texture;}
  int get_size() const {return m_size;}
private:
  void read_from_file();
  std::string m_texture_file;
  GLuint m_texture;
  int m_size;
  GLubyte * m_image;
  std::vector<uint32> m_raster;
};


#endif
