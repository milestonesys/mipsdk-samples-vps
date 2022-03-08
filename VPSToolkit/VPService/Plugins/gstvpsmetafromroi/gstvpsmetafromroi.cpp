/*
 * Milestone
 * Element which transforms GstVideoRegionOfInterestMeta into ONVIF bounding boxes
 * at the same time stripping the video, so it is not returned back to the XProtect VMS
 *
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2020  <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-vpsmetafromroi
 *
 * Element which transforms GstVideoRegionOfInterestMeta into ONVIF bounding boxes and strips original video.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! vpsmetafromroi ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/gststructure.h>
#include <gst/video/gstvideometa.h>
#include <string>
#include <sstream>
#include <chrono>
#include "../../Meta/gstvpsonvifmeta/gstvpsonvifmeta.h"
#include "../../VpsUtilities/OSHelper.h"

#include "gstvpsmetafromroi.h"

GST_DEBUG_CATEGORY_STATIC (gst_vpsmetafromroi_debug);
#define GST_CAT_DEFAULT gst_vpsmetafromroi_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_vpsmetafromroi_parent_class parent_class
G_DEFINE_TYPE (Gstvpsmetafromroi, gst_vpsmetafromroi, GST_TYPE_ELEMENT);

static void gst_vpsmetafromroi_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_vpsmetafromroi_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_vpsmetafromroi_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_vpsmetafromroi_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the vpsmetafromroi's class */
static void gst_vpsmetafromroi_class_init (GstvpsmetafromroiClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_vpsmetafromroi_set_property;
  gobject_class->get_property = gst_vpsmetafromroi_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple(gstelement_class,
    "vpsmetafromroi",
    "VPS/test",
    "Transforms GstVideoRegionOfInterestMeta into ONVIF bounding boxes and strips the video",
    "developer.milestonesys.com");
    
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_vpsmetafromroi_init (Gstvpsmetafromroi * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_vpsmetafromroi_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_vpsmetafromroi_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
}

static void gst_vpsmetafromroi_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
  Gstvpsmetafromroi *filter = GST_VPSMETAFROMROI (object);

  switch (prop_id) 
  {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void gst_vpsmetafromroi_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
  Gstvpsmetafromroi *filter = GST_VPSMETAFROMROI (object);

  switch (prop_id) 
  {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_vpsmetafromroi_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  Gstvpsmetafromroi *filter;
  gboolean ret;

  filter = GST_VPSMETAFROMROI (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) 
  {
    case GST_EVENT_CAPS:
    {
      GstCaps * caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

static std::string make_onvif_xml(std::string timestamp, std::string bbox)
{
  std::stringstream sstream;
  sstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  sstream << "<tt:MetadataStream xmlns:tt=\"http://www.onvif.org/ver10/schema\">";
  sstream << "<tt:VideoAnalytics>";
  sstream << "<tt:Frame UtcTime=\"" << timestamp << "\">";

  sstream << bbox;

  sstream << "</tt:Frame>";
  sstream << "</tt:VideoAnalytics>";
  sstream << "</tt:MetadataStream>";

  return sstream.str();
}

static std::string make_onvif_face_age_bounding_box(int id, double confidence, double bottom, double top, double left, double right, std::string color, std::string age, std::string gender)
{
  std::stringstream sl;
  sl << gender;
  sl << " - ";
  sl << age;
  sl << " - ";
  sl << confidence;
  std::string label = sl.str();
    
  std::stringstream ss;
  ss << "<tt:Object ObjectId=\"" << id << "\">";
  ss <<   "<tt:Appearance>";
  ss <<     "<tt:Class>";
  ss <<       "<tt:ClassCandidate>";
  ss <<         "<tt:Type>Human</tt:Type>";
  ss <<         "<tt:Likelihood>" << confidence << "</tt:Likelihood>";
  ss <<       "</tt:ClassCandidate>";
  ss <<     "</tt:Class>";
  ss <<     "<tt:Class>";
  ss <<       "<tt:ClassCandidate>";
  ss <<         "<tt:Type>Face</tt:Type>";
  ss <<         "<tt:Likelihood>" << confidence << "</tt:Likelihood>";
  ss <<       "</tt:ClassCandidate>";
  ss <<     "</tt:Class>";
  ss <<     "<tt:Shape>";
  ss <<       "<tt:BoundingBox bottom=\"" << bottom << "\" right=\"" << right << "\" top=\"" << top << "\" left=\"" << left << "\"/>";
  ss <<       "<tt:CenterOfGravity y=\"" << bottom << "\" x=\"" << left << "\"/>";
  ss <<       "<tt:Extension>";
  ss <<         "<BoundingBoxAppearance>";
  ss <<           "<Fill color = \"#30" << color << "\" />";
  ss <<           "<Line color = \"#FF" << color << "\" displayedThicknessInPixels = \"2\" />";
  ss <<         "</BoundingBoxAppearance>";
  ss <<       "</tt:Extension>";
  ss <<     "</tt:Shape>";
  ss <<     "<tt:Extension>";
  ss <<       "<Description x=\"" << left + (right - left) / 2 << "\" y=\"" << top + 0.075 << "\" size=\"0.05\" bold=\"true\" ";
  ss <<         "italic=\"false\" fontFamily=\"Helvetica\" color=\"#FF" << color << "\"" << ">" << label;
  ss <<       "</Description>";
  ss <<     "</tt:Extension>";
  ss <<   "</tt:Appearance>";
  ss <<   "<tt:Extension>";
  ss <<     "<Properties>";
  ss <<       "<Property name=\"Gender\">" << gender << "</Property>";
  ss <<       "<Property name=\"Age\">" << age << "</Property>";
  ss <<     "</Properties>";
  ss <<   "</tt:Extension>";
  ss << "</tt:Object>";

  return ss.str();
}

static std::string make_onvif_bounding_boxes(GstBuffer *buffer) 
{
  std::stringstream ss;
  gpointer state = NULL;
  GstMeta *meta = NULL;
  const gchar* gender = "?";
  const gchar* age = "?";
  double confidence = 0.0;
  double bottom = 0.0;
  double top = 0.0;
  double left = 0.0;
  double right = 0.0;
  gint count = 0;

  while ((meta = gst_buffer_iterate_meta(buffer, &state)) != NULL) 
  {
    if (meta->info->api != GST_VIDEO_REGION_OF_INTEREST_META_API_TYPE)
    {
      continue;
    }
        
    GstVideoRegionOfInterestMeta *roi_meta = (GstVideoRegionOfInterestMeta*)meta;
    
    for (GList *gl = roi_meta->params; gl; gl = g_list_next(gl)) 
    {
      GstStructure *structure = (GstStructure *) gl->data;            
      const gchar* layer_name = gst_structure_get_string(structure, "layer_name");
      if (layer_name != NULL)
      {
        if (gst_structure_has_field(structure, "x_min"))
        {
          gst_structure_get_double(structure, "x_min", &left);
        }
        if (gst_structure_has_field(structure, "x_max"))
        {
          gst_structure_get_double(structure, "x_max", &right);
        }
        if (gst_structure_has_field(structure, "y_min"))
        {
          gst_structure_get_double(structure, "y_min", &top);
        }
        if (gst_structure_has_field(structure, "y_max"))
        {
          gst_structure_get_double(structure, "y_max", &bottom);
        }
        if (gst_structure_has_field(structure, "confidence"))
        {
          gst_structure_get_double(structure, "confidence", &confidence);
        }
      }

      const gchar* attribute_name = gst_structure_get_string(structure, "attribute_name");
      if (attribute_name == NULL)
      {
        continue;
      }
      if (g_strstr_len(attribute_name, -1, "gender") != NULL)
      {
        gender = gst_structure_get_string(structure, "label");
      }
      else if (g_strstr_len(attribute_name, -1, "age") != NULL)
      {
        age = gst_structure_get_string(structure, "label");
      }
    }
	
    // ONVIF has image center at 0,0, ranges from -1.0 to +1.0 in either direction, -1,-1 in the bottom left corner.
    // gvadetect output ranges from 0.0 to +1.0 in either direction, 0,0 in the top left corner.
	  bottom = (bottom-0.5)*-2.0;
    top = (top-0.5)*-2.0;
	  right = (right-0.5)*2.0;
    left = (left-0.5)*2.0;    
    count++;
    gint id = count; // Have not identified anywhere the gvadetect will return an object id.
    
    // Boys are blue, girls are pink.
    std::string color = "FFFFFF";
    if (g_strstr_len(gender, -1, "Male") != NULL)
    {
      color = "1010FF";
    }
    if (g_strstr_len(gender, -1, "Female") != NULL)
    {
      color = "FFA0A0";
    }
  
    ss << make_onvif_face_age_bounding_box(count, confidence, bottom, top, left, right, color, age, gender);
  }
  
  return ss.str();
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_vpsmetafromroi_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  Gstvpsmetafromroi *filter;

  filter = GST_VPSMETAFROMROI (parent);

  uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
  std::string currentTimeStr = OSHelper::get_time_as_string(currentTime);

  std::string boxesStr = make_onvif_bounding_boxes(buf);
  std::string xmlStr = make_onvif_xml(currentTimeStr, boxesStr);
  size_t strLength = xmlStr.length();

  GstMapInfo infoInput;
  GstMapInfo infoOutput;
  gst_buffer_map(buf, &infoInput, GST_MAP_READ);

  GstBuffer * outputBuffer = gst_buffer_new();
  size_t frameLength = 0; // No video to be returned to XProtect
  GstMemory * mem = gst_allocator_alloc(NULL, frameLength, NULL);

  gst_buffer_append_memory(outputBuffer, mem);
  gst_buffer_map(outputBuffer, &infoOutput, GST_MAP_WRITE);
  gst_buffer_add_onvif_meta(outputBuffer, (gchar*)xmlStr.c_str(), strLength, currentTime);

  gst_buffer_unmap(outputBuffer, &infoOutput);
  gst_buffer_unmap(buf, &infoInput);
  gst_buffer_unref(buf);

  return gst_pad_push (filter->srcpad, outputBuffer);
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean vpsmetafromroi_init (GstPlugin * vpsmetafromroi)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template vpsmetafromroi' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_vpsmetafromroi_debug, "vpsmetafromroi",
      0, "Template vpsmetafromroi");

  return gst_element_register (vpsmetafromroi, "vpsmetafromroi", GST_RANK_NONE,
      GST_TYPE_VPSMETAFROMROI);
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

/* gstreamer looks for this structure to register vpsmetafromrois
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    vpsmetafromroi,
    "Transforms GstVideoRegionOfInterestMeta into ONVIF bounding boxes and strips the video",
    vpsmetafromroi_init,
    PACKAGE_VERSION,
    GST_LICENSE,
    GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN
)
