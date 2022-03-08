#include "gstvpsonvifmeta.h"


static gboolean gst_onvif_meta_init(GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  GstOnvifMeta *emeta = (GstOnvifMeta *)meta;

  emeta->onvifXml = NULL;
  emeta->xmlSize = 0;
  emeta->timestamp = 0;

  return TRUE;
}

static gboolean gst_onvif_meta_transform(GstBuffer * dest, GstMeta * meta,
  GstBuffer * buffer, GQuark type, gpointer data)
{
  GstOnvifMeta * smeta, *dmeta;

  smeta = (GstOnvifMeta *)meta;

  dmeta = (GstOnvifMeta *)gst_buffer_add_meta(dest, GST_ONVIF_META_INFO, NULL);

  if (!dmeta)
  {
    return FALSE;
  }
  dmeta->onvifXml = g_strdup(smeta->onvifXml);
  dmeta->xmlSize = smeta->xmlSize;
  dmeta->timestamp = smeta->timestamp;

  return TRUE;
}

GType gst_onvif_meta_api_get_type(void)
{
  static volatile GType type = 0;
  static const gchar *tags[] = { NULL };

  if (g_once_init_enter(&type))
  {
    GType _type = gst_meta_api_type_register("GstOnvifMetaAPI", tags);
    g_once_init_leave(&type, _type);
  }
  return type;
}

const GstMetaInfo * gst_onvif_meta_get_info(void)
{
  static const GstMetaInfo *meta_info = NULL;

  if (g_once_init_enter((GstMetaInfo **)& meta_info))
  {
    const GstMetaInfo *mi = gst_meta_register(GST_ONVIF_META_API_TYPE,
      "GstOnvifMeta",
      sizeof(GstOnvifMeta),
      (GstMetaInitFunction)gst_onvif_meta_init,
      (GstMetaFreeFunction)gst_onvif_meta_free,
      (GstMetaTransformFunction)gst_onvif_meta_transform);
    g_once_init_leave((GstMetaInfo **)& meta_info, (GstMetaInfo *)mi);
  }
  return meta_info;
}

GstOnvifMeta * gst_buffer_get_onvif_meta(GstBuffer * buffer)
{
  gpointer state = NULL;
  GstOnvifMeta *out = NULL;
  GstMeta *meta;
  const GstMetaInfo *info = GST_ONVIF_META_INFO;

  while ((meta = gst_buffer_iterate_meta(buffer, &state)))
  {
    if (meta->info->api == info->api)
    {
      out = (GstOnvifMeta *)meta;
    }
  }
  return out;
}

GstOnvifMeta * gst_buffer_add_onvif_meta(GstBuffer *buffer, gchar *xml, size_t xmlSize, guint64 timestamp)
{
  GstOnvifMeta *meta;

  g_return_val_if_fail(GST_IS_BUFFER(buffer), NULL);

  meta = (GstOnvifMeta *)gst_buffer_add_meta(buffer,
    GST_ONVIF_META_INFO, NULL);

  meta->onvifXml = g_strdup(xml);
  meta->xmlSize = xmlSize;
  meta->timestamp = timestamp;

  return meta;
}

void gst_onvif_meta_free(GstMeta *meta, GstBuffer *buffer)
{
  GstOnvifMeta *onvif_meta = (GstOnvifMeta *)meta;
  if (onvif_meta->onvifXml) {
    g_free(onvif_meta->onvifXml);
    onvif_meta->onvifXml = NULL;
    onvif_meta->xmlSize = 0;
    onvif_meta->timestamp = 0;
  }
}
