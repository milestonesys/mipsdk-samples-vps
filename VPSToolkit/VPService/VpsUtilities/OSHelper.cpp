#include "OSHelper.h"
#include <sstream> // for std::stringstreeam
#include <iomanip> // for std::setfill
#include <iostream>
#include <time.h>
#include <stdio.h>

std::string OSHelper::get_time_as_string(uint64_t msSinceEpoch)
{
  time_t timestamp = msSinceEpoch / 1000;
  struct tm info;
  bool convertedOk = false;

#ifdef _WIN32
  errno_t err = gmtime_s(&info, &timestamp);
  if (err == 0)
  {
    convertedOk = true;
  }
#else    
  if (gmtime_r(&timestamp, &info) != NULL)
  {
    convertedOk = true;
  }
#endif

  int year = convertedOk ? (info.tm_year + 1900) : 1970;
  int month = convertedOk ? (info.tm_mon + 1) : 1;
  int day = convertedOk ? info.tm_mday : 1;
  int hour = convertedOk ? info.tm_hour : 0;
  int minutes = convertedOk ? info.tm_min : 0;
  int seconds = convertedOk ? info.tm_sec : 0;
  int msec = convertedOk ? (msSinceEpoch % 1000) : 0;

  std::stringstream ss;
  ss << year << "-";
  ss << std::setfill('0') << std::setw(2);
  ss << month << "-";
  ss << std::setfill('0') << std::setw(2);
  ss << day << "T";
  ss << std::setfill('0') << std::setw(2);
  ss << hour << ":";
  ss << std::setfill('0') << std::setw(2);
  ss << minutes << ":";
  ss << std::setfill('0') << std::setw(2);
  ss << seconds << ".";
  ss << std::setfill('0') << std::setw(3);
  ss << msec << "+00:00";
  return ss.str();
}
