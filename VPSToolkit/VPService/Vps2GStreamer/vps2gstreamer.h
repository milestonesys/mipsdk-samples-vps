#ifndef _VPS2GSTREAMER_H_
#define _VPS2GSTREAMER_H_
/// <summary>
/// This class serves as the interface between the VPS web server and a named GStreamer pipeline
/// Callers call PutData() to provide video frames in generic byte data format as input data to an AppSrc which feeds the pipeline
/// Internally, the pipeline will make data available via an AppSink, which feeds data into a member queue 
/// Callers call GetData() to obtain data from that queue.
/// The caller is the C# web server via the P/Invoke C# to C++ interface in C# NativeMethods and PInvoke.cpp/h
/// </summary>

#include <gst/gst.h>
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include "MediaData.h"
#include "MetaData.h"

/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _VpsData
{
  GstElement *pipeline, *app_source, *app_sink;
  GstElement *partner_pipeline;
  unsigned char * mData;
  unsigned int mDataSize;
  guint sourceid;
} VpsData;

enum DATATYPE
{
  Unknown1,
  MediaDataType,
  MetaDataType
};

class Vps2GStreamer
{
public:
  Vps2GStreamer(std::string partnerPipeline);
  ~Vps2GStreamer();
  void SetProperties(std::string serviceParameters);
  void PutData(unsigned char* pData, int dataSize);
  IData* GetData();

public: // Because they must be accessible from a callback parameter of type Vps2GStreamer*
  VpsData mData;

  std::mutex m_sinkQueueMutex;
  std::condition_variable m_sinkQueueNotEmpty;
  std::queue<IData*> m_sinkQueue;

private:
  void SetupGStreamer();
  void TeardownGStreamer();
  std::vector<std::string> KeyValuePairsFromString(std::string serviceProperties);
  std::string m_gstPartnerPipeline;
  bool m_ready;
  gulong m_handlerIdAppSinkMediadataReady;
  gulong m_handlerIdAppSourceStartFeed;
  gulong m_handlerIdAppSourceStopFeed;
  bool IsValidTrueValue(std::string val);
  bool IsValidFalseValue(std::string val);
  void DoSleep(int const seconds);
public:
  void  main_loop_thread(void)
  {
    g_main_loop_run(m_loop);
    g_thread_exit(0);
  };

public:
  GMainLoop *m_loop;
  GThread *m_loop_thread;
};

#endif // _VPS2GSTREAMER_H_
