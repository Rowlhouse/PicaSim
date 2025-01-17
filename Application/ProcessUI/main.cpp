#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

//---------------------------------------------------------------------------------------------------------------------
const char* gInputFile = "iwui_768_propertysets.itx";
const char* gDir = "../../data/iwui_style";
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
struct OutputSetting
{
  OutputSetting(const std::string& filename, float scale)
    : mFilename(filename)
    , mScale(scale) {}
  std::string mFilename;
  float mScale;
};

//---------------------------------------------------------------------------------------------------------------------
typedef std::vector<OutputSetting> OutputSettings;
OutputSettings gOutputSettings;
typedef std::vector<std::string> Lines;

//---------------------------------------------------------------------------------------------------------------------
std::string GetAdjustedLine(const std::string& line, const OutputSetting& os)
{
  std::string result = line;

  // All substitutions are of the form blah { X Y }
  if (
    result.find("margin") < 5 ||
    result.find("sizeMin") < 5 ||
    result.find("sizeMax") < 5 ||
    result.find("sizeHint") < 5 ||
    result.find("boxSize") < 5 ||
    result.find("markerSize") < 5 ||
    result.find("cursorSize") < 5
    )
  {
    // Adjust the X Y values

    std::string::size_type leftBracketPos = result.find("{");
    std::string::size_type rightBracketPos = result.find("}");

    std::string numbers(result.begin() + leftBracketPos + 1, result.begin() + rightBracketPos);
    size_t nChars = numbers.length();

    char X[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
    char Y[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
    size_t iX = 0;
    size_t iY = 0;

    size_t iChar = 0;
    for ( ; iChar != nChars ; ++iChar)
    {
      if (numbers[iChar] != ' ')
        break;
    }
    for ( ; iChar != nChars ; ++iChar)
    {
      if (numbers[iChar] == ' ' || !numbers[iChar])
        break;
      X[iX++] = numbers[iChar];
    }
    for ( ; iChar != nChars ; ++iChar)
    {
      if (numbers[iChar] != ' ')
        break;
    }
    for ( ; iChar != nChars ; ++iChar)
    {
      if (numbers[iChar] == ' ' || !numbers[iChar])
        break;
      Y[iY++] = numbers[iChar];
    }

    int x = atoi(X);
    int y = atoi(Y);

    float offset = 0.0f;
    float newX = offset + (x - offset) * os.mScale;
    float newY = offset + (y - offset) * os.mScale;

    int newx = (int) (newX + 0.5f);
    int newy = (int) (newY + 0.5f);
    if (x < 1)
      newx = 1;
    if (y < 1)
      newy = 1;

    result = std::string(line.begin(), line.begin() + leftBracketPos);
    char extra[32];
    sprintf(extra, "{ %d %d }", x > 0 ? newx : x, y > 0 ? newy : y);
    result += extra;
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  gOutputSettings.push_back(OutputSetting("iwui_240_propertysets.itx",  1.28f *  240/768.0f));
  gOutputSettings.push_back(OutputSetting("iwui_320_propertysets.itx",  1.08f *  320/768.0f));
  gOutputSettings.push_back(OutputSetting("iwui_480_propertysets.itx",  1.0f  *  480/768.0f));
  gOutputSettings.push_back(OutputSetting("iwui_640_propertysets.itx",  1.0f  *  640/768.0f));
  gOutputSettings.push_back(OutputSetting("iwui_1080_propertysets.itx", 1.0f  * 1080/768.0f));
  gOutputSettings.push_back(OutputSetting("iwui_1536_propertysets.itx", 1.0f  * 1536/768.0f));
  gOutputSettings.push_back(OutputSetting("iwui_2048_propertysets.itx", 1.0f  * 2048/768.0f));

  std::string inputFile(gDir);
  inputFile += "/";
  inputFile += gInputFile;

  std::ifstream input( inputFile );

  Lines lines;
  std::string line;
  for( std::string line; getline( input, line ); )
  {
    lines.push_back(line);
  }

  for (OutputSettings::iterator it = gOutputSettings.begin() ; it != gOutputSettings.end() ; ++it)
  {
    const OutputSetting& outputSetting = *it;
    std::ofstream output(std::string(gDir) + "/" + outputSetting.mFilename);
    for (Lines::iterator linesIt = lines.begin() ; linesIt != lines.end() ; ++linesIt)
    {
      const std::string& line = *linesIt;
      std::string newLine = GetAdjustedLine(line, outputSetting);
      output << newLine << std::endl;
    }
  }
}