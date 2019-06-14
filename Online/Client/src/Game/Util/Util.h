#pragma once
#include <math.h>

class UtilTool
{
public:
  static float scaleValue(float value) {
    return sin((value * 3.1415) - 1.57) * 0.5 + 0.5;
  }

};
