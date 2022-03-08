#ifndef _GENERICBYTEDATA_H_
#define _GENERICBYTEDATA_H_

#include <cstdint>

namespace VpsUtilities
{
  const unsigned int HEADER_LENGTH = 32;      // Total length of the Generic Byte Data header
  
  const unsigned int DATATYPE_POS = 0;        // Byte position of the data type
  const unsigned int TOTALLENGTH_POS = 2;     // Byte position of the total length
  const unsigned int CODECTYPE_POS = 6;       // Byte position of the codec type
  const unsigned int SEQNUM_POS = 8;          // Byte position of the sequence number
  const unsigned int FLAGS_POS = 10;          // Byte position of the flags
  const unsigned int TIMESTAMP_SYNC_POS = 12; // Byte position of the sync timestamp
  const unsigned int TIMESTAMP_POS = 20;      // Byte position of the timestamp
  const unsigned int RESERVED_POS = 28;       // Byte position of the reserved bytes
  
  enum class Codec : uint16_t
  {
    JPEG = 0x0001,  // Value representing JPEG video codec
    H264 = 0x000A,  // Value representing H.264 video codec
    H265 = 0x000E   // Value representing H.265 video codec
  };

  enum class DataType : uint16_t
  {
    VIDEO = 0x0010,    // Value representing video data
    AUDIO = 0x0020,    // Value representing audio data
    METADATA = 0x0030  // Value representing metadata
  };

  /* ------------------------------- GenericByteData class ---------------------------------------- */
  /**
  * @brief Implementation of the Generic Byte Data format.
  *
  * This class is a convenience class for extracting Generic Byte Data header information from raw data
  * as well as for constructing Generic Byte Data headers.
  * For more information about the Generic Byte Data format, please refer to the MIP documentation:
  * https://doc.developer.milestonesys.com/html/index.html?base=mipgenericbytedata/main.html&tree=tree_3.html
  */
  class GenericByteData
  {
  public:
    /**
    * Constructor for creating a GenericByteData with the given data.
    * 
    * @param data: Pointer to the raw data
    * @length: Length in bytes of the data
    * @shouldGenerateHeader: Indicates wether a Generic Byte Data header should be generated. If true,
    *                        the header will be initialized with zeroes. If not true, it is assumed that the 
    *                        data in @param data contains a Generic Byte Data header in the first 32 bytes.
    * @shouldCopyData: Indicates wether this object should make a copy of the data or not. If this is not set 
    *                  to true, remember to free the memory of @param data after deleting this object.
    */
    GenericByteData(unsigned char * data, unsigned int length, bool shouldGenerateHeader = false, bool shouldCopyData = false); 

    /**
    * Destructor for GenericByteData.
    *
    * If the GenericByteData was created with shouldCopyData = true, this will deallocate the memory of the copied data.
    */
    ~GenericByteData();

    /**
    * Returns the full length of the Generic Byte Data frame (including the header).
    */
    unsigned int GetLength();
    /**
    * Returns the pointer to the Generic Byte Data frame (starting from the header).
    */
    unsigned char * GetData();
    /** 
    * Returns the length of the body of the Generic Byte Data frame (not including the header).
    */
    unsigned int GetBodyLength();
    /**
    * Returns the pointer to the body of the Generic Byte Data frame (starting after the header).
    */
    unsigned char * GetBody();
    /**
    * Returns the data type of the Generic Byte Data frame.
    */
    DataType GetDataType();
    /**
    * Sets the data type of the Generic Byte Data frame.
    */
    void SetDataType(DataType datatype);
    /**
    * Returns the codec of the Generic Byte Data frame.
    */
    Codec GetCodec();
    /**
    * Sets the codec of the Generic Byte Data frame.
    */
    void SetCodec(Codec codec);
    /**
    * Returns the sequence number of the Generic Byte Data frame.
    */
    uint16_t GetSequenceNumber();
    /**
    * Sets the sequence number of the Generic Byte Data frame.
    */
    void SetSequenceNumber(uint16_t seqNum);
    /**
    * Returns the sync timestamp of the Generic Byte Data frame.
    */
    uint64_t GetSyncTimeStamp();
    /**
    * Sets the sync timestamp of the Generic Byte Data frame.
    */
    void SetSyncTimeStamp(uint64_t ts);
    /**
    * Returns the timestamp of the Generic Byte Data frame.
    */
    uint64_t GetTimeStamp();
    /**
    * Sets the timestamp of the Generic Byte Data frame.
    */
    void SetTimeStamp(uint64_t ts);
    /**
    * Returns the flags of the Generic Byte Data frame.
    */
    uint16_t GetFlags();
    /**
    * Sets the flags of the Generic Byte Data frame.
    */
    void SetFlags(uint16_t flags);

  private:
    unsigned char * m_pData;
    unsigned int m_length;
    bool m_ownsData;

    void InitializeHeader();
    uint16_t GetTwoBytes(int pos);
    uint32_t GetFourBytes(int pos);
    uint64_t GetEightBytes(int pos);
    void SetLength(uint32_t length);
    void FillHeaderWithZeroes();

  };
}

#endif // _GENERICBYTEDATA_H_
