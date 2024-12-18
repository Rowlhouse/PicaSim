#include "Profile.h"

#ifdef IW_USE_PROFILE

void CIwMenuItemProfile::Select()
{
  // Create debug menu
   new CIwProfileMenu();
  CIwProfileManager::Get().SetActive(true);
}

#endif
