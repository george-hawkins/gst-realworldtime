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
#include "gsttimestampfiltersink.h"

GST_DEBUG_CATEGORY_STATIC (gst_timestampfiltersink_debug_category);
#define GST_CAT_DEFAULT gst_timestampfiltersink_debug_category

/* prototypes */


static void gst_timestampfiltersink_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_timestampfiltersink_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_timestampfiltersink_finalize (GObject * object);

static GstFlowReturn gst_timestampfiltersink_render (GstBaseSink * sink,
    GstBuffer * buffer);

#define DEFAULT_LOCATION "%05d"
#define DEFAULT_INDEX 0

enum
{
  PROP_0,
  PROP_LOCATION
};

/* pad templates */

static GstStaticPadTemplate gst_timestampfiltersink_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstTimestampfiltersink, gst_timestampfiltersink,
    GST_TYPE_BASE_SINK,
    GST_DEBUG_CATEGORY_INIT (gst_timestampfiltersink_debug_category,
        "timestampfiltersink", 0,
        "debug category for timestampfiltersink element"));

static void
gst_timestampfiltersink_class_init (GstTimestampfiltersinkClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseSinkClass *base_sink_class = GST_BASE_SINK_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass),
      &gst_timestampfiltersink_sink_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
      "Timestamp-Filter Sink", "Sink/File",
      "Write buffers filtered by timestamps to files",
      "George Hawkins <https://github.com/george-hawkins>");

  gobject_class->set_property = gst_timestampfiltersink_set_property;
  gobject_class->get_property = gst_timestampfiltersink_get_property;

  g_object_class_install_property (gobject_class, PROP_LOCATION,
      g_param_spec_string ("location", "File Location",
          "Location of the file to write", NULL,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gobject_class->finalize = gst_timestampfiltersink_finalize;
  base_sink_class->render = GST_DEBUG_FUNCPTR (gst_timestampfiltersink_render);
}

static void
gst_timestampfiltersink_init (GstTimestampfiltersink * timestampfiltersink)
{
  timestampfiltersink->filename = g_strdup (DEFAULT_LOCATION);
  timestampfiltersink->index = DEFAULT_INDEX;

  gst_base_sink_set_sync (GST_BASE_SINK (timestampfiltersink), FALSE);
}

void
gst_timestampfiltersink_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstTimestampfiltersink *timestampfiltersink =
      GST_TIMESTAMPFILTERSINK (object);

  GST_DEBUG_OBJECT (timestampfiltersink, "set_property");

  switch (property_id) {
    case PROP_LOCATION:
      g_free (timestampfiltersink->filename);
      /* FIXME: validate location to have just one %s */
      timestampfiltersink->filename = g_strdup (g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_timestampfiltersink_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstTimestampfiltersink *timestampfiltersink =
      GST_TIMESTAMPFILTERSINK (object);

  GST_DEBUG_OBJECT (timestampfiltersink, "get_property");

  switch (property_id) {
    case PROP_LOCATION:
      g_value_set_string (value, timestampfiltersink->filename);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_timestampfiltersink_finalize (GObject * object)
{
  GstTimestampfiltersink *timestampfiltersink =
      GST_TIMESTAMPFILTERSINK (object);

  GST_DEBUG_OBJECT (timestampfiltersink, "finalize");

  /* clean up object here */
  g_free (timestampfiltersink->filename);

  G_OBJECT_CLASS (gst_timestampfiltersink_parent_class)->finalize (object);
}

static GstFlowReturn
gst_timestampfiltersink_render (GstBaseSink * sink, GstBuffer * buffer)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "render");

  GstMapInfo map;
  GError *error = NULL;

  gst_buffer_map (buffer, &map, GST_MAP_READ);

  gchar *filename = g_strdup_printf (timestampfiltersink->filename, timestampfiltersink->index);
  gboolean ret = g_file_set_contents (filename, (char *) map.data, map.size, &error);
  if (!ret)
    goto write_error;

  g_free (filename);

  timestampfiltersink->index++;

  gst_buffer_unmap (buffer, &map);
  return GST_FLOW_OK;

  /* ERRORS */
write_error:
  {
    switch (error->code) {
      case G_FILE_ERROR_NOSPC:{
        GST_ELEMENT_ERROR (timestampfiltersink, RESOURCE, NO_SPACE_LEFT, (NULL), (NULL));
        break;
      }
      default:{
        GST_ELEMENT_ERROR (timestampfiltersink, RESOURCE, WRITE,
            ("Error while writing to file \"%s\".", filename),
            ("%s", g_strerror (errno)));
      }
    }
    g_error_free (error);
    g_free (filename);

    gst_buffer_unmap (buffer, &map);
    return GST_FLOW_ERROR;
  }
}
