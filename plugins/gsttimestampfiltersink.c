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
/**
 * SECTION:element-gsttimestampfiltersink
 *
 * The timestampfiltersink element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! timestampfiltersink ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>
#include "gsttimestampfiltersink.h"

GST_DEBUG_CATEGORY_STATIC (gst_timestampfiltersink_debug_category);
#define GST_CAT_DEFAULT gst_timestampfiltersink_debug_category

/* prototypes */


static void gst_timestampfiltersink_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_timestampfiltersink_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_timestampfiltersink_dispose (GObject * object);
static void gst_timestampfiltersink_finalize (GObject * object);

static GstCaps *gst_timestampfiltersink_get_caps (GstBaseSink * sink,
    GstCaps * filter);
static gboolean gst_timestampfiltersink_set_caps (GstBaseSink * sink,
    GstCaps * caps);
static GstCaps *gst_timestampfiltersink_fixate (GstBaseSink * sink,
    GstCaps * caps);
static gboolean gst_timestampfiltersink_activate_pull (GstBaseSink * sink,
    gboolean active);
static void gst_timestampfiltersink_get_times (GstBaseSink * sink,
    GstBuffer * buffer, GstClockTime * start, GstClockTime * end);
static gboolean gst_timestampfiltersink_propose_allocation (GstBaseSink * sink,
    GstQuery * query);
static gboolean gst_timestampfiltersink_start (GstBaseSink * sink);
static gboolean gst_timestampfiltersink_stop (GstBaseSink * sink);
static gboolean gst_timestampfiltersink_unlock (GstBaseSink * sink);
static gboolean gst_timestampfiltersink_unlock_stop (GstBaseSink * sink);
static gboolean gst_timestampfiltersink_query (GstBaseSink * sink,
    GstQuery * query);
static gboolean gst_timestampfiltersink_event (GstBaseSink * sink,
    GstEvent * event);
static GstFlowReturn gst_timestampfiltersink_wait_event (GstBaseSink * sink,
    GstEvent * event);
static GstFlowReturn gst_timestampfiltersink_prepare (GstBaseSink * sink,
    GstBuffer * buffer);
static GstFlowReturn gst_timestampfiltersink_prepare_list (GstBaseSink * sink,
    GstBufferList * buffer_list);
static GstFlowReturn gst_timestampfiltersink_preroll (GstBaseSink * sink,
    GstBuffer * buffer);
static GstFlowReturn gst_timestampfiltersink_render (GstBaseSink * sink,
    GstBuffer * buffer);
static GstFlowReturn gst_timestampfiltersink_render_list (GstBaseSink * sink,
    GstBufferList * buffer_list);

enum
{
  PROP_0
};

/* pad templates */

static GstStaticPadTemplate gst_timestampfiltersink_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/unknown")
    );


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
  gobject_class->dispose = gst_timestampfiltersink_dispose;
  gobject_class->finalize = gst_timestampfiltersink_finalize;
  base_sink_class->get_caps =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_get_caps);
  base_sink_class->set_caps =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_set_caps);
  base_sink_class->fixate = GST_DEBUG_FUNCPTR (gst_timestampfiltersink_fixate);
  base_sink_class->activate_pull =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_activate_pull);
  base_sink_class->get_times =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_get_times);
  base_sink_class->propose_allocation =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_propose_allocation);
  base_sink_class->start = GST_DEBUG_FUNCPTR (gst_timestampfiltersink_start);
  base_sink_class->stop = GST_DEBUG_FUNCPTR (gst_timestampfiltersink_stop);
  base_sink_class->unlock = GST_DEBUG_FUNCPTR (gst_timestampfiltersink_unlock);
  base_sink_class->unlock_stop =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_unlock_stop);
  base_sink_class->query = GST_DEBUG_FUNCPTR (gst_timestampfiltersink_query);
  base_sink_class->event = GST_DEBUG_FUNCPTR (gst_timestampfiltersink_event);
  base_sink_class->wait_event =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_wait_event);
  base_sink_class->prepare =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_prepare);
  base_sink_class->prepare_list =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_prepare_list);
  base_sink_class->preroll =
      GST_DEBUG_FUNCPTR (gst_timestampfiltersink_preroll);
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
gst_timestampfiltersink_dispose (GObject * object)
{
  GstTimestampfiltersink *timestampfiltersink =
      GST_TIMESTAMPFILTERSINK (object);

  GST_DEBUG_OBJECT (timestampfiltersink, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_timestampfiltersink_parent_class)->dispose (object);
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

static GstCaps *
gst_timestampfiltersink_get_caps (GstBaseSink * sink, GstCaps * filter)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "get_caps");

  return NULL;
}

/* notify subclass of new caps */
static gboolean
gst_timestampfiltersink_set_caps (GstBaseSink * sink, GstCaps * caps)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "set_caps");

  return TRUE;
}

/* fixate sink caps during pull-mode negotiation */
static GstCaps *
gst_timestampfiltersink_fixate (GstBaseSink * sink, GstCaps * caps)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "fixate");

  return NULL;
}

/* start or stop a pulling thread */
static gboolean
gst_timestampfiltersink_activate_pull (GstBaseSink * sink, gboolean active)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "activate_pull");

  return TRUE;
}

/* get the start and end times for syncing on this buffer */
static void
gst_timestampfiltersink_get_times (GstBaseSink * sink, GstBuffer * buffer,
    GstClockTime * start, GstClockTime * end)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "get_times");

}

/* propose allocation parameters for upstream */
static gboolean
gst_timestampfiltersink_propose_allocation (GstBaseSink * sink,
    GstQuery * query)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "propose_allocation");

  return TRUE;
}

/* start and stop processing, ideal for opening/closing the resource */
static gboolean
gst_timestampfiltersink_start (GstBaseSink * sink)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "start");

  return TRUE;
}

static gboolean
gst_timestampfiltersink_stop (GstBaseSink * sink)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "stop");

  return TRUE;
}

/* unlock any pending access to the resource. subclasses should unlock
 * any function ASAP. */
static gboolean
gst_timestampfiltersink_unlock (GstBaseSink * sink)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "unlock");

  return TRUE;
}

/* Clear a previously indicated unlock request not that unlocking is
 * complete. Sub-classes should clear any command queue or indicator they
 * set during unlock */
static gboolean
gst_timestampfiltersink_unlock_stop (GstBaseSink * sink)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "unlock_stop");

  return TRUE;
}

/* notify subclass of query */
static gboolean
gst_timestampfiltersink_query (GstBaseSink * sink, GstQuery * query)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "query");

  return TRUE;
}

/* notify subclass of event */
static gboolean
gst_timestampfiltersink_event (GstBaseSink * sink, GstEvent * event)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "event");

  return TRUE;
}

/* wait for eos or gap, subclasses should chain up to parent first */
static GstFlowReturn
gst_timestampfiltersink_wait_event (GstBaseSink * sink, GstEvent * event)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "wait_event");

  return GST_FLOW_OK;
}

/* notify subclass of buffer or list before doing sync */
static GstFlowReturn
gst_timestampfiltersink_prepare (GstBaseSink * sink, GstBuffer * buffer)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "prepare");

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_timestampfiltersink_prepare_list (GstBaseSink * sink,
    GstBufferList * buffer_list)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "prepare_list");

  return GST_FLOW_OK;
}

/* notify subclass of preroll buffer or real buffer */
static GstFlowReturn
gst_timestampfiltersink_preroll (GstBaseSink * sink, GstBuffer * buffer)
{
  GstTimestampfiltersink *timestampfiltersink = GST_TIMESTAMPFILTERSINK (sink);

  GST_DEBUG_OBJECT (timestampfiltersink, "preroll");

  return GST_FLOW_OK;
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
