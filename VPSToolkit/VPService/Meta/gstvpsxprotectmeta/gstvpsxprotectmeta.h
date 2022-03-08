#ifndef __GST_XPROTECT_META_H__
#define __GST_XPROTECT_META_H__

#ifdef _WIN32
#define __GST_XPROTECT_META_H__DllExport   __declspec( dllexport ) 
#else
#define __GST_XPROTECT_META_H__DllExport
#endif

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_XPROTECT_META_API_TYPE (gst_xprotect_meta_api_get_type())
#define GST_XPROTECT_META_INFO (gst_xprotect_meta_get_info())
typedef struct _GstXprotectMeta GstXprotectMeta;

/**
 * GstXprotectMeta:
 *
 * @meta: parent #GstMeta
 * @sequence_number: A 32 bit sequence number for the frame associated with this.
 * @sync_timestamp: Milliseconds since Unix Epoch time of the sync frame.
 * @timestamp: Milliseconds since Unix Epoch time of the frame associated with this meta.
 *
 * This meta can be used to attach information from the XProtect Generic Byte Data header that
 * should follow a video frame. 
 * You may wish to extend this meta with more information from this header. Please see the
 * GenericByteData class in vpsutilities or the MIP documentation on the Generic Byte Data format:
 * https://doc.developer.milestonesys.com/html/index.html?base=mipgenericbytedata/main.html&tree=tree_3.html
 */
struct _GstXprotectMeta 
{
  GstMeta meta;

  guint32 sequence_number;
  guint64 sync_timestamp;
  guint64 timestamp;

};

GType gst_xprotect_meta_api_get_type(void);
const GstMetaInfo *gst_xprotect_meta_get_info(void);
__GST_XPROTECT_META_H__DllExport GstXprotectMeta * gst_buffer_get_xprotect_meta(GstBuffer * buffer);
__GST_XPROTECT_META_H__DllExport GstXprotectMeta * gst_buffer_add_xprotect_meta(GstBuffer *buffer, guint32 sequence_number, guint64 sync_timestamp, guint64 timestamp);

G_END_DECLS

#endif // __GST_XPROTECT_META_H__
