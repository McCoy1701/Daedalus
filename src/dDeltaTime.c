#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Daedalus.h"

double d_get_delta_time( dDeltaTime_t *time )
{
  time->lastTime = time->currentTime;
  time->currentTime = clock();
  double elapsedTime = (double)( time->currentTime - time->lastTime ) / CLOCKS_PER_SEC;
  return elapsedTime / 1000.0f;
}
