#include "MetaData.h"

MetaData::MetaData(char * data, int size, unsigned long long timestamp) : m_dataSize(size), m_timestamp(timestamp)
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

MetaData::~MetaData()
{
  if (m_pData != nullptr)
  {
    delete[] m_pData;
    m_pData = nullptr;
    m_dataSize = 0;
  }
}

const char * MetaData::GetDataPointer()
{
  return m_pData;
}

int MetaData::GetDataSize()
{
  return m_dataSize;
}

unsigned long long MetaData::GetTimestamp()
{
  return m_timestamp;
}
