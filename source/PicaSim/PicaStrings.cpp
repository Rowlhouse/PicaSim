#include "PicaStrings.h"
#include "GameSettings.h"

#include <IwDebug.h>


const char* gPicaStringsEN[PS_NUM_STRINGS];
const char* gPicaStringsDE[PS_NUM_STRINGS];
const char* gPicaStringsPT[PS_NUM_STRINGS];
const char* gPicaStringsFR[PS_NUM_STRINGS];

const char** gPicaStrings[LANG_NUM] = {
  gPicaStringsEN,
  gPicaStringsDE,
  gPicaStringsPT,
  gPicaStringsFR
};

extern void InitStringsEN(const char** strings);
extern void InitStringsDE(const char** strings);
extern void InitStringsPT(const char** strings);
extern void InitStringsFR(const char** strings);

void InitPicaStrings()
{
  InitStringsEN(gPicaStringsEN);

  // Make all non-english default to english
  for (size_t l = 0 ; l != LANG_NUM ; ++l)
  {
    if (l == LANG_EN)
      continue;
    for (size_t i = 0 ; i != PS_NUM_STRINGS ; ++i)
      gPicaStrings[l][i] = gPicaStrings[LANG_EN][i];
  }

  InitStringsDE(gPicaStringsDE);
  InitStringsPT(gPicaStringsPT);
  InitStringsFR(gPicaStringsFR);

#ifdef _DEBUG
  for (size_t l = 0 ; l != LANG_NUM ; ++l)
  {
    if (l == LANG_EN)
      continue;
    printf("#######################################\n");
    printf("Unset strings in %s\n", gPicaStrings[l][PS_LANGUAGE_EN]);
    printf("#######################################\n");
    for (size_t i = 0 ; i != PS_NUM_STRINGS ; ++i)
    {
      if (gPicaStrings[l][i] == gPicaStrings[LANG_EN][i])
      {
        printf("%s\n", gPicaStrings[l][i]);
      }
    }
  }
#endif
}

const char* GetPS(PicaString fs, Language l)
{
  IwAssert(ROWLHOUSE, fs < PS_NUM_STRINGS);
  IwAssert(ROWLHOUSE, l < LANG_NUM);
  return gPicaStrings[l][fs];
}
