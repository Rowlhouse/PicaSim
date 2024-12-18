#ifndef ENTITY_H
#define ENTITY_H

#include <s3eTypes.h>

class Entity
{
public:
  Entity() {}
  virtual ~Entity() {}

  virtual void EntityUpdate(float deltaTime, int entityLevel) = 0;
};

#endif
