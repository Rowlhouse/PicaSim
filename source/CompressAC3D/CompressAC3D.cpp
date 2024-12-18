#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <cstring>

//---------------------------------------------------------------------------------------------------------------------
void ConvertLine(char* buff, int line, bool compress)
{
  int key = line + 13;
  const int start = 32;
  const int end = 126;
  const int range = end - start;
  const int offset = compress ? (key % range) : -(key % range);
  while (*buff != 0)
  {
    int c = *buff;
    if (c >= start && c < end)
    {
      c -= start;
      c = (c + range + offset) % range;
      *buff = c + start;
    }
    ++buff;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void PrintUsage()
{
  printf("Usage: CompressAC3D.exe OPTIONS source destination\n");
  printf("Where OPTIONS can be\n");
  printf("-c compress (default)\n");
  printf("-d decompress\n");
}

//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    PrintUsage();
    return 1;
  }
  const char* sourceFile = argv[argc-2];
  const char* destFile = argv[argc-1];
  bool compress = true;

  if (strcmp(argv[1], "-d") == 0)
    compress = false;

  if (
    strlen(sourceFile) < 2 ||
    strlen(destFile) < 2 ||
    strcmp(sourceFile, destFile) == 0
    )
  {
    PrintUsage();
    return 1;
  }

  FILE* s = fopen(sourceFile, "rb");
  if (!s)
  {
    printf("Error opening source file %s\n", sourceFile);
    return 1;
  }

  FILE* d = fopen(destFile, "wb");
  if (!d)
  {
    printf("Error opening destination file %s\n", destFile);
    fclose(s);
    return 1;
  }

  const size_t buffSize = 1024;
  char buff[buffSize];

  int line = 0;
  while (!feof(s))
  {
    if (fgets(buff, buffSize, s))
    {
      buff[buffSize-1] = 0;
      ConvertLine(buff, line++, compress);
      fputs(buff, d);
    }
  }


  fclose(s);
  fclose(d);
}