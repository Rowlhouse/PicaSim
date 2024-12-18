#include "VersionChecker.h"
#include "Framework.h"

#include <IwHTTP.h>

enum HTTPsStatus
{
    kNone,
    kDownloading,
    kOK,
    kError,
};

#define HTTP_URI "http://www.rowlhouse.co.uk/PicaSim/version.txt"

static const int PICASIM_BUILD_NUMBER = 11085;
static bool sIsNewVersionAvailable = false;
static CIwHTTP* sHttpObject = 0;
static char* sResult = NULL;
static uint32 len = 0;
static HTTPsStatus sStatus = kNone;

//----------------------------------------------------------------------------------------------------------------------
int GetBuildNumber()
{
  return PICASIM_BUILD_NUMBER;
}
//----------------------------------------------------------------------------------------------------------------------
bool IsNewVersionAvailable()
{
  return sIsNewVersionAvailable;
}

//----------------------------------------------------------------------------------------------------------------------
static int32 GotData(void*, void*)
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
    sStatus = kError;
  }
  else if (sHttpObject->ContentReceived() != sHttpObject->ContentLength())
  {
    // We have some data but not all of it. We need more space.
    uint32 oldLen = len;
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
    // We've got all the data. Display it.
    sStatus = kOK;
    int version = -1;
    if (sResult && strlen(sResult) > 0)
      version = atoi(sResult);
    if (version > PICASIM_BUILD_NUMBER)
    {
      TRACE("New version available - current = %d available = %d\n", PICASIM_BUILD_NUMBER, version);
      sIsNewVersionAvailable = true;
    }
    else
    {
      TRACE("No new version available - current = %d available = %d\n", PICASIM_BUILD_NUMBER, version);
    }
  }
  return 0;
}


//----------------------------------------------------------------------------------------------------------------------
static int32 GotHeaders(void*, void*)
{
  if (sHttpObject->GetStatus() == S3E_RESULT_ERROR)
  {
    // Something has gone wrong
    sStatus = kError;
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
void InitVersionChecker()
{
  TerminateVersionChecker();

  sHttpObject = new CIwHTTP;
  if (sHttpObject->Get(HTTP_URI, GotHeaders, NULL) == S3E_RESULT_SUCCESS)
    sStatus = kDownloading;
}

//----------------------------------------------------------------------------------------------------------------------
void TerminateVersionChecker()
{
  delete sHttpObject;
  s3eFree(sResult);
}

