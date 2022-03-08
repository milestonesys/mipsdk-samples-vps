#ifndef _OS_HELPER_H_
#define _OS_HELPER_H_

#include <string>
#include <cstdint>

class OSHelper
{
public:
  static std::string get_time_as_string(uint64_t time);
};

#endif // _OS_HELPER_H_
