#include "Helpers.h"
#include "Trace.h"

//#include <IwUtil.h>

//---------------------------------------------------------------------------------------------------------------------
void ApplyRollPitchYawToRotationDegrees(float roll, float pitch, float yaw, glm::vec3& rotation)
{
  if (roll == 0.0f && pitch == 0.0f && yaw == 0.0f)
    return;

  float angle = DegreesToRadians(glm::length(rotation));
  if (angle > 0.0f)
    glm::normalize(rotation);
  else
    rotation = glm::vec3(1,0,0);

  btQuaternion quat1(Vector3ToBulletVector3(rotation), angle);
  btMatrix3x3 m1(quat1);

  btMatrix3x3 m2;
  m2.setEulerZYX(DegreesToRadians(roll), DegreesToRadians(pitch), DegreesToRadians(yaw));
  m1 = m1 * m2;

  m1.getRotation(quat1);
  rotation = BulletVector3ToVector3(quat1.getAxis());
  rotation *= quat1.getAngle();

  rotation *= 180.0f / PI;
}

