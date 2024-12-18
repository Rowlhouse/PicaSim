#include "PicaDialog.h"
#include "Trace.h"
#include "Menu.h"

//---------------------------------------------------------------------------------------------------------------------
int ShowDialog(const char* title, const char* text, const char* button0, const char* button1, const char* button2)
{
  TRACE("ShowDialog: %s", text);

  int surfaceWidth = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
  int surfaceHeight = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

  int width = surfaceWidth/2;
  int height = surfaceHeight/2;

  bool createBackground = IwGetUIView()->GetNumElements() == 0;

  // Background
  s3eDeviceYield();
  CIwUIImage* backgroundImage = 0;
  Texture* backgroundTexture = createBackground ? AddCentredImage("Menus/StartBackground.jpg", true, &backgroundImage) : 0;
  s3eDeviceYield();

  // Set the layout
  CIwUILayoutVertical* dialogLayout = new CIwUILayoutVertical;
  dialogLayout->SetSizeToSpace(true);

  CIwUIImage* dialogArea  = new CIwUIImage;
  dialogArea->SetSizeToContent(false); // If true the the whole screen is vertically centred. False places it at the top
  dialogArea->SetSizeMax(CIwVec2(width,height));
  dialogArea->SetLayout(dialogLayout);
  CIwTexture* texture = (CIwTexture*)IwGetResManager()->GetResNamed("DialogBackground", "CIwTexture");
  dialogArea->SetTexture(texture);

  IwGetUIView()->AddElement(dialogArea);
  IwGetUIView()->AddElementToLayout(dialogArea);

  // ====== Title
  CIwUILabel* titleLabel = new CIwUILabel;
  titleLabel->SetCaption(title);
  titleLabel->SetStyle("<label_dialog_title>");
  dialogArea->AddChild(titleLabel);
  dialogLayout->AddElement(titleLabel);

  // ====== Text
  CIwUILabel* textLabel = new CIwUILabel;
  textLabel->SetCaption(text);
  textLabel->SetStyle("<label_dialog_text>");
  textLabel->SetProperty("alignV", IW_UI_ALIGN_TOP);
  textLabel->SetSizeMax(CIwVec2(-1,-1));
  textLabel->SetSizeToContent(false);
  dialogArea->AddChild(textLabel);
  dialogLayout->AddElement(textLabel);

  // ====== Button area
  CIwUILayoutHorizontal* buttonLayout = new CIwUILayoutHorizontal;
  buttonLayout->SetSizeToSpace(true);

  CIwUIElement* buttonArea = new CIwUIElement;
  buttonArea->SetSizeToContent(true);
  buttonArea->SetLayout(buttonLayout);

  dialogArea->AddChild(buttonArea);
  dialogLayout->AddElement(buttonArea);

  DialogHandler<3> dialogHandler;

  // Actual buttons
  const char* buttonTxts[3] = {button0, button1, button2};
  int numButtons = 0;
  for (int i = 0 ; i != 3 ; ++i)
  {
    if (buttonTxts[i])
    {
      dialogHandler.MakeButton(i, buttonTxts[i], buttonArea, buttonLayout);
      dialogHandler.mButtons[i]->SetVisible(false);
      ++numButtons;
    }
  }

  // Update loop

  uint64 lastTime = s3eTimerGetMs();

  // Zooming params
  static float smoothTime = 0.12f;
  static float dampingRatio = 0.7f;
  CIwVec2 finalPosition(surfaceWidth/2 - width/2, surfaceHeight/2 - height/2);
  float offsetX = 0.5f * surfaceWidth + 0.5f * width;
  float offsetY = 0.0f;
  float offsetXRate = -offsetX / smoothTime;
  float offsetYRate = -offsetY / smoothTime;

  dialogHandler.mButtonClicked = -1;
  float totalTime = 0.0f;
  while (!s3eDeviceCheckQuitRequest() && dialogHandler.mButtonClicked < 0)
  {
    uint64 currentTime = s3eTimerGetMs();
    int32 updateTime = currentTime > lastTime ?
      MIN((int32)(currentTime - lastTime), 100) : 0;
    lastTime = currentTime;
    float dt = updateTime * 0.001f;
    totalTime += dt;

    int titleHeight = titleLabel->GetSize().y;
    int buttonHeight = buttonArea->GetSize().y;
    int textHeight = height - (titleHeight + buttonHeight);
    textLabel->SetSizeMin(CIwVec2(width, textHeight));

    if (numButtons > 1)
    {
      int buttonWidthTotal = 0;
      for (int i = 0 ; i != 3 ; ++i)
      {
        if (dialogHandler.mButtons[i])
        {
          buttonWidthTotal += dialogHandler.mButtons[i]->GetSize().x;
        }
      }
      int space = width - buttonWidthTotal;
      space /= (numButtons - 1);
      buttonLayout->SetSpacing(space);
    }

    if (totalTime > 0.2f)
    {
      for (int i = 0 ; i != 3 ; ++i)
      {
        if (dialogHandler.mButtons[i])
          dialogHandler.mButtons[i]->SetVisible(true);
      }
    }

    // Clear frame buffer
    IwGxSetColClear(128, 128, 128, 255);
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
    IwGetUIController()->Update();
    IwGetUIView()->Update(updateTime);

    // Update dialog position
    SmoothSpringDamper(offsetX, offsetXRate, dt, 0.0f, smoothTime, dampingRatio);
    SmoothSpringDamper(offsetY, offsetYRate, dt, 0.0f, smoothTime, dampingRatio);

    CIwVec2 offset((int32) offsetX, (int32) offsetY);
    CIwVec2 position = finalPosition + offset;
    dialogArea->SetPos(position);

    IwGetUIView()->Render();
    IwGxFlush();

    //Render Debugging outlines of the element hierarchy
    static bool debugLayout = false;
    if (debugLayout)
      IwUIDebugRender(NULL, IW_UI_DEBUG_LAYOUT_OUTLINE_F | IW_UI_DEBUG_ELEMENT_OUTLINE_F);

    s3eDeviceYield(1);

    IwGxSwapBuffers();
  }

  IwGetUIView()->RemoveElement(dialogArea);
  if (backgroundImage)
    IwGetUIView()->RemoveElement(backgroundImage);

  for (int i = 0 ; i != 3 ; ++i)
  {
    if (dialogHandler.mButtons[i])
      dialogHandler.mButtons[i]->RemoveEventHandler(&dialogHandler);
  }

  delete dialogArea;
  delete backgroundImage;

  delete backgroundTexture;
  TRACE("Clicked %d %s", dialogHandler.mButtonClicked, dialogHandler.mButtonClicked >= 0 ? buttonTxts[dialogHandler.mButtonClicked] : "Cancelled");
  return dialogHandler.mButtonClicked;
}

