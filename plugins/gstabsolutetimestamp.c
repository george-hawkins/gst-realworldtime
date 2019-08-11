/*
 * Copyright (C) 2019 George Hawkins <https://github.com/george-hawkins>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "config.h"
#include "gstabsolutetimestamp.h"

GST_DEBUG_CATEGORY_STATIC (gst_absolutetimestamp_debug_category);
#define GST_CAT_DEFAULT gst_absolutetimestamp_debug_category

#define DEFAULT_FILENAME "timestamps.log"

/* prototypes */


static void gst_absolutetimestamp_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_absolutetimestamp_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_absolutetimestamp_dispose (GObject * object);

static gboolean gst_absolutetimestamp_accept_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps);
static gboolean gst_absolutetimestamp_start (GstBaseTransform * trans);
static gboolean gst_absolutetimestamp_stop (GstBaseTransform * trans);
static GstFlowReturn gst_absolutetimestamp_transform_ip (GstBaseTransform *
    trans, GstBuffer * buf);

enum
{
  PROP_0,
  PROP_LOCATION
};

/* pad templates */

static GstStaticPadTemplate gst_absolutetimestamp_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY
    );

static GstStaticPadTemplate gst_absolutetimestamp_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY
    );


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstAbsolutetimestamp, gst_absolutetimestamp,
    GST_TYPE_BASE_TRANSFORM,
    GST_DEBUG_CATEGORY_INIT (gst_absolutetimestamp_debug_category,
        "absolutetimestamp", 0,
        "debug category for absolutetimestamp element"));

static void
gst_absolutetimestamp_class_init (GstAbsolutetimestampClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *base_transform_class =
      GST_BASE_TRANSFORM_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass),
      &gst_absolutetimestamp_src_template);
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass),
      &gst_absolutetimestamp_sink_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
      "Absolutetimestamps", "Generic", "Generate a mapping from relative timestamps to absolute timestamps",
      "George Hawkins <https://github.com/george-hawkins>");

  gobject_class->set_property = gst_absolutetimestamp_set_property;
  gobject_class->get_property = gst_absolutetimestamp_get_property;

  g_object_class_install_property (gobject_class, PROP_LOCATION,
      g_param_spec_string ("location", "File Location",
          "Location of the timestamp mapping file to write", DEFAULT_FILENAME,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gobject_class->dispose = gst_absolutetimestamp_dispose;
  base_transform_class->accept_caps =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_accept_caps);
  base_transform_class->start = GST_DEBUG_FUNCPTR (gst_absolutetimestamp_start);
  base_transform_class->stop = GST_DEBUG_FUNCPTR (gst_absolutetimestamp_stop);
  base_transform_class->transform_ip =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_transform_ip);

}

static void
gst_absolutetimestamp_init (GstAbsolutetimestamp * absolutetimestamp)
{
  absolutetimestamp->filename = g_strdup(DEFAULT_FILENAME);
  absolutetimestamp->file = NULL;
}

void
gst_absolutetimestamp_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (object);

  GST_DEBUG_OBJECT (absolutetimestamp, "set_property");

  const gchar *location;

  switch (property_id) {
    case PROP_LOCATION:
	  location = g_value_get_string (value);
      g_free (absolutetimestamp->filename); // Free the value created in gst_absolutetimestamps_init.
      absolutetimestamp->filename = g_strdup (location);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_absolutetimestamp_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (object);

  GST_DEBUG_OBJECT (absolutetimestamp, "get_property");

  switch (property_id) {
    case PROP_LOCATION:
      g_value_set_string (value, absolutetimestamp->filename);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_absolutetimestamp_dispose (GObject * object)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (object);

  GST_DEBUG_OBJECT (absolutetimestamp, "dispose");

  /* clean up as possible.  may be called multiple times */

  g_free (absolutetimestamp->filename);
  absolutetimestamp->filename = NULL;

  G_OBJECT_CLASS (gst_absolutetimestamp_parent_class)->dispose (object);
}

static gboolean
gst_absolutetimestamp_accept_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "accept_caps");

  GstPad *pad;

  if (direction == GST_PAD_SRC)
    pad = GST_BASE_TRANSFORM_SINK_PAD (trans);
  else
    pad = GST_BASE_TRANSFORM_SRC_PAD (trans);

  return gst_pad_peer_query_accept_caps (pad, caps);
}

/* states */
static gboolean
gst_absolutetimestamp_start (GstBaseTransform * trans)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "start");

  absolutetimestamp->file = g_fopen (absolutetimestamp->filename, "wb");

  if (absolutetimestamp->file == NULL) {
    GST_ELEMENT_ERROR (absolutetimestamp, RESOURCE, OPEN_WRITE,
        ("Could not open file \"%s\" for writing.", absolutetimestamp->filename),
        GST_ERROR_SYSTEM);
    return FALSE;
  }

  return TRUE;
}

// In GStreamer source you often see _("...") - the underscore is defined in <gst/gst-i18n-lib.h> and
// is a shortcut for dgettext (a function for retrieving a localized version of a given string).
// In copying over snippets from gstfilesink.c I've removed the underscore usage.

static gboolean
gst_absolutetimestamp_stop (GstBaseTransform * trans)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "stop");

  if (absolutetimestamp->file) {
    if (fclose (absolutetimestamp->file) != 0)
      GST_ELEMENT_ERROR (absolutetimestamp, RESOURCE, CLOSE,
          ("Error closing file \"%s\".", absolutetimestamp->filename), GST_ERROR_SYSTEM);

    absolutetimestamp->file = NULL;
  }

  return TRUE;
}

/* transform */
static GstFlowReturn
gst_absolutetimestamp_transform_ip (GstBaseTransform * trans, GstBuffer * buf)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "transform_ip");

  GstClockTime timestamp = GST_BUFFER_TIMESTAMP (buf);

  if (timestamp != GST_CLOCK_TIME_NONE) {
      GTimeVal real_time;

      g_get_current_time (&real_time);

      gchar *s = g_time_val_to_iso8601 (&real_time);

      g_fprintf (absolutetimestamp->file, "%" GST_TIME_FORMAT " %s\n", GST_TIME_ARGS (timestamp), s);

      g_free (s);
  }

  return GST_FLOW_OK;
}
