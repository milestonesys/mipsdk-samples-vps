#include "vps2gstreamer.h"
#include "../Meta/gstvpsonvifmeta/gstvpsonvifmeta.h"
#include <cstddef>
#include <gst/app/gstappsink.h>
#include <ctime>
#include <chrono>

#ifndef _WINDOWS
#include <unistd.h>
#endif

using namespace std::chrono_literals;

const static size_t max_queue_size = 100;
const static int sleep_time_seconds = 5;

static gboolean push_data(VpsData * data);
static GstFlowReturn mediadata_ready(GstElement * sink, Vps2GStreamer * obj);
static void stop_feed(GstElement * source, VpsData * data);
static void start_feed(GstElement * source, guint size, VpsData * data);
static void error_cb(GstBus * bus, GstMessage * msg, Vps2GStreamer * data);

Vps2GStreamer::Vps2GStreamer(std::string partnerPipeline)
  : m_gstPartnerPipeline(partnerPipeline)
{
  m_ready = false;
  m_handlerIdAppSinkMediadataReady = 0;
  m_handlerIdAppSourceStartFeed = 0;
  m_handlerIdAppSourceStopFeed = 0;
  SetupGStreamer();
  m_ready = true;
}

Vps2GStreamer::~Vps2GStreamer()
{
  TeardownGStreamer();
}

void Vps2GStreamer::PutData(unsigned char * pData, int dataSize)
{
  mData.mData = pData;
  mData.mDataSize = dataSize;
  push_data(&mData);
}

IData * Vps2GStreamer::GetData()
{
  if (!m_ready)
  {
    return nullptr;
  }

  IData * data = nullptr;

  {
    std::unique_lock<std::mutex> mutex(m_sinkQueueMutex);

    auto now = std::chrono::system_clock::now();

    while (m_sinkQueue.empty())
    {
      if (m_sinkQueueNotEmpty.wait_until(mutex, now + 500ms) == std::cv_status::timeout)
        return nullptr;
    }

    data = m_sinkQueue.front();
    m_sinkQueue.pop();
  }

  return data;
}

void Vps2GStreamer::SetupGStreamer()
{
  GstCaps *caps;
  GstBus *bus;

  /* Initialize mData structure */
  memset(&mData, 0, sizeof(mData));

  /* Initialize GStreamer */
  gst_init(NULL, NULL);

  /* Create the appsrc element */
  mData.app_source = gst_element_factory_make("appsrc", "source");

  if (!mData.app_source)
  {
    GST_ERROR("AppSrc element could not be created.\n");
    return;
  }

  /* Create the appsink element */
  mData.app_sink = gst_element_factory_make("appsink", "sink");

  if (!mData.app_sink)
  {
    GST_ERROR("AppSink element could not be created.\n");
    return;
  }

  /* Create the external element, which may be a pipeline supplied by a partner */
  mData.partner_pipeline = gst_element_factory_make(m_gstPartnerPipeline.c_str(), "partnerPipeline");

  if (!mData.partner_pipeline)
  {
    GST_ERROR("Element from 3rd party partner pipeline could not be created.\n");
    return;
  }

  /* Create the empty pipeline */
  mData.pipeline = gst_pipeline_new("pipeline");

  if (!mData.pipeline)
  {
    GST_ERROR("Pipeline element could not be created.\n");
    return;
  }

  /* Configure appsrc */
  m_handlerIdAppSourceStartFeed = g_signal_connect(mData.app_source, "need-data", G_CALLBACK(start_feed), &mData);
  m_handlerIdAppSourceStopFeed = g_signal_connect(mData.app_source, "enough-data", G_CALLBACK(stop_feed), &mData);

  /* Configure appsink */
  caps = gst_caps_new_any();
  g_object_set(mData.app_sink, "emit-signals", TRUE, "caps", caps, NULL);
  m_handlerIdAppSinkMediadataReady = g_signal_connect(mData.app_sink, "new-sample", G_CALLBACK(mediadata_ready), this);
  gst_caps_unref(caps);

  /* Link all elements that can be automatically linked because they have "Always" pads */
  gst_bin_add_many(GST_BIN(mData.pipeline), mData.app_source, mData.partner_pipeline, mData.app_sink, NULL);

  if (gst_element_link(mData.app_source, mData.partner_pipeline) != (gboolean)TRUE)
  {
    GST_ERROR("app source and partner plugin could not be linked.\n");
  }

  if (gst_element_link(mData.partner_pipeline, mData.app_sink) != (gboolean)TRUE)
  {
    GST_ERROR("partner plugin and app sink could not be linked.\n");
  }

  /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
  bus = gst_element_get_bus(mData.pipeline);
  gst_bus_add_signal_watch(bus);
  g_signal_connect(G_OBJECT(bus), "message::error", (GCallback)error_cb, this);
  gst_object_unref(bus);

  /* Start playing the pipeline */
  while (gst_element_set_state(mData.pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
  {
    GST_ERROR("Changing state to playing failed! Will retry in 5 seconds\n");
    DoSleep(sleep_time_seconds);
  }
}

void Vps2GStreamer::TeardownGStreamer()
{
  // Ensure pipeline is stopped and then unref it
  if (mData.pipeline != 0)
  {
    gst_element_set_state(mData.pipeline, GST_STATE_NULL);
    g_object_unref(mData.pipeline);
  }
}

std::vector<std::string> Vps2GStreamer::KeyValuePairsFromString(std::string serviceProperties)
{
  // Extract the individual comma-separated key-value pairs like "color=red"
  std::vector<std::string> keyValuePairs;
  std::string keyValuePair;
  size_t offset = 0;
  size_t nextOffset = 0;

  while (true)
  {
    nextOffset = serviceProperties.find_first_of(',', offset);

    if (nextOffset == std::string::npos)
    {
      keyValuePair = serviceProperties.substr(offset, std::string::npos);

      if (!keyValuePair.empty())
      {
        keyValuePairs.push_back(keyValuePair);
      }

      break;
    }

    keyValuePair = serviceProperties.substr(offset, nextOffset - offset);
    keyValuePairs.push_back(keyValuePair);
    offset = nextOffset + 1;
  }

  return keyValuePairs;
}

std::pair<std::string, std::string> PairFromKeyValueString(std::string keyValue)
{
  std::string key = keyValue; // If there is no equal sign, pass the string as a key with an empty string value.
  std::string val;
  size_t equalSignIndex = keyValue.find_first_of('=');

  if (equalSignIndex != std::string::npos)
  {
    key = keyValue.substr(0, equalSignIndex);
    val = keyValue.substr(equalSignIndex + 1);
  }

  return std::make_pair(key, val);
}

void Vps2GStreamer::SetProperties(std::string serviceProperties)
{
  std::vector<std::string> keyValuePairs = KeyValuePairsFromString(serviceProperties);

  // For each key-value pair, extract the key and the value, like "color" and "red"
  // Determine the type of the value and call the GStreamer set property function accordingly
  // You may extend this with other types than bool and string
  for (std::vector<std::string>::iterator it = keyValuePairs.begin(); it != keyValuePairs.end(); ++it)
  {
    std::pair<std::string, std::string> keyValuePair = PairFromKeyValueString(*it);

    if (keyValuePair.first.empty() || keyValuePair.second.empty())
    {
      continue;
    }

    if (IsValidTrueValue(keyValuePair.second))
    {
      g_object_set(G_OBJECT(mData.partner_pipeline), keyValuePair.first.c_str(), TRUE, NULL);
    }
    else if (IsValidFalseValue(keyValuePair.second))
    {
      g_object_set(G_OBJECT(mData.partner_pipeline), keyValuePair.first.c_str(), FALSE, NULL);
    }
    else
    {
      // We pass it as the original string type value
      g_object_set(G_OBJECT(mData.partner_pipeline), keyValuePair.first.c_str(), keyValuePair.second.c_str(), NULL);
    }
  }
}

bool Vps2GStreamer::IsValidTrueValue(std::string val)
{
  return (val.compare("yes") == 0 || val.compare("Yes") == 0 || val.compare("YES") == 0 || val.compare("true") == 0 || val.compare("True") == 0 || val.compare("TRUE") == 0);
}

bool Vps2GStreamer::IsValidFalseValue(std::string val)
{
  return (val.compare("no") == 0 || val.compare("No") == 0 || val.compare("NO") == 0 || val.compare("false") == 0 || val.compare("False") == 0 || val.compare("FALSE") == 0);
}

void Vps2GStreamer::DoSleep(int const seconds)
{
#ifdef _WINDOWS
  std::this_thread::sleep_for(std::chrono::milliseconds(seconds * 1000));
#else
  sleep(seconds);
#endif
}

/* This signal callback triggers when appsrc needs data. Here, we add an idle handler
 * to the mainloop to start pushing data into the appsrc */
static void start_feed(GstElement * source, guint size, VpsData * data)
{
  if (data->sourceid == 0)
  {
    data->sourceid = g_idle_add((GSourceFunc)push_data, data);
  }
}

/* This callback triggers when appsrc has enough data and we can stop sending.
 * We remove the idle handler from the mainloop */
static void stop_feed(GstElement * source, VpsData * data)
{
  if (data->sourceid != 0)
  {
    g_source_remove(data->sourceid);
    data->sourceid = 0;
  }
}

static GstFlowReturn mediadata_ready(GstElement * sink, Vps2GStreamer* self)
{
  GstSample *sample;
  GstBuffer *buffer;

  /* Retrieve the buffer */
  g_signal_emit_by_name(sink, "pull-sample", &sample);
  buffer = gst_sample_get_buffer(sample);
  GstOnvifMeta * meta = gst_buffer_get_onvif_meta(buffer);

  GstMapInfo info;
  gst_buffer_map(buffer, &info, GST_MAP_READ);

  {
    std::unique_lock<std::mutex> mutex(self->m_sinkQueueMutex);

    if (self->m_sinkQueue.size() > max_queue_size)
    {
      GST_WARNING("app sink queue is full; data is being dropped!");
    }
    else
    {
      bool wasEmpty = self->m_sinkQueue.empty();

      if (info.data != NULL && info.size > 0)
        self->m_sinkQueue.push(new MediaData((char*)info.data, (int)info.size));

      if (meta != nullptr)
        self->m_sinkQueue.push(new MetaData((char*)meta->onvifXml, (int)meta->xmlSize, meta->timestamp));

      if (wasEmpty && !self->m_sinkQueue.empty())
        self->m_sinkQueueNotEmpty.notify_one();
    }
  }

  gst_buffer_unmap(buffer, &info);
  gst_sample_unref(sample);

  return GST_FLOW_OK;
}

/* This function is called when an error message is posted on the bus */
static void error_cb(GstBus * bus, GstMessage * msg, Vps2GStreamer * self)
{
  GError *err;
  gchar *debug_info;

  /* Print error details on the screen */
  gst_message_parse_error(msg, &err, &debug_info);
  GST_ERROR("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
  GST_ERROR("Debugging information: %s\n", debug_info ? debug_info : "none");
  g_clear_error(&err);
  g_free(debug_info);
  g_main_loop_quit(self->m_loop);
}

/*
 * This method is called by the idle GSource in the mainloop, to feed bytes into appsrc.
 * The idle handler is added to the mainloop when appsrc requests us to start sending data (need-data signal)
 * and is removed when appsrc has enough data (enough-data signal).
 */

static gboolean push_data(VpsData * data)
{
  if (data->mData != nullptr)
  {
    int dataSize = data->mDataSize;

    GstBuffer *buffer = gst_buffer_new();
    GstMemory *mem = gst_allocator_alloc(NULL, dataSize, NULL);
    gst_buffer_append_memory(buffer, mem);

    GstMapInfo map;
    gst_buffer_map(buffer, &map, GST_MAP_WRITE);
    memcpy(map.data, data->mData, dataSize);

    /* Push the buffer into the appsrc */
    GstFlowReturn ret;
    g_signal_emit_by_name(data->app_source, "push-buffer", buffer, &ret);

    /* Free the buffer now that we are done with it */
    gst_buffer_unmap(buffer, &map);
    gst_buffer_unref(buffer);

    if (ret != GST_FLOW_OK) 
    {
      GST_ERROR("error in push_data\n");
      /* We got some error, stop sending data */
      return FALSE;
    }

    return TRUE;
  }

  GST_ERROR("push_data error - data is null\n");
  return FALSE;
}
