#ifndef __GST_ONVIF_META_H__
#define __GST_ONVIF_META_H__

#ifdef _WIN32
#define __GST_ONVIF_META_H__DllExport   __declspec( dllexport ) 
#else
#define __GST_ONVIF_META_H__DllExport
#endif

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_ONVIF_META_API_TYPE (gst_onvif_meta_api_get_type())
#define GST_ONVIF_META_INFO (gst_onvif_meta_get_info())
typedef struct _GstOnvifMeta GstOnvifMeta;

/**
 * GstOnvifMeta:
 * 
 * @meta: parent #GstMeta
 * @onvifXml: A char byte pointer pointing to the full ONVIF xml
 * @xmlSize: Size of the xml string in bytes
 * @timestamp: Milliseconds since Unix Epoch time matching the one inside the ONVIF xml
 *
 * This meta can be used to attach ONVIF metadata to a data buffer. This can for example be used 
 * for bounding boxes.
 */
struct _GstOnvifMeta
{
  GstMeta meta;

  gchar * onvifXml;
  size_t xmlSize;
  guint64 timestamp;
};

GType gst_onvif_meta_api_get_type(void);
const GstMetaInfo *gst_onvif_meta_get_info(void);
__GST_ONVIF_META_H__DllExport GstOnvifMeta * gst_buffer_get_onvif_meta(GstBuffer * buffer);
__GST_ONVIF_META_H__DllExport GstOnvifMeta * gst_buffer_add_onvif_meta(GstBuffer *buffer, gchar *xml, size_t xmlSize, guint64 timestamp);
void gst_onvif_meta_free(GstMeta *meta, GstBuffer *buffer);

G_END_DECLS

#endif // __GST_ONVIF_META_H__
