#include "../Platform/S3ECompat.h"

struct JoystickData 
{
    JoystickData();
    enum {MAX_ANALOGUEINPUTS=8, MAX_POVDIRECTIONS=4, MAX_BUTTONS=32};
    long mAnalogueInputs[MAX_ANALOGUEINPUTS];
    unsigned long mPOVDirections[MAX_POVDIRECTIONS];
    unsigned char mButtons[MAX_BUTTONS];
    char mName[512];
};

s3eBool JoystickAvailable();

s3eResult GetJoystickStatus(JoystickData& joystick, int id);

void CalibrateJoystick();

void UpdateJoystick(int id);

void ResetJoystick();

bool ShowJoystickInGame(const struct GameSettings& gameSettings);
