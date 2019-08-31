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
static GstFlowReturn gst_timestampfiltersink_render_list (GstBaseSink * sink,
    GstBufferList * buffer_list);

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
      "FIXME Long name", "Generic", "FIXME Description",
      "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_timestampfiltersink_set_property;
  gobject_class->get_property = gst_timestampfiltersink_get_property;
  gobject_class->finalize = gst_timestampfiltersink_finalize;
  base_sink_class->render = GST_DEBUG_FUNCPTR (gst_timestampfiltersink_render);
  base_sink_class->render_list =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_render_list);

}

static void
gst_timestampfiltersink_init (GstTimestampfiltersink * timestampfiltersink)
{
}

void
gst_timestampfiltersink_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstTimestampfiltersink *timestampfiltersink =
      GST_TIMESTAMPFILTERSINK (object);

  GST_DEBUG_OBJECT (timestampfiltersink, "set_property");

  switch (property_id) {
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

  G_OBJECT_CLASS (gst_timestampfiltersink_parent_class)->finalize (object);
}

static GstFlowReturn
gst_timestampfiltersink_render (GstBaseSink * sink, GstBuffer * buffer)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "render");

  return GST_FLOW_OK;
}

/* Render a BufferList */
static GstFlowReturn
gst_timestampfiltersink_render_list (GstBaseSink * sink,
    GstBufferList * buffer_list)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "render_list");

  return GST_FLOW_OK;
}
