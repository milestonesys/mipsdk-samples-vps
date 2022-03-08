#include "gstvpsxprotectmeta.h"


static gboolean gst_xprotect_meta_init(GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  GstXprotectMeta *emeta = (GstXprotectMeta *)meta;

  emeta->sequence_number = 0;
  emeta->sync_timestamp = 0;
  emeta->timestamp = 0;

  return TRUE;
}

static gboolean gst_xprotect_meta_transform(GstBuffer * dest, GstMeta * meta,
  GstBuffer * buffer, GQuark type, gpointer data)
{
  GstXprotectMeta * smeta, *dmeta;

  smeta = (GstXprotectMeta *)meta;

  dmeta = (GstXprotectMeta *)gst_buffer_add_meta(dest, GST_XPROTECT_META_INFO, NULL);

  if (!dmeta)
  {
    return FALSE;
  }
  dmeta->sequence_number = smeta->sequence_number;
  dmeta->sync_timestamp = smeta->sync_timestamp;
  dmeta->timestamp = smeta->timestamp;

  return TRUE;
}

GType gst_xprotect_meta_api_get_type(void)
{
  static volatile GType type = 0;
  static const gchar *tags[] = { NULL };

  if (g_once_init_enter(&type)) 
  {
    GType _type = gst_meta_api_type_register("GstXprotectMetaAPI", tags);
    g_once_init_leave(&type, _type);
  }
  return type;
}

const GstMetaInfo * gst_xprotect_meta_get_info(void)
{
  static const GstMetaInfo *meta_info = NULL;

  if (g_once_init_enter((GstMetaInfo **) & meta_info))
  {
    const GstMetaInfo *mi = gst_meta_register(GST_XPROTECT_META_API_TYPE,
      "GstXprotectMeta",
      sizeof(GstXprotectMeta),
      (GstMetaInitFunction) gst_xprotect_meta_init,
      (GstMetaFreeFunction) NULL,
      (GstMetaTransformFunction) gst_xprotect_meta_transform);
    g_once_init_leave((GstMetaInfo **) & meta_info, (GstMetaInfo *) mi);
  }
  return meta_info;
}

GstXprotectMeta * gst_buffer_get_xprotect_meta(GstBuffer * buffer)
{
  gpointer state = NULL;
  GstXprotectMeta *out = NULL;
  GstMeta *meta;
  const GstMetaInfo *info = GST_XPROTECT_META_INFO;

  while ((meta = gst_buffer_iterate_meta(buffer, &state))) 
  {
    if (meta->info->api == info->api) 
    {
      out = (GstXprotectMeta *)meta;
    }
  }
  return out;
}

GstXprotectMeta * gst_buffer_add_xprotect_meta(GstBuffer *buffer, guint32 sequence_number, guint64 sync_timestamp, guint64 timestamp)
{
  GstXprotectMeta *meta;

  g_return_val_if_fail(GST_IS_BUFFER(buffer), NULL);

  meta = (GstXprotectMeta *)gst_buffer_add_meta(buffer,
    GST_XPROTECT_META_INFO, NULL);

  meta->sequence_number = sequence_number;
  meta->sync_timestamp = sync_timestamp;
  meta->timestamp = timestamp;

  return meta;
}
