#ifndef LOADINGSCREENHELPER_H
#define LOADINGSCREENHELPER_H

class LoadingScreenHelper
{
public:
  virtual ~LoadingScreenHelper() {}

  virtual void Update(const char* moduleName = 0) = 0;
};

#endif