#ifndef PROFILE_H
#define PROFILE_H

//#include <IwProfileMenu.h>
//#include <IwGx.h>

#ifdef IW_USE_PROFILE

class CIwMenuItemProfile : public CIwMenuItem
{
public:
  CIwMenuItemProfile() : CIwMenuItem("", "Profile", 0, CIwMenuItem::MENU) {};
  void Select();
};


inline CIwMenu* DebugCreateMainMenu()
{
  CIwMenu* pMenu = new CIwMenu;
  pMenu->AddItem( new CIwMenuItemResManager );
  pMenu->AddItem( new CIwMenuItemGx );

  pMenu->AddItem( new CIwMenuItemProfile );

  return pMenu;
}

inline void createDebugMenu()
{
  new CIwMenuManager; 
  // Rendering and main menu construction callbacks
   IwGetMenuManager()->SetTextCallback(IwGxPrintMenuCallback);
   IwGetMenuManager()->SetMainMenuFn(DebugCreateMainMenu);

  new CIwProfileManager();
  CIwProfileManager::Get().SetActive(true);
}

inline void destroyDebugMenu()
{
   delete &CIwProfileMenu::Get();
   delete &CIwProfileManager::Get();

   delete IwGetMenuManager();
}

#endif

#endif

