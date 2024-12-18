#ifndef PICA_DIALOG_H
#define PICA_DIALOG_H

#include "Menu.h"
#include "IwUI.h"


//---------------------------------------------------------------------------------------------------------------------
template<int NUM_BUTTONS>
class DialogHandler : public CIwUIElementEventHandler
{
public:
  DialogHandler() : mButtonClicked(-1) 
  {
    for (int i = 0 ; i != NUM_BUTTONS ; ++i)
      mButtons[i] = 0;
  }

  bool HandleEvent(CIwEvent* pEvent) OVERRIDE
  {
    if (pEvent->GetID() == IWUI_EVENT_BUTTON)
    {
      CIwManaged* sender = pEvent->GetSender();
      for (int i = 0 ; i != NUM_BUTTONS ; ++i)
        if (sender == mButtons[i])
          mButtonClicked = i;
      return true;
    }
    return false;
  }

  void MakeButton(int index, const char* txt, CIwUIElement* buttonArea, CIwUILayout* buttonLayout)
  {
    mButtons[index] = new CIwUIButton;
    mButtons[index]->AddEventHandler(this);
    mButtons[index]->SetStyle("<button_wide>");
    mButtons[index]->SetCaption(txt);
    buttonArea->AddChild(mButtons[index]);
    buttonLayout->AddElement(mButtons[index]);
  }

  int mButtonClicked;
  CIwUIButton* mButtons[NUM_BUTTONS];

};

int ShowDialog(
  const char* title, 
  const char* text, 
  const char* button0, 
  const char* button1 = 0, 
  const char* button2 = 0
);

#endif