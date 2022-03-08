#include <gst/gst.h>

#include "gstvpstoxprotectconverter.h"
#include "gstvpsfromxprotectconverter.h"
#include "gstvpsxprotectjoin.h"
#include "gstvpsxprotect.h"



static gboolean plugin_init(GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT(gst_xprotect_debug, "xprotect",
    0, "Template fromxprotectconverter");

  if (!gst_element_register(plugin, "toxprotectconverter", GST_RANK_NONE, gst_toxprotectconverter_get_type()))
    return FALSE;
  else if (!gst_element_register(plugin, "fromxprotectconverter", GST_RANK_NONE, gst_fromxprotectconverter_get_type()))
    return FALSE;
  else if (!gst_element_register(plugin, "xprotectjoin", GST_RANK_NONE, gst_xprotectjoin_get_type()))
    return FALSE;

  return TRUE;
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "gst-vps-test"
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "1.0"
#endif

#ifndef GST_PACKAGE_NAME
#define GST_PACKAGE_NAME "VPS Gstreamer test plugin package"
#endif

#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "Milestone Systems"
#endif

#ifndef GST_LICENSE
#define GST_LICENSE "LGPL"
#endif


 /* gstreamer looks for this structure to register xprotect
  *
  * exchange the string 'Template xprotect' with your xprotect description
  */
GST_PLUGIN_DEFINE(
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  vpsxprotect,
  "Plugins to convert from and to XProtect generic byte data format",
  plugin_init,
  PACKAGE_VERSION,
  GST_LICENSE,
  GST_PACKAGE_NAME,
  GST_PACKAGE_ORIGIN
)