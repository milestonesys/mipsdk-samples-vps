#include "GenericByteDataInterface.h"
#include "GenericByteData.h"

using namespace VpsUtilities;

extern "C"
{
  gbd_t create_genericbytedata(unsigned char * data, gbd_length_t length, bool shouldGenerateHeader, bool shouldCopyData)
  {
    return new GenericByteData(data, length, shouldGenerateHeader, shouldCopyData);
  }

  void destroy_genericbytedata(gbd_t p)
  {
    if (p != nullptr)
    {
      delete static_cast<GenericByteData*>(p);
    }
  }

  gbd_length_t get_length(gbd_t p)
  {
    if (p != nullptr)
    {
      return static_cast<GenericByteData*>(p)->GetLength();
    }
    return 0;
  }

  unsigned char * get_data(gbd_t p)
  {
    if (p != nullptr)
    {
      return static_cast<GenericByteData*>(p)->GetData();
    }
    return nullptr;
  }

  gbd_length_t get_body_length(gbd_t p)
  {
    if (p != nullptr)
    {
      return static_cast<GenericByteData*>(p)->GetBodyLength();
    }
    return 0;
  }

  unsigned char * get_body_data(gbd_t p)
  {
    if (p != nullptr)
    {
      return static_cast<GenericByteData*>(p)->GetBody();
    }
    return nullptr;
  }

  void set_codec(gbd_t p, gbd_codec_t codec)
  {
    if (p != nullptr)
    {
      switch (codec)
      {
      case 1:
        static_cast<GenericByteData*>(p)->SetCodec(Codec::JPEG);
        break;
      case 10:
        static_cast<GenericByteData*>(p)->SetCodec(Codec::H264);
        break;
      case 14:
        static_cast<GenericByteData*>(p)->SetCodec(Codec::H265);
        break;
      default:
        break;
      }
    }
  }

  gbd_codec_t get_codec(gbd_t p)
  {
    if (p != nullptr)
    {
      switch (static_cast<GenericByteData*>(p)->GetCodec())
      {
      case Codec::H264:
        return 10;
        break;
      case Codec::H265:
        return 14;
        break;
      case Codec::JPEG:
      default:
        return 1;
        break;
      }
    }
    return 0;
  }

  void set_sequence_number(gbd_t p, gbd_sequence_number_t seqNo)
  {
    if (p != nullptr)
    {
      static_cast<GenericByteData*>(p)->SetSequenceNumber(seqNo);
    }
  }

  gbd_sequence_number_t get_sequence_number(gbd_t p)
  {
    if (p != nullptr)
    {
      return static_cast<GenericByteData*>(p)->GetSequenceNumber();
    }
    return 0;
  }

  void set_sync_timestamp(gbd_t p, gbd_timestamp_t ts)
  {
    if (p != nullptr)
    {
      static_cast<GenericByteData*>(p)->SetSyncTimeStamp(ts);
    }
  }

  gbd_timestamp_t get_sync_timestamp(gbd_t p)
  {
    if (p != nullptr)
    {
      return static_cast<GenericByteData*>(p)->GetSyncTimeStamp();
    }
    return 0;
  }

  void set_timestamp(gbd_t p, gbd_timestamp_t ts)
  {
    if (p != nullptr)
    {
      static_cast<GenericByteData*>(p)->SetTimeStamp(ts);
    }
  }

  gbd_timestamp_t get_timestamp(gbd_t p)
  {
    if (p != nullptr)
    {
      return static_cast<GenericByteData*>(p)->GetTimeStamp();
    }
    return 0;
  }

  void set_flags(gbd_t p, gbd_flags_t flags)
  {
    if (p != nullptr)
    {
      static_cast<GenericByteData*>(p)->SetFlags(flags);
    }
  }

  unsigned int get_flags(gbd_t p)
  {
    if (p != nullptr)
    {
      return static_cast<GenericByteData*>(p)->GetFlags();
    }
    return 0;
  }
}

