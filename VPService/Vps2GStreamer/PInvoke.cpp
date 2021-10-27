#include "PInvoke.h"

Vps2GStreamer * CreateGStreamer(char *pipelineName)
{
  std::string str(pipelineName);
  Vps2GStreamer* vps2gstreamer = new Vps2GStreamer(str);
  if (vps2gstreamer != nullptr && vps2gstreamer->mData.partner_pipeline)
  {
    return vps2gstreamer;
  }
  else
  {
    if (vps2gstreamer != nullptr)
    {
      delete vps2gstreamer;
    }
    return nullptr;
  }
}

void DeleteGStreamer(Vps2GStreamer * instance)
{
  if (instance != nullptr)
  {
    delete instance;
    instance = nullptr;
  }
}

void SetGStreamerProperties(Vps2GStreamer * instance, char* serviceParameters)
{
  if (instance != nullptr)
  {
    instance->SetProperties(std::string(serviceParameters));
  }
}

void PutData(Vps2GStreamer * instance, unsigned char * dataPtr, int dataSize)
{
  if (instance != nullptr)
  {
    instance->PutData(dataPtr, dataSize);
  }
}

IData * GetData(Vps2GStreamer * instance)
{
  if (instance != nullptr)
  {
    return instance->GetData();
  }
  return nullptr;
}

DATATYPE GetDataType(IData * data)
{

  if (data != nullptr)
  {
    MediaData * pMediaData = dynamic_cast<MediaData *>(data);
    if (pMediaData != nullptr)
    {
      return DATATYPE::MediaDataType;
    }
    MetaData * pMetaData = dynamic_cast<MetaData *>(data);
    if (pMetaData != nullptr)
    {
      return DATATYPE::MetaDataType;
    }
  }
  return DATATYPE::Unknown1;
}

const char * MediaData_GetDataPointer(MediaData * data)
{
  if (data != nullptr)
  {
    return data->GetDataPointer();
  }
  return nullptr;
}

int MediaData_GetDataSize(MediaData * data)
{
  if (data != nullptr)
  {
    return data->GetDataSize();
  }
  return 0;
}

void DeleteMemory(void * ptr)
{
  MediaData * pMediaData = static_cast<MediaData*>(ptr);
  if (pMediaData != nullptr)
  {
    delete pMediaData;
    return;
  }
  MetaData * pMetaData = static_cast<MetaData*>(ptr);
  if (pMetaData != nullptr)
  {
    delete pMetaData;
    return;
  }
}

const char * MetaData_GetDataPointer(MetaData * data)
{
  if (data != nullptr)
  {
    return data->GetDataPointer();
  }
  return nullptr;
}

int MetaData_GetDataSize(MetaData * data)
{
  if (data != nullptr)
  {
    return data->GetDataSize();
  }
  return 0;
}

unsigned long long MetaData_GetTimestamp(MetaData * data)
{
  if (data != nullptr)
  {
    return data->GetTimestamp();
  }
  return 0;

}

