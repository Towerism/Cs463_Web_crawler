#pragma once

#include "Stats.h"
#include <cstdarg>

inline void printIfNoStats(const char* format, ...)
{
  if (SharedData::Stats::trackStats())
    return;
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}