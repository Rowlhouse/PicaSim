#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller
{
public:
  enum Channel
  {
    CHANNEL_AILERONS  = 0,
    CHANNEL_ELEVATOR  = 1,
    CHANNEL_RUDDER    = 2,
    CHANNEL_THROTTLE  = 3,
    CHANNEL_LOOKYAW   = 4,
    CHANNEL_LOOKPITCH = 5,
    CHANNEL_AUX1      = 6,
    CHANNEL_SMOKE1    = 7,
    CHANNEL_SMOKE2    = 8,
    CHANNEL_HOOK      = 9,
    MAX_CHANNELS      = 10
  };

  virtual ~Controller() {}

  /// Controls range from -1 to +1
  virtual float GetControl(Channel channel) const = 0;

  /// Elevator trim control from -1 to 1
  virtual float GetElevatorTrim() const {return 0.0f;}

  // control is ControllerSettings::ControllerControls
  virtual void SetInputControl(int control, float value) {}
  virtual float GetInputControl(int control) const {return 0.0f;}

};

#endif
