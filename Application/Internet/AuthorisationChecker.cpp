#include "AuthorisationChecker.h"
#include "PicaDialog.h"
#include "VersionChecker.h"
#include <cstddef>
#include <cstdint>
#include "FonctionsMarmelade.h"
#include "SocketMarmelade.h"

// #include <IwHTTP.h>

enum HTTPsStatus
{
    kNone,
    kDownloading,
    kOK,
    kError,
};

#define HTTP_URI "http://www.rowlhouse.co.uk/PicaSim/Testing"
char authorisationFailureMessage[256] = "Failure";
#define EXIT exit(0)
const char expectedContents[] = "OK";
size_t expectedContentLength = sizeof(expectedContents);

static CIwHTTP* sHttpObject = 0;
static char* sResult = NULL;
static uint32_t len = 0;

//----------------------------------------------------------------------------------------------------------------------
static int32_t GotData(void*, void*)
{
  // This is the callback indicating that a ReadContent call has
  // completed.  Either we've finished, or a bigger buffer is
  // needed.  If the correct ammount of data was supplied initially,
  // then this will only be called once. However, it may well be
  // called several times when using chunked encoding.

  // Firstly see if there's an error condition.
  if (sHttpObject->GetStatus() == S3E_RESULT_ERROR)
  {
    // Something has gone wrong
    ShowDialog("PicaSim", authorisationFailureMessage, "OK");
    EXIT;
  }
  else if (sHttpObject->ContentReceived() != sHttpObject->ContentLength())
  {
    // We have some data but not all of it. We need more space.
    uint32_t oldLen = len;
    // If iwhttp has a guess how big the next bit of data is (this
    // basically means chunked encoding is being used), allocate
    // that much space. Otherwise guess.
    if (len < sHttpObject->ContentExpected())
      len = sHttpObject->ContentExpected();
    else
      len += 1024;

    // Allocate some more space and fetch the data.
    sResult = (char*)s3eRealloc(sResult, len);
    sHttpObject->ReadContent(&sResult[oldLen], len - oldLen, GotData);
  }
  else
  {
    if (sResult && strlen(sResult) > 0)
    {
      if (strncmp(expectedContents, sResult, expectedContentLength-1) == 0)
      {
        // We've got all the data. Display it.
        ShowDialog("PicaSim", "Test version authorised", "OK");
      }
      else
      {
        // Something has gone wrong
        ShowDialog("PicaSim", authorisationFailureMessage, "OK");
        EXIT;
      }
    }
  }
  return 0;
}


//----------------------------------------------------------------------------------------------------------------------
static int32_t GotHeaders(void*, void*)
{
  if (sHttpObject->GetStatus() == S3E_RESULT_ERROR)
  {
    // Something has gone wrong
    ShowDialog("PicaSim", authorisationFailureMessage, "OK");
    EXIT;
  }
  else
  {
    // Depending on how the server is communicating the content
    // length, we may actually know the length of the content, or
    // we may know the length of the first part of it, or we may
    // know nothing. ContentExpected always returns the smallest
    // possible size of the content, so allocate that much space
    // for now if it's non-zero. If it is of zero size, the server
    // has given no indication, so we need to guess. We'll guess at 1k.
    len = sHttpObject->ContentExpected();
    if (!len)
    {
      len = 1024;
    }

    s3eFree(sResult);
    sResult = (char*)s3eMalloc(len + 1);
    sResult[len] = 0;
    sHttpObject->ReadContent(sResult, len, GotData, NULL);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void InitAuthorisationChecker()
{
  TerminateAuthorisationChecker();

  sHttpObject = new CIwHTTP;
  int thisVersion = GetBuildNumber();
  char url[256];
  sprintf(url, "%s/%d.txt", HTTP_URI, thisVersion);
  sprintf(authorisationFailureMessage, "Failed to get authorisation for version %d", thisVersion);

  if (sHttpObject->Get(url, GotHeaders, NULL) != S3E_RESULT_SUCCESS)
  {
    ShowDialog("PicaSim", "Failed to get authorisation - no internet access?", "OK");
    EXIT;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TerminateAuthorisationChecker()
{
  delete sHttpObject;
  s3eFree(sResult);
}

