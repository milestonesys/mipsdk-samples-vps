#ifndef _MEDIADATA_H_
#define _MEDIADATA_H_
/// <summary>
/// This class serves as a container for video (or audio) frames, so we can copy data and queue it up in parallel with metadata
/// </summary>

#include "IData.h"
#include <string>

class MediaData : public IData
{
private:
  char * m_pData;
  int m_dataSize;

public:
  MediaData(char * data, int size);
  virtual ~MediaData();
  const char * GetDataPointer();
  int GetDataSize();
};
#endif // _MEDIADATA_H_


