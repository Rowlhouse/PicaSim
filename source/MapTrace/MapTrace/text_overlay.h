/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/

/*
  Handles the drawing of text over the top of the display. Should be
  called only after everything else has been drawn.
*/
#ifndef TEXT_OVERLAY_H
#define TEXT_OVERLAY_H

#include <string>
#include <vector>
using namespace std;

//! Stores and displays text as a 2D overlay
class Text_overlay
{
public:
  Text_overlay() {};
  
  //! Add a text entry - (x,y) are from bottom left, each 0-100
  int add_entry(int x, int y, const char * text);
  void reset() {entries.clear();} //!< clears all the entries
  void display(); 
  
private:
  
  //! Helper structure for Text_overlay
  struct Entry
  {
    Entry(const char * text, int x, int y) : text(text), x(x), y(y) {};
    string text;
    int x, y;
  };
  
  enum {MAX_ENTRIES = 128}; //!< in case someone forgets to call reset
  vector<Entry> entries;
};

#endif
