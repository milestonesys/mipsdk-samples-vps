
#include "GenericByteData.h"
#include <string.h>
namespace VpsUtilities
{

  GenericByteData::GenericByteData(unsigned char * data, unsigned int length, bool shouldGenerateHeader, bool shouldCopyData)
    : m_length(length)
  {
    if (shouldGenerateHeader) // When shouldGenerateHeader = true, shouldCopyData is assumed true 
    {
      m_length = length + HEADER_LENGTH;
      m_pData = new unsigned char[m_length];
      memcpy(m_pData + HEADER_LENGTH, data, m_length - HEADER_LENGTH);
      InitializeHeader();
      m_ownsData = true;
    }
    else
    {
      if (shouldCopyData)
      {
        m_pData = new unsigned char[m_length];
        memcpy(m_pData, data, m_length);
        m_ownsData = true;
      }
      else
      {
        m_pData = data;
        m_ownsData = false;
      }
    }
  }

  GenericByteData::~GenericByteData()
  {
    if (m_pData != nullptr && m_ownsData)
    {
      delete[] m_pData;
      m_pData = nullptr;
      m_length = 0;
    }
  }

  unsigned int GenericByteData::GetLength()
  {
    return m_length;
  }

  unsigned char * GenericByteData::GetData()
  {
    return m_pData;
  }

  unsigned char * GenericByteData::GetBody()
  {
    return m_pData + HEADER_LENGTH;
  }

  unsigned int GenericByteData::GetBodyLength()
  {
    return m_length - HEADER_LENGTH;
  }

  DataType GenericByteData::GetDataType()
  {
    return (DataType)GetTwoBytes(DATATYPE_POS);
  }

  void GenericByteData::SetDataType(DataType datatype)
  {
    m_pData[DATATYPE_POS] = (unsigned char)(((uint16_t)datatype >> 8) & 0xff);
    m_pData[DATATYPE_POS + 1] = (unsigned char)((uint16_t) datatype & 0xff);
  }

  Codec GenericByteData::GetCodec()
  {
    return (Codec)GetTwoBytes(CODECTYPE_POS);
  }

  void GenericByteData::SetCodec(Codec codec)
  {
    m_pData[CODECTYPE_POS] = (unsigned char)(((uint16_t)codec >> 8) & 0xff);
    m_pData[CODECTYPE_POS + 1] = (unsigned char)((uint16_t)codec & 0xff);
  }

  uint16_t GenericByteData::GetSequenceNumber()
  {
    return (uint16_t)GetTwoBytes(SEQNUM_POS);
  }

  void GenericByteData::SetSequenceNumber(uint16_t seqNum)
  {
    m_pData[SEQNUM_POS] = (unsigned char)((seqNum>> 8) & 0xff);
    m_pData[SEQNUM_POS + 1] = (unsigned char)(seqNum & 0xff);
  }

  uint64_t GenericByteData::GetSyncTimeStamp()
  {
    return GetEightBytes(TIMESTAMP_SYNC_POS);
  }

  void GenericByteData::SetSyncTimeStamp(uint64_t ts)
  {
    m_pData[TIMESTAMP_SYNC_POS] = (unsigned char)(((ts / 0x100000000) >> 24) & 0xff);
    m_pData[TIMESTAMP_SYNC_POS + 1] = (unsigned char)(((ts / 0x100000000) >> 16) & 0xff);
    m_pData[TIMESTAMP_SYNC_POS + 2] = (unsigned char)(((ts / 0x100000000) >> 8) & 0xff);
    m_pData[TIMESTAMP_SYNC_POS + 3] = (unsigned char)((ts / 0x100000000) & 0xff);
    m_pData[TIMESTAMP_SYNC_POS + 4] = (unsigned char)((ts >> 24) & 0xff);
    m_pData[TIMESTAMP_SYNC_POS + 5] = (unsigned char)((ts >> 16) & 0xff);
    m_pData[TIMESTAMP_SYNC_POS + 6] = (unsigned char)((ts >> 8) & 0xff);
    m_pData[TIMESTAMP_SYNC_POS + 7] = (unsigned char)(ts & 0xff);
  }

  uint64_t GenericByteData::GetTimeStamp()
  {
    return GetEightBytes(TIMESTAMP_POS);
  }

  void GenericByteData::SetTimeStamp(uint64_t ts)
  {
    m_pData[TIMESTAMP_POS] = (unsigned char)(((ts / 0x100000000) >> 24) & 0xff);
    m_pData[TIMESTAMP_POS + 1] = (unsigned char)(((ts / 0x100000000) >> 16) & 0xff);
    m_pData[TIMESTAMP_POS + 2] = (unsigned char)(((ts / 0x100000000) >> 8) & 0xff);
    m_pData[TIMESTAMP_POS + 3] = (unsigned char)((ts / 0x100000000) & 0xff);
    m_pData[TIMESTAMP_POS + 4] = (unsigned char)((ts >> 24) & 0xff);
    m_pData[TIMESTAMP_POS + 5] = (unsigned char)((ts >> 16) & 0xff);
    m_pData[TIMESTAMP_POS + 6] = (unsigned char)((ts >> 8) & 0xff);
    m_pData[TIMESTAMP_POS + 7] = (unsigned char)(ts & 0xff);
  }

  uint16_t GenericByteData::GetFlags()
  {
    return GetTwoBytes(FLAGS_POS);
  }

  void GenericByteData::SetFlags(uint16_t flags)
  {
    m_pData[FLAGS_POS] = (unsigned char)(( flags >> 8) & 0xff);
    m_pData[FLAGS_POS + 1] = (unsigned char)(flags & 0xff);
  }

  void GenericByteData::InitializeHeader()
  {
    FillHeaderWithZeroes();
    SetLength(m_length);
    SetDataType(DataType::VIDEO);
    SetCodec(Codec::JPEG);
    SetSequenceNumber(0);
    SetSyncTimeStamp(0);
    SetTimeStamp(0);
    SetFlags(1);
  }

  uint16_t GenericByteData::GetTwoBytes(int pos)
  {
    uint8_t firstByte = m_pData[pos];
    uint8_t secondByte = m_pData[pos + 1];
    uint16_t fullTwoBytes = (((uint16_t)firstByte) << 8) + ((uint16_t)secondByte);
    return fullTwoBytes;
  }

  uint32_t GenericByteData::GetFourBytes(int pos)
  {
    uint16_t firstTwoBytes = GetTwoBytes(pos);
    uint16_t secondTwoBytes = GetTwoBytes(pos + 2);
    uint32_t fullFourBytes = (((uint32_t)firstTwoBytes) << 16) + ((uint32_t)secondTwoBytes);
    return fullFourBytes;
  }

  uint64_t GenericByteData::GetEightBytes(int pos)
  {
    uint32_t firstFourBytes = GetFourBytes(pos);
    uint32_t secondFourBytes = GetFourBytes(pos + 4);
    uint64_t fullEightBytes = (((uint64_t)firstFourBytes) << 32) + ((uint64_t)secondFourBytes);
    return fullEightBytes;
  }

  void GenericByteData::SetLength(uint32_t length)
  {
    m_pData[TOTALLENGTH_POS] = (unsigned char)((length >> 24) & 0xff);
    m_pData[TOTALLENGTH_POS + 1] = (unsigned char)((length >> 16) & 0xff);
    m_pData[TOTALLENGTH_POS + 2] = (unsigned char)((length >> 8) & 0xff);
    m_pData[TOTALLENGTH_POS + 3] = (unsigned char)(length & 0xff);
  }

  void GenericByteData::FillHeaderWithZeroes()
  {
    for (int i = 0; i < HEADER_LENGTH; i++)
    {
      m_pData[i] = 0x00;
    }
  }

}

