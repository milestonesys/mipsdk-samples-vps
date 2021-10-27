#ifndef _METADATA_H_
#define _METADATA_H_
/// <summary>
/// This class serves as a container for metadata, so we can copy data and queue it up in parallel with video frames
/// </summary>

#include "IData.h"
#include <string>

class MetaData : public IData
{
private:
  char * m_pData;
  int m_dataSize;
  unsigned long long m_timestamp;

public:
  MetaData(char * data, int size, unsigned long long timestamp);
  virtual ~MetaData();

  const char * GetDataPointer();
  int GetDataSize();
  unsigned long long GetTimestamp();
};

#endif // _METADATA_H_