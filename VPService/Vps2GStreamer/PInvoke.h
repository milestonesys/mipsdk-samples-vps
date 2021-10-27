#include "vps2gstreamer.h"

// things to expose in the dll
extern "C"
{
  Vps2GStreamer * CreateGStreamer(char *pipelineName);
  void DeleteGStreamer(Vps2GStreamer * instance);
  void SetGStreamerProperties(Vps2GStreamer * instance, char* serviceParameters);
  void PutData(Vps2GStreamer * instance, unsigned char * dataPtr, int dataSize);
  IData * GetData(Vps2GStreamer * instance);
  DATATYPE GetDataType(IData * data);
  void DeleteMemory(void * ptr);

  const char * MediaData_GetDataPointer(MediaData * data);
  int MediaData_GetDataSize(MediaData * data);

  const char * MetaData_GetDataPointer(MetaData * data);
  int MetaData_GetDataSize(MetaData * data);
  unsigned long long MetaData_GetTimestamp(MetaData * data);
}
