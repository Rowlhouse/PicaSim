#ifndef FILEMENU_H
#define FILEMENU_H

#include <string>

void RemoveExtension(char* filename, const char* extension);
void RemovePath(char* filename);

struct IncludeCallback
{
  virtual ~IncludeCallback() {}
  virtual bool GetInclude(const char* filename) = 0;
};

enum FileMenuType
{
  FILEMENUTYPE_FILE,
  FILEMENUTYPE_AEROPLANE,
  FILEMENUTYPE_SCENERY,
  FILEMENUTYPE_CHALLENGE,
  FILEMENUTYPE_LIGHTING
};

enum FileMenuResult
{
  FILEMENURESULT_SELECTED,
  FILEMENURESULT_CANCEL,
  FILEMENURESULT_ALT,
  FILEMENURESULT_PURCHASE,
};

/// Displays the menu based on the files in the paths, and returns the file name to be saved/loaded. 
FileMenuResult FileMenuLoad(
  std::string& result,
  const struct GameSettings& gameSettings, 
  const char* systemPath, 
  const char* userPath, 
  const char* extension, 
  const char* title,
  const char* tabTitles[],
  const size_t numTabs,
  const char* cancelButtonText,
  const char* altButtonText,
  FileMenuType fileMenuType = FILEMENUTYPE_FILE,
  float imagesPerScreen = -1.0f,
  IncludeCallback* includeCallback = 0);

FileMenuResult FileMenuSave(
  std::string& result,
  const struct GameSettings& gameSettings, 
  const char* userPath, 
  const char* extension, 
  const char* title, 
  FileMenuType fileMenuType = FILEMENUTYPE_FILE);

void FileMenuDelete(
  const struct GameSettings& gameSettings, 
  const char* userPath, 
  const char* extension, 
  const char* title, 
  FileMenuType fileMenuType = FILEMENUTYPE_FILE);

#endif
