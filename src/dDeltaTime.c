#include <stdio.h>
#include <stdlib.h>

float getDeltaTime( unsigned int *currentTime, unsigned int *lastTime )
{
  unsigned int elapsedTime = currentTime - lastTime;
  *lastTime = *currentTime;
  return ( float )elapsedTime / 1000.0f;
}
