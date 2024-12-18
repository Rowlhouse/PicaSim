/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker
 
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
 
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
 
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 
     For further information visit http://plib.sourceforge.net

     $Id: ssgaSky.h,v 1.3 2003/09/26 14:54:20 sjbaker Exp $
*/

// Written by Curtis Olson, started December 1997.
// SSG-ified by Curtis Olson, February 2000.
//
// Moved into ssgAux, July 2003.

#ifndef _INCLUDED_SSGASKY_H_
#define _INCLUDED_SSGASKY_H_

#include "ssg.h"

class ssgaCelestialBody;
class ssgaCelestialBodyList;
class ssgaCloudLayer;
class ssgaCloudLayerList;
class ssgaStars;
class ssgaSkyDome;
class ssgaSky;


class ssgaCelestialBody
{
private:

  ssgTransform *transform;
  ssgColourArray *cl;

  // used by repaint for rise/set effects
  double body_angle;
  double body_rotation;

  // used by reposition
  double body_right_ascension;
  double body_declination;
  double body_dist;

public:

  ssgaCelestialBody( void );
  ~ssgaCelestialBody( void );

  ssgBranch *build( const char* body_tex_path, const char* halo_tex_path, double body_size );
  ssgBranch *build( ssgSimpleState *orb_state, ssgSimpleState *halo_state, double body_size );

  bool reposition( sgVec3 p, double angle ) {
    return reposition ( p, angle, body_right_ascension, body_declination, body_dist ); }
  bool reposition( sgVec3 p, double angle, double rightAscension, double declination, double dist );

  bool repaint() { return repaint ( body_angle ); }
  bool repaint( double angle );

  void getPosition ( sgCoord* p )
  {
	sgMat4 Xform;
	transform->getTransform(Xform);
	sgSetCoord(p, Xform);
  }

  void setAngle ( double angle ) { body_angle = angle; }
  double getAngle () { return body_angle; }

  void setRotation ( double rotation ) { body_rotation = rotation; }
  double getRotation () { return body_rotation; }

  void setRightAscension ( double ra ) { body_right_ascension = ra; }
  double getRightAscension () { return body_right_ascension; }

  void setDeclination ( double decl ) { body_declination = decl; }
  double getDeclination () { return body_declination; }

  void setDist ( double dist ) { body_dist = dist; }
  double getDist () { return body_dist; }

  inline float *getColor() { return  cl->get( 0 ); }
} ;


class ssgaCelestialBodyList : private ssgSimpleList
{
public:

  ssgaCelestialBodyList ( int init = 3 )
	  : ssgSimpleList ( sizeof(ssgaCelestialBody*), init ) { }

  ssgaCelestialBodyList::~ssgaCelestialBodyList () { removeAll(); }

  int getNum (void) { return total ; }

  ssgaCelestialBody* get ( unsigned int n )
  {
    assert(n<total);
    return *( (ssgaCelestialBody**) raw_get ( n ) ) ;
  }

  void add ( ssgaCelestialBody* item ) { raw_add ( (char *) &item ) ;}

  void removeAll ()
  {
    for ( int i = 0; i < getNum (); i++ )
      delete get (i) ;
    ssgSimpleList::removeAll () ;
  }
} ;


class ssgaCloudLayer
{
private:

  ssgRoot *layer_root;
  ssgTransform *layer_transform;
  ssgLeaf *layer[4];

  ssgColourArray *cl[4]; 
  ssgVertexArray *vl[4];
  ssgTexCoordArray *tl[4];

  bool enabled;
  float layer_span;
  float layer_asl;
  float layer_thickness;
  float layer_transition;
  float scale;
  float speed;
  float direction;

  double last_lon, last_lat;
  double last_x, last_y;

public:

  ssgaCloudLayer( void );
  ~ssgaCloudLayer( void );

  void build( const char *cloud_tex_path, float span, float elevation, float thickness, float transition );
  void build( ssgSimpleState *cloud_state, float span, float elevation, float thickness, float transition );

  bool repositionFlat( sgVec3 p, double dt );
  bool reposition( sgVec3 p, sgVec3 up, double lon, double lat, double alt, double dt );

  bool repaint( sgVec3 fog_color );

  void draw();

  void enable() { enabled = true; }
  void disable() { enabled = false; }
  bool isEnabled() { return enabled; }

  float getElevation () { return layer_asl; }
  void  setElevation ( float elevation ) { layer_asl = elevation; }

  float getThickness () { return layer_thickness; }
  void  setThickness ( float thickness ) { layer_thickness = thickness; }

  float getTransition () { return layer_transition; }
  void  setTransition ( float transition ) { layer_transition = transition; }

  float getSpeed () { return speed; }
  void  setSpeed ( float val ) { speed = val; }

  float getDirection () { return direction; }
  void  setDirection ( float val ) { direction = val; }
};


class ssgaCloudLayerList : private ssgSimpleList
{
public:

  ssgaCloudLayerList ( int init = 3 )
	  : ssgSimpleList ( sizeof(ssgaCloudLayer*), init ) { }

  ssgaCloudLayerList::~ssgaCloudLayerList () { removeAll(); }

  int getNum (void) { return total ; }

  ssgaCloudLayer* get ( unsigned int n )
  {
    assert(n<total);
    return *( (ssgaCloudLayer**) raw_get ( n ) ) ;
  }

  void add ( ssgaCloudLayer* item ) { raw_add ( (char *) &item ) ;}

  void removeAll ()
  {
    for ( int i = 0; i < getNum (); i++ )
      delete get (i) ;
    ssgSimpleList::removeAll () ;
  }
} ;


class ssgaStars
{
private:

  ssgTransform *stars_transform;
  ssgSimpleState *state;

  ssgColourArray *cl;
  ssgVertexArray *vl;

  int old_phase;  // data for optimization

public:

  ssgaStars( void );
  ~ssgaStars( void );

  ssgBranch *build( int num, sgdVec3 *star_data, double star_dist );

  bool reposition( sgVec3 p, double angle );

  bool repaint( double sol_angle, int num, sgdVec3 *star_data );
};


class ssgaSkyDome
{
private:

  ssgTransform *dome_transform;
  ssgSimpleState *dome_state;

  ssgVertexArray *center_disk_vl;
  ssgColourArray *center_disk_cl;

  ssgVertexArray *upper_ring_vl;
  ssgColourArray *upper_ring_cl;

  ssgVertexArray *middle_ring_vl;
  ssgColourArray *middle_ring_cl;

  ssgVertexArray *lower_ring_vl;
  ssgColourArray *lower_ring_cl;

public:

  ssgaSkyDome( void );
  ~ssgaSkyDome( void );

  ssgBranch *build( double hscale = 80000.0, double vscale = 80000.0 );

  bool repositionFlat( sgVec3 p, double spin );
  bool reposition( sgVec3 p, double lon, double lat, double spin );

  bool repaint( sgVec3 sky_color, sgVec3 fog_color, double sol_angle, double vis );
};


class ssgaSky
{
private:

  // components of the sky
  ssgaSkyDome *dome;
  ssgaCelestialBody* sol_ref;
  ssgaCelestialBodyList bodies;
  ssgaCloudLayerList clouds;
  ssgaStars *planets;
  ssgaStars *stars;

  ssgRoot *pre_root, *post_root;

  ssgSelector *pre_selector, *post_selector;
  ssgTransform *pre_transform, *post_transform;
  ssgTransform *bodies_transform, *stars_transform;

  // visibility
  float visibility;
  float effective_visibility;

  // near cloud visibility state variables
  bool in_puff;
  double puff_length;       // in seconds
  double puff_progression;  // in seconds
  double ramp_up;           // in seconds
  double ramp_down;         // in seconds

public:

  ssgaSky( void );
  ~ssgaSky( void );

  void build( double h_radius, double v_radius,
	  int nplanets, sgdVec3 *planet_data,
	  int nstars, sgdVec3 *star_data);

  ssgaCelestialBody* addBody( const char *body_tex_path, const char *halo_tex_path, double size, double dist, bool sol = false );
  ssgaCelestialBody* addBody( ssgSimpleState *orb_state, ssgSimpleState *halo_state, double size, double dist, bool sol = false );
  ssgaCelestialBody* getBody(int i) { return bodies.get(i); }
  int getBodyCount() { return bodies.getNum(); }

  ssgaCloudLayer* addCloud( const char *cloud_tex_path, float span, float elevation, float thickness, float transition );
  ssgaCloudLayer* addCloud( ssgSimpleState *cloud_state, float span, float elevation, float thickness, float transition );
  ssgaCloudLayer* getCloud(int i) { return clouds.get(i); }
  int getCloudCount() { return clouds.getNum(); }

  bool repositionFlat( sgVec3 view_pos, double spin, double dt );
  bool reposition( sgVec3 view_pos, sgVec3 zero_elev, sgVec3 view_up, double lon, double lat, double alt, double spin, double gst, double dt );

  bool repaint( sgVec4 sky_color, sgVec4 fog_color, sgVec4 cloud_color, double sol_angle,
	  int nplanets, sgdVec3 *planet_data,
	  int nstars, sgdVec3 *star_data );

  // modify visibility based on cloud layers, thickness, transition range, and simulated "puffs".
  void modifyVisibility( float alt, float time_factor );

  // draw background portions of sky (do this before you draw rest of your scene).
  void preDraw();

  // draw translucent clouds (do this after you've drawn all oapaque elements of your scene).
  void postDraw( float alt );

  // enable the sky
  inline void enable() {
    pre_selector->select( 1 );
    post_selector->select( 1 );
  }

  // disable the sky
  inline void disable() {
    pre_selector->select( 0 );
    post_selector->select( 0 );
  }

  // current effective visibility
  inline float getVisibility() const { return effective_visibility; }
  inline void setVisibility( float v ) {
    effective_visibility = visibility = v;
  }
} ;


// return a random number between [0.0, 1.0)
inline double ssgaRandom(void)
{
  return(rand() / (double)RAND_MAX);
}

//#if defined( macintosh )
//const float system_gamma = 1.4;
//#elif defined (sgi)
//const float system_gamma = 1.7;
//#else	// others
const float system_gamma = 2.5;
//#endif

// simple architecture independant gamma correction function.
inline void ssgaGammaCorrectRGB(float *color, float reff = 2.5, float system = system_gamma)
{
  color[0] = (float)pow(color[0], reff/system);
  color[1] = (float)pow(color[1], reff/system);
  color[2] = (float)pow(color[2], reff/system);
};

inline void ssgaGammaCorrectC(float *color, float reff = 2.5, float system = system_gamma)
{
  *color = (float)pow(*color, reff/system);
};

inline void ssgaGammaRestoreRGB(float *color, float reff = 2.5, float system = system_gamma)
{
  color[0] = (float)pow(color[0], system/reff);
  color[1] = (float)pow(color[1], system/reff);
  color[2] = (float)pow(color[2], system/reff);
};

inline void ssgaGammaRestoreC(float *color, float reff = 2.5, float system = system_gamma)
{
  *color = (float)pow(*color, system/reff);
};

#endif
