/*
     PUI Auxiliary Widget Library
     Derived from PLIB, the Portable Game Library by Steve Baker.

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

     $Id: puAux.h,v 1.3 2004/09/22 21:09:35 smokydiamond Exp $
*/

#ifndef _PUI_AUX_WIDGETS_H_
#define _PUI_AUX_WIDGETS_H_ 1

#include "pu.h"

// Widget Class Bit Patterns
// PUI classes take up 0x00000001 through 0x01000000

#define PUCLASS_FILESELECTOR        0x00040000 /* Because FilePicker is obsolete */
#define PUCLASS_BISLIDER            0x00080000
#define PUCLASS_TRISLIDER           0x00100000
#define PUCLASS_VERTMENU            0x00200000
#define PUCLASS_LARGEINPUT          0x00400000
#define PUCLASS_COMBOBOX            0x00800000
#define PUCLASS_SELECTBOX           0x01000000
#define PUCLASS_SPINBOX             0x02000000
#define PUCLASS_SCROLLBAR           0x04000000
#define PUCLASS_BISLIDERWITHENDS    0x02000000
#define PUCLASS_SLIDERWITHINPUT     0x04000000
#define PUCLASS_COMPASS             0x08000000
#define PUCLASS_CHOOSER             0x10000000


// Widget Declarations
class puaSpinBox          ;
class puaFileSelector     ;
class puaBiSlider         ;
class puaTriSlider        ;
class puaVerticalMenu     ;
class puaLargeInput       ;
class puaComboBox         ;
class puaSelectBox        ;
class puaScrollBar        ;
class puaBiSliderWithEnds ;
class puaCompass          ;
class puaSliderWithInput  ;
class puaChooser          ;


// A File selector widget
class puaFileSelector : public puDialogBox
{
  UL_TYPE_DATA

protected:
  char** files ;
  char*  dflag ;
  int num_files   ;
  int arrow_count ;

  char *startDir ;

  void find_files ( void ) ;
  static void handle_select ( puObject* ) ;
  static void input_entered ( puObject* ) ;

  puFrame   *frame         ;
  puListBox *list_box      ;
  puSlider  *slider        ;
  puOneShot *cancel_button ;
  puOneShot *ok_button     ;
  puInput   *input         ;
  puArrowButton *up_arrow       ;
  puArrowButton *down_arrow     ;
  puArrowButton *fastup_arrow   ;
  puArrowButton *fastdown_arrow ;

  void puaFileSelectorInit ( int x, int y, int w, int h,
                             int arrows, const char *dir, const char *title ) ;

public:

  puaFileSelector ( int x, int y, int w, int h, int arrows,
                    const char *dir, const char *title = "Pick a file" ) :
      puDialogBox ( x, y )
  {
    puaFileSelectorInit ( x, y, w, h, arrows, dir, title ) ;
  }

  puaFileSelector ( int x, int y, int w, int h,
                    const char *dir, const char *title = "Pick a file" ) :
      puDialogBox ( x, y )
  {
    puaFileSelectorInit ( x, y, w, h, 1, dir, title ) ;
  }

  puaFileSelector ( int x, int y, int arrows,
                    const char *dir, const char *title = "Pick a file" ) :
      puDialogBox ( x, y )
  {
    puaFileSelectorInit ( x, y, 220, 170, arrows, dir, title ) ;
  }

  puaFileSelector ( int x, int y,
                    const char *dir, const char *title = "Pick a file" ) :
      puDialogBox ( x, y )
  {
    puaFileSelectorInit ( x, y, 220, 170, 1, dir, title ) ;
  }

  ~puaFileSelector () ;

  /* Not for application use!! */
  puInput *__getInput ( void ) const { return input ; }
  char *__getStartDir ( void ) const { return (char *) startDir ; }

  void setInitialValue ( const char *fname ) ;
  void setSize ( int w, int h ) ;
} ;


// An input box that takes an arbitrary number of lines of input
class puaLargeInput : public puInputBase, public puGroup
{
  UL_TYPE_DATA

protected:
  int num_lines ;              // Number of lines of text in the box
  int lines_in_window ;        // Number of lines showing in the window
  int top_line_in_window ;     // Number of the first line in the window
  float max_width ;            // Width of longest line of text in box, in pixels
  int slider_width ;

  puFrame *frame ;

  puSlider *bottom_slider ;    // Horizontal slider at bottom of window
  puScrollBar *right_slider ;  // Vertical slider at right of window

  char *wrapped_text ;         // Pointer to word-wrapped text in the box

  int arrow_count ;          // Number of up/down arrows above and below the right slider

  void normalizeCursors ( void ) ;
  void removeSelectRegion ( void ) ;

  void wrapText ( void ) ;

public:
  puaLargeInput ( int x, int y, int w, int h, int arrows, int sl_width, int wrap_text = FALSE ) ;
  ~puaLargeInput ()
  {
    delete [] wrapped_text ;

    if ( puActiveWidget() == this )
      puDeactivateWidget () ;
  }

  void setSize ( int w, int h ) ;

  int getNumLines ( void ) const {  return num_lines ;  }
  int getLinesInWindow ( void ) const { return lines_in_window ; }
  void setTopLineInWindow ( int val ) {  top_line_in_window = (val<0) ? 0 : ( (val>num_lines-2) ? num_lines-2 : val ) ;  }

  void draw     ( int dx, int dy ) ;
  int  checkHit ( int button, int updown, int x, int y ) ;
  void doHit    ( int button, int updown, int x, int y ) ;
  int  checkKey ( int key, int updown ) ;

  void setSelectRegion ( int s, int e ) ;
  void selectEntireLine ( void ) ;

  void invokeDownCallback ( void )
  {
    rejectInput () ;
    normalizeCursors () ;
    if ( down_cb != NULL ) (*down_cb)(this) ;
  }

  void  setValue ( const char *s ) ;
  void  setText ( const char *l ) { setValue ( l ) ; }  /* DEPRECATED */
  char *getText ( void ) { return getStringValue () ; }  /* DEPRECATED */
  char *getWrappedText ( void )
  {
    return ( wrapped_text == NULL ? getStringValue () : wrapped_text ) ;
  }
  void  addNewLine ( const char *l ) ;
  void  addText ( const char *l ) ;
  void  appendText ( const char *l ) ;
  void  removeText ( int start, int end ) ;
} ;


// A box that contains a set of alternatives that drop down when the user clicks on the down-arrow
class puaComboBox : public puGroup
{
  UL_TYPE_DATA

protected:
  char ** list  ;
  int num_items ;

  int curr_item ;

  puInput *input ;
  puArrowButton *arrow_btn ;
  puPopupMenu *popup_menu  ;

  static void input_down_cb ( puObject *inp   ) ;
  static void handle_arrow ( puObject *arrow  ) ;
  static void handle_popup ( puObject *popupm ) ;

  void update_widgets ( void ) ;

public:
  /* Not for application use ! */
  puPopupMenu * __getPopupMenu ( void ) const { return popup_menu ; }

  void newList ( char ** _list ) ;
  int  getNumItems ( void ) const { return num_items ; }

  int  getCurrentItem ( void ) ;
  void setCurrentItem ( int item )
  {
    if ( ( item >= 0 ) && ( item < num_items ) )
    {
      curr_item = item ;
      update_widgets () ;

      invokeCallback () ;
    }
  }

  void setCurrentItem ( const char *item_ptr ) ;

  void setPosition ( int x, int y )
  {
    puGroup::setPosition ( x, y ) ;

    /* Ensure that popup menu will show up at the right place */
    newList ( list ) ;
  }

  void setSize ( int w, int h ) ;

  void draw ( int dx, int dy ) ;
  int  checkHit ( int button, int updown, int x, int y ) ;
  int  checkKey ( int key, int updown ) ;

  puaComboBox ( int minx, int miny, int maxx, int maxy,
                char **list, int editable = TRUE ) ;
} ;


// Like a menu bar, but the selections are one above the other
class puaVerticalMenu : public puGroup
{
  UL_TYPE_DATA

protected:
public:
  puaVerticalMenu ( int x = -1, int y = -1 ) :

  puGroup ( x < 0 ? puGetWindowWidth() -
                     ( puGetDefaultLegendFont().getStringWidth ( " " )
                       + PUSTR_TGAP + PUSTR_BGAP ) : x,

          y < 0 ? puGetWindowHeight() -
                   ( puGetDefaultLegendFont().getStringHeight ()
                     + PUSTR_TGAP + PUSTR_BGAP ) : y)
  {
    type |= PUCLASS_VERTMENU ;
    floating = TRUE ;  // DEPRECATED! -- we need to replace this code.
    if ( y < 0 ) { setVStatus( TRUE ) ; } /* It is now supposed to stick to the top left - JCJ*/
  }

  void add_submenu ( const char *str, char *items[], puCallback _cb[],
                     void *_user_data[] = NULL ) ;
  void close ( void ) ;
} ;


// Not sure what this one does ...
class puaSelectBox : public puGroup
{
  UL_TYPE_DATA

protected:
  char ** list  ;
  int num_items ;

  int curr_item ;

  puInput *input ;
  puArrowButton *down_arrow ;
  puArrowButton *up_arrow   ;

  static void handle_arrow ( puObject *arrow ) ;

  void update_widgets ( void ) ;

public:
  void newList ( char ** _list ) ;
  int  getNumItems ( void ) const { return num_items ; }

  int  getCurrentItem ( void ) const { return curr_item ; }
  void setCurrentItem ( int item )
  {
    if ( ( item >= 0 ) && ( item < num_items ) )
    {
      curr_item = item ;
      update_widgets () ;

      invokeCallback () ;
    }
  }

  void setSize ( int w, int h ) ;
  void draw ( int dx, int dy ) ;
  int  checkKey ( int key, int updown ) ;

  puaSelectBox ( int minx, int miny, int maxx, int maxy,
                 char **list ) ;
} ;


// A slider with up- and down-arrows on its ends
class puaScrollBar : public puSlider
{
  UL_TYPE_DATA

protected:
  int arrow_count ;
  int fast_up_arrow_active, fast_down_arrow_active ;
  int up_arrow_active, down_arrow_active ;

public:
  void doHit ( int button, int updown, int x, int y ) ;
  void draw  ( int dx, int dy ) ;
  puaScrollBar ( int minx, int miny, int sz, int arrows, int vertical = FALSE ) :
      puSlider ( minx, miny, sz, vertical )
  {
    type |= PUCLASS_SCROLLBAR ;
    arrow_count = arrows ;
    fast_up_arrow_active = fast_down_arrow_active = up_arrow_active = down_arrow_active = FALSE ;
  }

  /* Alternate constructor which lets you explicitly set width */

  puaScrollBar ( int minx, int miny, int sz, int arrows, int vertical, int width ) :
      puSlider ( minx, miny, sz, vertical, width )
  {
    type |= PUCLASS_SCROLLBAR ;
    arrow_count = arrows ;
    fast_up_arrow_active = fast_down_arrow_active = up_arrow_active = down_arrow_active = FALSE ;
  }

  void setMaxValue ( float f )
  {
    maximum_value = f ;
    slider_fraction = 1.0f / ( getMaxValue() - getMinValue() + 1.0f ) ;
    puPostRefresh () ;
  }


  void setMinValue ( float i )
  {
    minimum_value = i ;
    slider_fraction = 1.0f / ( getMaxValue() - getMinValue() + 1.0f ) ;
    puPostRefresh () ;
  }
} ;



// A puSlider with two slide boxes instead of one
class puaBiSlider : public puSlider
{
  UL_TYPE_DATA

protected:
  float current_max ;
  float current_min ;

  int active_button ;  // Zero for none, one for min, two for max
public:
  void doHit ( int button, int updown, int x, int y ) ;
  void draw  ( int dx, int dy ) ;
  puaBiSlider ( int minx, int miny, int sz, int vertical = FALSE ) :
     puSlider ( minx, miny, sz, vertical )
  {
    type |= PUCLASS_BISLIDER ;
    setMaxValue ( 1.0f ) ;
    setMinValue ( 0.0f ) ;
    setStepSize ( 1.0f ) ;
    current_max = 1.0f ;
    current_min = 0.0f ;
    active_button = 0 ;
  }

  /* Alternate constructor which lets you explicitly set width */

  puaBiSlider ( int minx, int miny, int sz, int vertical, int width ) :
     puSlider ( minx, miny, sz, vertical, width )
  {
    type |= PUCLASS_BISLIDER ;
    setMaxValue ( 1.0f ) ;
    setMinValue ( 0.0f ) ;
    setStepSize ( 1.0f ) ;
    current_max = 1.0f ;
    current_min = 0.0f ;
    active_button = 0 ;
  }

  void setMaxValue ( float i )
  {
    maximum_value = i ;
    slider_fraction = 1.0f / ( getMaxValue() - getMinValue() + 1.0f ) ;
    puPostRefresh () ;
  }


  void setMinValue ( float i )
  {
    minimum_value = i ;
    slider_fraction = 1.0f / ( getMaxValue() - getMinValue() + 1.0f ) ;
    puPostRefresh () ;
  }

  void setCurrentMax ( int i ) { current_max = (float) i ; puPostRefresh () ; } /* DEPRECATED */
  void setCurrentMax ( float f ) { current_max = f ; puPostRefresh () ; }
  float getCurrentMax ( void ) const { return current_max ; }

  void setCurrentMin ( int i ) { current_min = (float) i ; puPostRefresh () ; } /* DEPRECATED */
  void setCurrentMin ( float f ) { current_min = f ; puPostRefresh () ; }
  float getCurrentMin ( void ) const { return current_min ; }

  void setActiveButton ( int i ) { active_button = i ; }
  int getActiveButton ( void ) const { return active_button ; }
} ;


// A puSlider with three slide boxes

class puaTriSlider : public puaBiSlider
{
  UL_TYPE_DATA

protected:
  // "active_button" is now zero for none, one for min, two for middle, three for max
  int freeze_ends ;  // true to make end sliders unmovable
public:
  void doHit ( int button, int updown, int x, int y ) ;
  void draw  ( int dx, int dy ) ;
  puaTriSlider ( int minx, int miny, int sz, int vertical = FALSE ) :
     puaBiSlider ( minx, miny, sz, vertical )
  {
    type |= PUCLASS_TRISLIDER ;
    freeze_ends = TRUE ;
  }

  /* Alternate constructor which lets you explicitly set width */

  puaTriSlider ( int minx, int miny, int sz, int vertical, int width ) :
     puaBiSlider ( minx, miny, sz, vertical, width )
  {
    type |= PUCLASS_TRISLIDER ;
    freeze_ends = TRUE ;
  }

  int getFreezeEnds ( void ) const { return freeze_ends ; }
  void setFreezeEnds ( int val )   { freeze_ends = val ; puPostRefresh () ; }
} ;


// A vertical puBiSlider with a puInput box above it showing the current maximum
// value and a puInput box below it showing the current minimum value

class puaBiSliderWithEnds : public puGroup
{
  UL_TYPE_DATA

protected:
  puaBiSlider *slider ;
  puInput *max_box ;
  puInput *min_box  ;

  static void handle_slider ( puObject *obj ) ;
  static void handle_max ( puObject *obj  ) ;
  static void handle_min ( puObject *obj ) ;
  static void input_down_callback ( puObject *obj ) ;

  void update_widgets ( void ) ;

public:
  // For internal use only:
  void __setMax ( float f ) { max_box->setValue ( f ) ; }
  void __setMin ( float f ) { min_box->setValue ( f ) ; }

  // For public use:

  void draw ( int dx, int dy ) ;
  int  checkHit ( int button, int updown, int x, int y )
  {
    return puGroup::checkHit ( button, updown, x, y ) ;
  }

  int  checkKey ( int key, int updown ) ;

  puaBiSliderWithEnds ( int minx, int miny, int maxx, int maxy ) ;

  void setSize ( int w, int h ) ;

  void setMaxValue ( float f ) { slider->setMaxValue ( f ) ; }
  float getMaxValue ( void ) const { return slider->getMaxValue () ; }
  void setMinValue ( float f ) { slider->setMinValue ( f ) ; }
  float getMinValue ( void ) const { return slider->getMinValue () ; }

  void setCurrentMax ( float f )
  {
    slider->setCurrentMax ( f ) ;
    max_box->setValue ( f ) ;
  }

  float getCurrentMax ( void ) const { return slider->getCurrentMax () ; }

  void setCurrentMin ( float f )
  {
    slider->setCurrentMin ( f ) ;
    min_box->setValue ( f ) ;
  }

  float getCurrentMin ( void ) const { return slider->getCurrentMin () ; }

  void setActiveButton ( int i ) { slider->setActiveButton ( i ) ; }
  int getActiveButton ( void ) const { return slider->getActiveButton () ; }

  char *getValidData ( void ) const { return max_box->getValidData () ; }
  void setValidData ( char *data ) { max_box->setValidData ( data ) ; min_box->setValidData ( data ) ; }
  void addValidData ( char *data ) { max_box->addValidData ( data ) ; min_box->addValidData ( data ) ; }

  void setCBMode ( int m ) { slider->setCBMode ( m ) ; }
  int getCBMode ( void ) const { return slider->getCBMode () ; }

  void setDelta ( float f ) { slider->setDelta ( f ) ; }
  float getDelta ( void ) const { return slider->getDelta () ; }

  float getStepSize ( void ) const { return slider->getStepSize () ; }
  void setStepSize ( float f )     { slider->setStepSize ( f )     ; }
} ;


// A vertical puSlider with a puInput box above or below it showing its value

class puaSliderWithInput : public puGroup
{
  UL_TYPE_DATA

protected:
  puSlider *slider ;
  puInput *input_box ;
  int input_position ;

  static void handle_slider ( puObject *obj ) ;
  static void handle_input ( puObject *obj ) ;
  static void input_down_callback ( puObject *obj ) ;

  void update_widgets ( void ) ;

public:
  // For internal use only:
  void __setInputBox ( float f ) { input_box->setValue ( f ) ; }

  // For public use:

  void draw ( int dx, int dy ) ;
  int  checkHit ( int button, int updown, int x, int y )
  {
    return puGroup::checkHit ( button, updown, x, y ) ;
  }

  int  checkKey ( int key, int updown ) ;

  puaSliderWithInput ( int minx, int miny, int maxx, int maxy, int above = 0 ) ;

  void setSize ( int w, int h ) ;

  void setMaxValue ( float f ) { slider->setMaxValue ( f ) ; }
  float getMaxValue ( void ) const { return slider->getMaxValue () ; }
  void setMinValue ( float f ) { slider->setMinValue ( f ) ; }
  float getMinValue ( void ) const { return slider->getMinValue () ; }

  void setValue ( int i ) { slider->setValue ( i ) ;  input_box->setValue ( i ) ; }
  void setValue ( float f ) { slider->setValue ( f ) ;  input_box->setValue ( f ) ; }
  virtual void setValue ( const char *s ) { slider->setValue ( s ) ; }
  virtual void setValue ( bool b ) { slider->setValue ( b ) ; }

  int   getIntegerValue ( void ) { return slider->getIntegerValue () ; }
  float getFloatValue ( void )   { return slider->getFloatValue ()   ; }
  char  getCharValue ( void )    { return slider->getCharValue ()    ; }
  char *getStringValue ( void )  { return slider->getStringValue ()  ; }
  bool  getBooleanValue ( void ) { return slider->getBooleanValue () ; }

  char *getValidData ( void ) const { return input_box->getValidData () ; }
  void setValidData ( char *data ) { input_box->setValidData ( data ) ; }
  void addValidData ( char *data ) { input_box->addValidData ( data ) ; }

  void setCBMode ( int m ) { slider->setCBMode ( m ) ; }
  int getCBMode ( void ) const { return slider->getCBMode () ; }

  void setDelta ( float f ) { slider->setDelta ( f ) ; }
  float getDelta ( void ) const { return slider->getDelta () ; }

  float getStepSize ( void ) const { return slider->getStepSize () ; }
  void setStepSize ( float f )     { slider->setStepSize ( f )     ; }
} ;

class puaSpinBox : public puRange, public puGroup
{
  UL_TYPE_DATA

protected :
  puInput *input_box ;
  puArrowButton *up_arrow ;
  puArrowButton *down_arrow ;

  int arrow_position ;

public :
  /* Whether the arrows are on the LEFT of the input box (0) or RIGHT (1 DEFAULT) */
  int getArrowPosition ( void ) const { return arrow_position ; }
  /* Offered as a proportion of the input box height. Default = 0.5 */
  void setArrowHeight ( float height )
  {
    puBox ibox = *(input_box->getABox()) ;
    int size = int(height * ( ibox.max[1] - ibox.min[1] )) ;
    up_arrow->setSize ( size, size ) ;
    down_arrow->setSize ( size, size ) ;
    int xpos = getArrowPosition () ? ibox.max[0] : ibox.min[0] - size ;
    int ymid = ( ibox.max[1] + ibox.min[1] ) / 2 ;
    if ( getArrowPosition () == 0 )  /* Arrows are on the left, adjust the x-position of the input box */
    {
      ibox.min[0] -= xpos ;
      input_box->setPosition ( ibox.min[0], ibox.min[1] ) ;
      abox.min[0] += xpos ;
      xpos = 0 ;
    }

    if ( height > 0.5f )  /* Adjust the input box to be up from the bottom */
    {
      input_box->setPosition ( ibox.min[0], ibox.min[1] + size - ymid ) ;
      abox.min[1] += ymid - size ;
      ymid = size ;
    }
    else  /* Input box is at the bottom of the group area */
    {
      input_box->setPosition ( ibox.min[0], 0 ) ;
      abox.min[1] += ibox.min[1] ;
    }

    up_arrow->setPosition ( xpos, ymid ) ;
    down_arrow->setPosition ( xpos, ymid - size ) ;
    recalc_bbox() ;
  }

  float getArrowHeight ( void ) const
  {
    int awid, ahgt, iwid, ihgt ;
    input_box->getSize ( &iwid, &ihgt ) ;
    up_arrow->getSize ( &awid, &ahgt ) ;
    return float(ahgt) / float(ihgt) ;
  }

  puaSpinBox ( int minx, int miny, int maxx, int maxy, int arrow_pos = 1 ) ;

  void setValue ( float f ) { puValue::setValue ( f ) ;  input_box->setValue ( f ) ; }
  void setValue ( int i ) { puValue::setValue ( i ) ;  input_box->setValue ( i ) ; }
  void setValue ( const char *s ) { puValue::setValue ( s ) ;  input_box->setValue ( s ) ; }
  void setValue ( puValue *pv ) { puValue::setValue ( pv ) ;  input_box->setValue ( pv ) ; }
} ;


/*
 * Widget that looks like a 3-d coordinate system with quarter-circles in the coordinate planes.
 * It is used in 3-d modeling to translate and rotate the scene.  The coordinates rotate with
 * the scene but do not translate.  Defined values are:
 *  0 - Nothing active
 *  1 - Dot at origin active:  reset (usually)
 *  2 - X-axis dot active:  rotate about y- and z-axes
 *  3 - Y-axis dot active:  rotate about z- and x-axes
 *  4 - Z-axis dot active:  rotate about x- and y-axes
 *  5 - X-axis bar active:  translate along x-axis
 *  6 - Y-axis bar active:  translate along y-axis
 *  7 - Z-axis bar active:  translate along z-axis
 *  8 - XY-plane arc active:  rotate about z-axis
 *  9 - YZ-plane arc active:  rotate about x-axis
 * 10 - ZX-plane arc active:  rotate about y-axis
 * 11 - X-axis, Y-axis, and XY-arc active:  translate in xy-plane
 * 12 - Y-axis, Z-axis, and YZ-arc active:  translate in yz-plane
 * 13 - Z-axis, X-axis, and ZX-arc active:  translate in zx-plane
 */
#define PUACOMPASS_INACTIVE          0
#define PUACOMPASS_RESET             1
#define PUACOMPASS_ROTATE_Y_Z        2
#define PUACOMPASS_ROTATE_Z_X        3
#define PUACOMPASS_ROTATE_X_Y        4
#define PUACOMPASS_TRANSLATE_X       5
#define PUACOMPASS_TRANSLATE_Y       6
#define PUACOMPASS_TRANSLATE_Z       7
#define PUACOMPASS_ROTATE_Z          8
#define PUACOMPASS_ROTATE_X          9
#define PUACOMPASS_ROTATE_Y         10
#define PUACOMPASS_TRANSLATE_X_Y    11
#define PUACOMPASS_TRANSLATE_Y_Z    12
#define PUACOMPASS_TRANSLATE_Z_X    13

class puaCompass : public puObject
{
  UL_TYPE_DATA

protected :
  sgQuat rotation ;
  sgVec3 translation ;
  float point_size ;
  int button_state ;
  int trigger_button ;
  float mouse_x, mouse_y, mouse_z ;
  float translation_sensitivity ;

  float xint, yint, zint ;
  float prev_angle ;
  sgQuat prev_rotation ;

public :
  puaCompass ( int minx, int miny, int maxx, int maxy ) : puObject ( minx, miny, maxx, maxy )
  {
    setValue ( PUACOMPASS_INACTIVE ) ;

//    sgSetQuat ( rotation, 1.0f, 0.0f, 0.0f, 0.0f ) ;
//    sgSetQuat ( rotation, 0.707107f, 0.707107f, 0.0f, 0.0f ) ;
//    sgSetQuat ( rotation, 0.5f, -0.5f, 0.5f, 0.5f ) ;
//    sgSetQuat ( rotation, 0.866025f, -0.166667f, 0.166667f, 0.166667f ) ;
    sgSetQuat ( rotation, 0.866025f, -0.408248f, 0.288675f, 0.0f ) ;
    sgSetVec3 ( translation, 0.0f, 0.0f, 0.0f ) ;

    point_size = 10.0f ;

    button_state = PU_UP ;
    trigger_button = PU_LEFT_BUTTON ;
    mouse_x = mouse_y = mouse_z = 0.0f ;
    translation_sensitivity = 1.0f ;

    xint = yint = zint = 0.0f ;
    prev_angle = 0.0f ;
    sgCopyQuat ( prev_rotation, rotation ) ;
  }

  void draw ( int dx, int dy ) ;
  void doHit ( int button, int updown, int x, int y ) ;

  // Accessors and mutators
  void getRotation ( sgQuat q ) const  {  memcpy ( q, rotation, 4 * sizeof(sgFloat) ) ;  }
  void setRotation ( sgQuat q )  {  memcpy ( rotation, q, 4 * sizeof(sgFloat) ) ;  }
  void setRotation ( sgFloat t, sgFloat x, sgFloat y, sgFloat z )
  {
    sgFloat sinth = sgSin ( t / 2.0f ) ;
    sgFloat norm = sgSqrt ( x * x + y * y + z * z ) ;
    if ( norm == 0.0 ) norm = 1.0 ;
    rotation[SG_W] = sgCos ( t / 2.0f ) ;
    rotation[SG_X] = sinth * x / norm ;
    rotation[SG_Y] = sinth * y / norm ;
    rotation[SG_Z] = sinth * z / norm ;
  }

  void getTranslation ( sgVec3 t ) const  {  memcpy ( t, translation, 3 * sizeof(sgFloat) ) ;  }
  void setTranslation ( sgVec3 t )  {  memcpy ( translation, t, 3 * sizeof(sgFloat) ) ;  }
  void setTranslation ( sgFloat x, sgFloat y, sgFloat z )  {  translation[SG_X] = x ;  translation[SG_Y] = y ;  translation[SG_Z] = z ;  }

  float getPointSize () const  {  return point_size ;  }
  void setPointSize ( float p )  {  point_size = p ;  }

  int getTriggerButton () const  {  return trigger_button ;  }
  void setTriggerButton ( int b )  {  trigger_button = b ;  }

  float getTranslationSensitivity () const  {  return translation_sensitivity ;  }
  void setTranslationSensitivity ( float t )  {  translation_sensitivity = t ;  }
} ;


class puaChooser
{
  UL_TYPE_DATA

  puButton    *chooser_button ;
  puPopupMenu *popup_menu ;

  int x1, y1, x2, y2 ;

  static void static_popup_cb ( puObject * ) ;
  static void static_menu_cb  ( puObject * ) ;

public:

  virtual ~puaChooser ()
  {
    delete chooser_button ;
    delete popup_menu     ;
  }

  puaChooser ( int _x1, int _y1, int _x2, int _y2, char *legend ) ;

  void add_item ( char *str, puCallback _cb, void *_user_data = NULL ) ;
  void close () ;

  void popup_cb () ;

  void menuCleanup  ( const char *s ) ;

  void hide   () { chooser_button -> hide   () ; popup_menu -> hide   () ; }
  void reveal () { chooser_button -> reveal () ; popup_menu -> hide   () ; }

  static void menuCleanup  ( puObject * ) ;
} ;


#endif

