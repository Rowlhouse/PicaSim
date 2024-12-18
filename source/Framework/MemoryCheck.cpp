#include "MemoryCheck.h"

#if USE_PARANOID_MEMTEST
#include <s3e.h>
#include <IwUtil.h>

enum
{
  DEBUG_BUCKET_GUARDED = IW_MEM_BUCKET_ID_USER, /// DEBUG_BUCKET_GUARDED must be less id than 
  ///
  //... rest
};

CIwMemBucket* guarded = NULL;

class CIwMemBucketParanoidScopeTest
{
private:
  CIwMemBucket* b;
public:
  CIwMemBucketParanoidScopeTest(CIwMemBucket* memb):b(memb)
  {
    IW_CALLSTACK("ParanoidTest Begin");
    b->DebugTestIntegrity();
  }
  ~CIwMemBucketParanoidScopeTest()
  {
    IW_CALLSTACK("ParanoidTest End");
    b->DebugTestIntegrity();
  }
};

#define PARANOID(t) IW_CALLSTACK(t); CIwMemBucketParanoidScopeTest p(this)
#define SAFE_DELETE(p) delete p

class CIwMemBucketParanoid:public CIwMemBucket
{
private:
  CIwMemBucket* _w;
  int _reallocId;
public:
  CIwMemBucketParanoid(CIwMemBucket* w):CIwMemBucket(),_w(w), _reallocId(0)
  {
    SetName("Paranoid");
  };
  ~CIwMemBucketParanoid()
  {
    SAFE_DELETE(_w);
  }

  virtual void    Free(void* item)
  {
    PARANOID("Free");
    _w->Free(item);
  }
  virtual void*   Realloc(void* item, int32 size)
  {
    PARANOID("Realloc");
    ++_reallocId;
    char* result = (char*) _w->Realloc(item, size);
    if(!result)
    {
      printf("CIwMemBucketParanoid::Realloc item = %p, size = %d returns NULL! #%d\n", item, size, _reallocId);
      return NULL;
    }
    //printf("CIwMemBucketParanoid::Realloc item = %p, size = %d returns %p! #%d\n", item, size, result, _reallocId);

    //if(_reallocId == 73357) 
    //    IwDebugBreak();
    return result;
  }
  virtual int32   Owns(void* item)
  {
    PARANOID("Owns");
    return _w->Owns(item);
  }
  virtual int32   DebugTestIntegrity()
  {
    return _w->DebugTestIntegrity();
  }
  virtual int32   GetSize(void* item)
  {
    PARANOID("GetSize");
    return _w->GetSize(item);
  }
  virtual int32   GetMemUsage(void* item)
  {
    PARANOID("GetMemUsage");
    return _w->GetMemUsage(item);
  }
  virtual int32   GetFree()
  {
    PARANOID("GetFree");
    return _w->GetFree();
  }
  virtual int32   GetUsed()
  {
    PARANOID("GetUsed");
    return _w->GetUsed();
  }
  virtual int32   GetLargestFreeBlock() 
  {
    PARANOID("GetLargestFreeBlock");
    return _w->GetLargestFreeBlock();
  }
  virtual int32   GetFragmentation() 
  {
    PARANOID("GetFragmentation");
    return _w->GetFragmentation();
  }
  virtual int32   GetTotalSize()
  {
    PARANOID("GetTotalSize");
    return _w->GetTotalSize();
  }
  virtual int32   IterateBlocks(IwMemBlockIterator* fn, void* user) 
  {
    PARANOID("IterateBlocks");
    return _w->IterateBlocks(fn, user);
  }
};

#undef PARANOID


void InitMemoryOverrunCheck()
{
  IW_CALLSTACK("InitMemoryOverrunCheck"); 
  IwUtilInit();

  CIwMemBucket* bucket = IwMemBucketGet();
  guarded = new CIwMemBucketGuarded(bucket, false);
  guarded = new CIwMemBucketParanoid(guarded);

  IwMemBucketRegister(DEBUG_BUCKET_GUARDED, "DEBUG_BUCKET", guarded);
  IwMemBucketSet(DEBUG_BUCKET_GUARDED);
}

#else

void InitMemoryOverrunCheck() {}

#endif// USE_PARANOID_MEMTEST
