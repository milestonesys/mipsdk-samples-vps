#include "MediaData.h"

  MediaData::MediaData(char * data, int size) : m_dataSize(size)
  {
    // We allocate a new copy of the data, because GStreamer may delete the data at "data" out of our control.
    if (size > 0)
    {
      m_pData = new char[size];
      if (m_pData != nullptr)
      {
        memcpy(m_pData, data, size);
      }
    }
  }

  MediaData::~MediaData()
  {
    if (m_pData != nullptr)
    {
      delete[] m_pData;
      m_pData = nullptr;
      m_dataSize = 0;
    }
  }

  const char * MediaData::GetDataPointer()
  {
    return m_pData;
  }

  int MediaData::GetDataSize()
  {
    return m_dataSize;
  }

