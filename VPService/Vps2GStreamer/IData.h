#ifndef _IDATA_H_
#define _IDATA_H_
/// <summary>
/// This class serves as a base class for MediaData and MetaData
/// It facilitates that we can maintain a queue containing a mixture of instances of those classes
/// </summary>

#include <gst/gst.h>

class IData
{
public:
  virtual ~IData() {};
  virtual const char * GetDataPointer() = 0;
  virtual int GetDataSize() = 0;
};

#endif // _IDATA_H_
