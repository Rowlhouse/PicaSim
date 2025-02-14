/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#define _CRT_SECURE_NO_WARNINGS
#include "contour_collection.h"
#include "ridge_collection.h"
#include "texture.h"
#include "text_overlay.h"
#include "detailed_dem.h"

#include <GL/glut.h>

//#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Map image
string map_file;
Texture * map_texture;

// Zoom/panning
float pos_x = 0.0;
float pos_y = 0.0;
int zoom = 1;

// Text overlay
Text_overlay text_overlay;

// wind size
int window_w = 400;
int window_h = 400;

// The contours
Contour_collection contour_collection;
Ridge_collection ridge_collection;

Contour * current_contour = 0;

float current_level = 0;
float d_level = 10;

bool right_button_down = false;

// contour or ridge mode
enum Input_mode {CONTOUR_MODE, RIDGE_MODE};
Input_mode input_mode = CONTOUR_MODE;

// output
int output_sizes[] = {33, 65, 129, 257, 513, 1025, 2049, 4096};
int output_size_index = 2;
// so temporary bitmap is output_smooth_factor bigger in each
// direction.
int output_smooth_factor = 2; 

// Floating end of the contour
Position current_contour_end(0, 0);

// functions registered with GLUT
void display();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void reshape(int w, int h);
void key(unsigned char key, int x, int y);
void special(int key, int x, int y);

// Other fns
void setup_projection();
void setup_gl_properties();

void add_new_contour();
void close_and_finish_current_contour();
void finish_current_contour();
void delete_current_contour();

void draw_map();
void do_text();
void do_output();

void do_save();
void do_load();

void do_help();

const char license[]="MapTrace v2.0.0 Copyright (C) 2013 Danny Chapman:\n"
"    picasimulator@gmail.com\n"
"MapTrace comes with ABSOLUTELY NO WARRANTY\n";

/*int main(int argc, char * argv[])
{
  cout << license << endl;

  // OPENFILENAME ofn ;
  // char szFile[512] ;
  // // open a file name
  // ZeroMemory( &ofn , sizeof( ofn));
  // ofn.lStructSize = sizeof ( ofn );
  // ofn.hwndOwner = NULL  ;
  // ofn.lpstrFile = szFile ;
  // ofn.lpstrFile[0] = '\0';
  // ofn.nMaxFile = sizeof( szFile );
  // ofn.lpstrFileTitle = "TIFF file to trace";
  // //ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
  // ofn.lpstrFilter = "TIFF\0*.tif*\0All\0*.*\0";
  // ofn.nFilterIndex =1;
  // ofn.lpstrFileTitle = NULL ;
  // ofn.nMaxFileTitle = 0 ;
  // ofn.lpstrInitialDir=NULL ;

  // /////////////////////////////////BOOL result = GetOpenFileName(&ofn);
  // bool result = GetOpenFileName(&ofn);

  // if (result == 1)
  // {
  //   map_file = ofn.lpstrFile;
  // }
  // else
  // {
  //   cout << "Need to pick a valid source image\n";
  //   exit(-1);
  // }
  // Demander à l'utilisateur de saisir le chemin du fichier
  cout << "Entrez le chemin du fichier TIFF à charger: ";
  cin >> map_file;

  if (map_file.empty()) {
      cout << "Veuillez saisir un fichier valide." << endl;
      exit(-1);
  }

  // print keys
  do_help();

  // start graphics

  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  
  glutInitWindowSize(window_w, window_h);
  glutCreateWindow("Map Trace");

  setup_projection();
  
  setup_gl_properties();

  glutDisplayFunc(display);
  glutKeyboardFunc(key);
  glutSpecialFunc(special);
  glutMouseFunc(mouse);
  glutReshapeFunc(reshape);
  glutMotionFunc(motion);
  glutPassiveMotionFunc(motion);

  // map texture, now GL is up
  map_texture = new Texture(map_file);

  // enter the main loop
  glutMainLoop();

  return 0;
}*/

void setup_projection()
{
  float x0 = pos_x;
  float y0 = pos_y;
  float width = 1.0 / zoom;
  float x1 = x0 + width;
  float y1 = y0 + width;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(x0, x1, y0, y1);
  glMatrixMode(GL_MODELVIEW);
}

void setup_gl_properties()
{
  glClearColor(1.0, 1.0, 1.0, 0);
  glDisable( GL_DEPTH_TEST ) ;
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw the map
  draw_map();
  
  // draw the finished contours
  contour_collection.draw(current_level);
  ridge_collection.draw(current_level);

  // draw the current contour, if there is one
  if (current_contour)
  {
    glPushMatrix();
    glLineWidth(2.0);
    bool added = current_contour->add_point(current_contour_end);
    current_contour->draw(current_level);
    if (added) current_contour->pop_end();
    glLineWidth(1.0);
    glPopMatrix();
  }
  
  // Draw the text
  do_text();
  
  text_overlay.display();
  text_overlay.reset();

  glutSwapBuffers();
}


void get_xy_from_window_xy(int x, int y, float & fx, float & fy)
{
  float width = 1.0 / zoom;

  fx = pos_x + width * ((float) x / window_w);
  fy = pos_y + width * (1.0F - (float) y / window_h);
}

void mouse(int button, int state, int x, int y)
{
  cout << "Mouse: " << button << " " << state << " " << x << " " << y << endl;

  float fx, fy;
  get_xy_from_window_xy(x, y, fx, fy);

  // act on button up so that the user can abort by moving outside the
  // window
  if ( (button == GLUT_LEFT_BUTTON) &&
       (state == GLUT_UP) )
  {
    if (current_contour == 0)
    {
      cout << "No current contour so adding one\n";
      add_new_contour();
    }

    Position pos(fx, fy);
    pos.clip();

    cout << "Adding point at " << pos.x << " " << pos.y << endl;
    
    current_contour->add_point(pos);
  }
  else if ( (button == GLUT_RIGHT_BUTTON) &&
            (state == GLUT_DOWN) )
  {
    right_button_down = true;
    // start a contour if we don't already have one
    if (current_contour == 0)
    {
      cout << "No current contour so adding one\n";
      add_new_contour();
      Position pos(fx, fy);
      pos.clip();
      
      cout << "Adding point at " << pos.x << " " << pos.y << endl;
      
      current_contour->add_point(pos);
    }
  }
  else if ( (button == GLUT_RIGHT_BUTTON) &&
            (state == GLUT_UP) )
    right_button_down = false;
  
       
  glutPostRedisplay();
}

void motion(int x, int y)
{
  float fx, fy;
  get_xy_from_window_xy(x, y, fx, fy);

  current_contour_end = Position(fx, fy);
  current_contour_end.clip();

  // if the right mouse button is down we add a point (only gets added
  // if it's not the same as whats already at the end).

  if ( current_contour && (true == right_button_down) )
  {
    // tolerance of so many pixels
    float d = 0.01/zoom;
    
    current_contour->add_point(current_contour_end, d);
  }
  
  glutPostRedisplay();
}

void reshape(int w, int h)
{
  window_w = w;
  window_h = h;
  glViewport(0, 0, w, h);
  setup_projection();
}

void key(unsigned char key, int x, int y)
{
  switch(key)
  {
  case 27: // escape
  case 'q':
    exit(0);
    break;
  case 'o':
    do_output();
    break;
  case 'm':
    input_mode = (input_mode == CONTOUR_MODE ? RIDGE_MODE : CONTOUR_MODE);
    break;
  case '=':
    current_level += d_level;
    if (current_contour) current_contour->set_level(current_level);
    cout << "New contour level = " << current_level << endl;
    break;
  case '-':
    current_level -= d_level;
    if (current_contour) current_contour->set_level(current_level);
    cout << "New contour level = " << current_level << endl;
    break;
  case '+':
    d_level += 1;
    break;
  case '_':
    d_level -= 1;
    if (d_level < 1) d_level = 1;
    break;
  case ']':
    if (output_size_index < 
        (int) (sizeof(output_sizes)/sizeof(output_sizes[0])) - 1 )
      ++output_size_index;
    break;
  case '[':
    if (output_size_index > 0)
      --output_size_index;
    break;
  case '}':
    ++output_smooth_factor;
    break;
  case '{':
    if (output_smooth_factor > 1)
      --output_smooth_factor;
    break;
  case 'c':
    close_and_finish_current_contour();
    break;
  case 'f':
    finish_current_contour();
    break;
  case 'd':
    delete_current_contour();
    break;
  case 'D':
    if (input_mode == CONTOUR_MODE)
      contour_collection.delete_contours(current_level, 0.01);
    else
      ridge_collection.delete_ridges(current_level, 0.01);
    break;
  case 's':
    do_save();
    break;
  case 'h':
    do_help();
    break;
  case 'l':
    do_load();
    break;
  case '.':
    zoom *= 2;
    break;
  case ',':
    if (zoom > 1)
      zoom /= 2;
    break;
  case 8: // backspace
    if (current_contour) current_contour->pop_end();
    break;
  default:
    cerr << "Unkown key " << key << " int val = " << (int) key << endl;
    break;
  }

  // make sure we're all OK.
  float width = 1.0 / zoom;
  if (pos_x < 0)
    pos_x = 0;
  if (pos_y < 0)
    pos_y = 0;
  if (pos_x > (1.0 - width))
    pos_x = 1.0-width;
  if (pos_y > (1.0 - width))
    pos_y = 1.0-width;
  // redo the projection
  setup_projection();

  glutPostRedisplay();
}

void special(int key, int x, int y)
{
  float width = 1.0 / zoom;
  float delta = width/10.0;

  switch(key)
  {
  case GLUT_KEY_UP:
    pos_y += delta;
    break;
  case GLUT_KEY_DOWN:
    pos_y -= delta;
    break;
  case GLUT_KEY_LEFT:
    pos_x -= delta;
    break;
  case GLUT_KEY_RIGHT:
    pos_x += delta;
    break;
  default:
    cerr << "Unkown special key " << key << endl;
    break;
  }

  // make sure we're all OK.
  if (pos_x < 0)
    pos_x = 0;
  if (pos_y < 0)
    pos_y = 0;
  if (pos_x > (1.0 - width))
    pos_x = 1.0-width;
  if (pos_y > (1.0 - width))
    pos_y = 1.0-width;

  // redo the projection
  setup_projection();

  glutPostRedisplay();
}

void add_new_contour()
{
  if (current_contour)
  {
    cout << "Already doing a contour - press 'd' to delete it\n";
    return;
  }

  cout << "Adding new contour at level " << current_level << endl;
  
  current_contour = new Contour(current_level);
}

void finish_current_contour()
{
  if (!current_contour)
  {
    cout << "Need to add a contour first\n";
    return;
  }
  
  if (input_mode == CONTOUR_MODE)
  {
    cout << "Adding finished contour to collection\n";
    contour_collection.add_contour(*current_contour);
  }
  else
  {
    cout << "Adding finished ridge to collection\n";
    ridge_collection.add_ridge(*current_contour);
  }
    
  delete current_contour;

  current_contour = 0;
}
void close_and_finish_current_contour()
{
  if (!current_contour)
  {
    cout << "Need to add a contour first\n";
    return;
  }
  
  cout << "Closing current contour\n";
  current_contour->close();
  
  if (input_mode == CONTOUR_MODE)
  {
    cout << "Adding finished contour to collection\n";
    contour_collection.add_contour(*current_contour);
  }
  else
  {
    cout << "Adding finished ridge to collection\n";
    ridge_collection.add_ridge(*current_contour);
  }
  
  delete current_contour;

  current_contour = 0;
}

void delete_current_contour()
{
  if (!current_contour)
  {
    cout << "Need to add a contour first\n";
    return;
  }

  cout << "Deleting contour\n";
  
  delete current_contour;

  current_contour = 0;
}


void draw_map()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, map_texture->get_texture());
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  
  // draw a single quad
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex3f(0, 0, 0);
  glTexCoord2f(1, 0);
  glVertex3f(1, 0, 0);
  glTexCoord2f(1, 1);
  glVertex3f(1, 1, 0);
  glTexCoord2f(0, 1);
  glVertex3f(0, 1, 0);
  glEnd();
  
  glDisable(GL_TEXTURE_2D);
  glPopAttrib();
}

void do_text()
{
  int y;
  int dy = 4;
  int x = 5;

  float width = 1.0 / zoom;

  if (current_contour_end.y > (pos_y + width/2))
  {
    y = 2;
  }
  else
  {
    y = 98 - 4*dy;
  }
  
  static char s1[128];
  static char s2[128];
  static char s3[128];
  static char s4[128];
  sprintf(s1, "Current level = %5.1f", current_level);
  sprintf(s2, "Level increment = %5.1f", d_level);
  sprintf(s3, "Output: size = %d, smooth_factor = %d", 
          output_sizes[output_size_index],
          output_smooth_factor);
  sprintf(s4, "Mode: %s", input_mode == CONTOUR_MODE ? "Contour" : "Ridge");
  
  text_overlay.add_entry(x, y+3*dy, s1);
  text_overlay.add_entry(x, y+2*dy, s2);
  text_overlay.add_entry(x, y+dy, s3);
  text_overlay.add_entry(x, y, s4);
  
}

// We have to:
//
// Prepare a temporary output buffer, output_smooth_factor times
// bigger than the real output buffer.
//
// Render our contours onto it. We also need to store a boolean for
// each pixel to indicate if the value is to be set in stone.
//
// Interpolate the unset pixels.
//
// Sample the result to give the lower-resolution actual output.
//
// Output the final result.
void do_output()
{
  cout << "Preparing to write output...\n";

  Detailed_dem dem(output_sizes[output_size_index] * output_smooth_factor,
                   0, 0, 1, 1,
                   0.01);
  
//    Detailed_dem dem(16,
//                     0, 0, 1, 1,
//                     0.01);
  
  contour_collection.insert_in_dem(dem);
  ridge_collection.insert_in_dem(dem);
  
  dem.diffuse(0.1);

//  dem.show();
  dem.do_output("orography.tif", "contour.tif", "orography.raw", output_smooth_factor);
  
  cout << "Done the high-resolution DEM\n";
}

void do_save()
{
  cout << "Starting save\n";
  
  ofstream out_file("orography.con");
  
  if (!out_file)
  {
    cerr << "Can't open output file\n";
    return;
  }

  contour_collection.save(out_file);

  out_file << "====================\n";
  
  ridge_collection.save(out_file);

  cout << "Done saving to orography.con\n";
}

void do_load()
{
  cout << "Starting loading from orography.con\n";

  ifstream in_file("orography.con");
  
  if (!in_file)
  {
    cerr << "Can't open input file\n";
    return;
  }
  
  contour_collection.load(in_file);

  string junk;
  in_file >> junk;
  
  ridge_collection.load(in_file);

  cout << "Done loading\n";
}

void do_help()
{
  const char help[] = 
    "= and -  : Increase/decrease the current contour level\n"
    "+ and _  : adjust the contour interval for the = and - keys\n"
    "] and [  : adjust the size of the final grid.\n"
    "{ and }  : adjust the smoothing value used in the interpolation\n"
    "           I.e. the interpolation is initially done to a grid that\n"
    "           is this many times higher resolution than the final grid.\n"
    ". and ,  : zoom in/out\n"
    "s        : save the contours in orography.con\n"
    "l        : load contours from orography.con\n"
    "d        : delete the current contour\n"
    "D        : delete all contours at the curent level\n"
    "o        : Output to orography.raw and orography.tif\n"
    "m        : Toggles between contour and ridge mode\n"
    "f        : Finish the current contour/ridge\n"
    "c        : Close and finish the current contour/ridge\n"
    "backspace: Delete the most recent contour/ridge point\n"
    "left mouse button (release)   : mark a contour point\n"
    "right mouse button (hold down): draws a \"free-hand\" contour.\n";

  /////////////////////////////////////////////////////////////////MessageBox ( NULL , help , "MapTrace keys" , MB_OK);
  cout << help << endl;
}


