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
 * SECTION:element-gstmillisoverlay
 *
 * The millisoverlay element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! millisoverlay ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/gst.h>
#include "gstmillisoverlay.h"

GST_DEBUG_CATEGORY_STATIC (gst_millisoverlay_debug_category);
#define GST_CAT_DEFAULT gst_millisoverlay_debug_category

/* prototypes */


static void gst_millisoverlay_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_millisoverlay_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_millisoverlay_dispose (GObject * object);
static void gst_millisoverlay_finalize (GObject * object);

static GstPad *gst_millisoverlay_request_new_pad (GstElement * element,
    GstPadTemplate * templ, const gchar * name);
static void gst_millisoverlay_release_pad (GstElement * element, GstPad * pad);
static GstStateChangeReturn
gst_millisoverlay_change_state (GstElement * element,
    GstStateChange transition);
static GstClock *gst_millisoverlay_provide_clock (GstElement * element);
static gboolean gst_millisoverlay_set_clock (GstElement * element,
    GstClock * clock);
static GstIndex *gst_millisoverlay_get_index (GstElement * element);
static void gst_millisoverlay_set_index (GstElement * element,
    GstIndex * index);
static gboolean gst_millisoverlay_send_event (GstElement * element,
    GstEvent * event);
static gboolean gst_millisoverlay_query (GstElement * element,
    GstQuery * query);

static GstCaps *gst_millisoverlay_sink_getcaps (GstPad * pad);
static gboolean gst_millisoverlay_sink_setcaps (GstPad * pad, GstCaps * caps);
static gboolean gst_millisoverlay_sink_acceptcaps (GstPad * pad,
    GstCaps * caps);
static void gst_millisoverlay_sink_fixatecaps (GstPad * pad, GstCaps * caps);
static gboolean gst_millisoverlay_sink_activate (GstPad * pad);
static gboolean gst_millisoverlay_sink_activatepush (GstPad * pad,
    gboolean active);
static gboolean gst_millisoverlay_sink_activatepull (GstPad * pad,
    gboolean active);
static GstPadLinkReturn gst_millisoverlay_sink_link (GstPad * pad,
    GstPad * peer);
static void gst_millisoverlay_sink_unlink (GstPad * pad);
static GstFlowReturn gst_millisoverlay_sink_chain (GstPad * pad,
    GstBuffer * buffer);
static GstFlowReturn gst_millisoverlay_sink_chainlist (GstPad * pad,
    GstBufferList * bufferlist);
static gboolean gst_millisoverlay_sink_event (GstPad * pad, GstEvent * event);
static gboolean gst_millisoverlay_sink_query (GstPad * pad, GstQuery * query);
static GstFlowReturn gst_millisoverlay_sink_bufferalloc (GstPad * pad,
    guint64 offset, guint size, GstCaps * caps, GstBuffer ** buf);
static GstIterator *gst_millisoverlay_sink_iterintlink (GstPad * pad);


static GstCaps *gst_millisoverlay_src_getcaps (GstPad * pad);
static gboolean gst_millisoverlay_src_setcaps (GstPad * pad, GstCaps * caps);
static gboolean gst_millisoverlay_src_acceptcaps (GstPad * pad, GstCaps * caps);
static void gst_millisoverlay_src_fixatecaps (GstPad * pad, GstCaps * caps);
static gboolean gst_millisoverlay_src_activate (GstPad * pad);
static gboolean gst_millisoverlay_src_activatepush (GstPad * pad,
    gboolean active);
static gboolean gst_millisoverlay_src_activatepull (GstPad * pad,
    gboolean active);
static GstPadLinkReturn gst_millisoverlay_src_link (GstPad * pad,
    GstPad * peer);
static void gst_millisoverlay_src_unlink (GstPad * pad);
static GstFlowReturn gst_millisoverlay_src_getrange (GstPad * pad,
    guint64 offset, guint length, GstBuffer ** buffer);
static gboolean gst_millisoverlay_src_event (GstPad * pad, GstEvent * event);
static gboolean gst_millisoverlay_src_query (GstPad * pad, GstQuery * query);
static GstIterator *gst_millisoverlay_src_iterintlink (GstPad * pad);


enum
{
  PROP_0
};

/* pad templates */

static GstStaticPadTemplate gst_millisoverlay_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/unknown")
    );

static GstStaticPadTemplate gst_millisoverlay_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/unknown")
    );


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstMillisoverlay, gst_millisoverlay, GST_TYPE_ELEMENT,
    GST_DEBUG_CATEGORY_INIT (gst_millisoverlay_debug_category, "millisoverlay",
        0, "debug category for millisoverlay element"));

static void
gst_millisoverlay_class_init (GstMillisoverlayClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (element_class,
      &gst_millisoverlay_sink_template);
  gst_element_class_add_static_pad_template (element_class,
      &gst_millisoverlay_src_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
      "FIXME Long name", "Generic", "FIXME Description",
      "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_millisoverlay_set_property;
  gobject_class->get_property = gst_millisoverlay_get_property;
  gobject_class->dispose = gst_millisoverlay_dispose;
  gobject_class->finalize = gst_millisoverlay_finalize;
  element_class->request_new_pad =
      GST_DEBUG_FUNCPTR (gst_millisoverlay_request_new_pad);
  element_class->release_pad =
      GST_DEBUG_FUNCPTR (gst_millisoverlay_release_pad);
  element_class->change_state =
      GST_DEBUG_FUNCPTR (gst_millisoverlay_change_state);
  element_class->provide_clock =
      GST_DEBUG_FUNCPTR (gst_millisoverlay_provide_clock);
  element_class->set_clock = GST_DEBUG_FUNCPTR (gst_millisoverlay_set_clock);
  element_class->get_index = GST_DEBUG_FUNCPTR (gst_millisoverlay_get_index);
  element_class->set_index = GST_DEBUG_FUNCPTR (gst_millisoverlay_set_index);
  element_class->send_event = GST_DEBUG_FUNCPTR (gst_millisoverlay_send_event);
  element_class->query = GST_DEBUG_FUNCPTR (gst_millisoverlay_query);

}

static void
gst_millisoverlay_init (GstMillisoverlay * millisoverlay)
{

  millisoverlay->sinkpad =
      gst_pad_new_from_static_template (&gst_millisoverlay_sink_template,
      "sink");
  gst_pad_set_getcaps_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_getcaps));
  gst_pad_set_setcaps_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_setcaps));
  gst_pad_set_acceptcaps_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_acceptcaps));
  gst_pad_set_fixatecaps_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_fixatecaps));
  gst_pad_set_activate_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_activate));
  gst_pad_set_activatepush_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_activatepush));
  gst_pad_set_activatepull_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_activatepull));
  gst_pad_set_link_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_link));
  gst_pad_set_unlink_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_unlink));
  gst_pad_set_chain_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_chain));
  gst_pad_set_chain_list_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_chainlist));
  gst_pad_set_event_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_event));
  gst_pad_set_query_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_query));
  gst_pad_set_bufferalloc_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_bufferalloc));
  gst_pad_set_iterate_internal_links_function (millisoverlay->sinkpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_sink_iterintlink));
  gst_element_add_pad (GST_ELEMENT (millisoverlay), millisoverlay->sinkpad);



  millisoverlay->srcpad =
      gst_pad_new_from_static_template (&gst_millisoverlay_src_template, "src");
  gst_pad_set_getcaps_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_getcaps));
  gst_pad_set_setcaps_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_setcaps));
  gst_pad_set_acceptcaps_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_acceptcaps));
  gst_pad_set_fixatecaps_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_fixatecaps));
  gst_pad_set_activate_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_activate));
  gst_pad_set_activatepush_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_activatepush));
  gst_pad_set_activatepull_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_activatepull));
  gst_pad_set_link_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_link));
  gst_pad_set_unlink_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_unlink));
  gst_pad_set_getrange_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_getrange));
  gst_pad_set_event_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_event));
  gst_pad_set_query_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_query));
  gst_pad_set_iterate_internal_links_function (millisoverlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_millisoverlay_src_iterintlink));
  gst_element_add_pad (GST_ELEMENT (millisoverlay), millisoverlay->srcpad);


}

void
gst_millisoverlay_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstMillisoverlay *millisoverlay = GST_MILLISOVERLAY (object);

  GST_DEBUG_OBJECT (millisoverlay, "set_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_millisoverlay_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstMillisoverlay *millisoverlay = GST_MILLISOVERLAY (object);

  GST_DEBUG_OBJECT (millisoverlay, "get_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_millisoverlay_dispose (GObject * object)
{
  GstMillisoverlay *millisoverlay = GST_MILLISOVERLAY (object);

  GST_DEBUG_OBJECT (millisoverlay, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_millisoverlay_parent_class)->dispose (object);
}

void
gst_millisoverlay_finalize (GObject * object)
{
  GstMillisoverlay *millisoverlay = GST_MILLISOVERLAY (object);

  GST_DEBUG_OBJECT (millisoverlay, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_millisoverlay_parent_class)->finalize (object);
}



static GstPad *
gst_millisoverlay_request_new_pad (GstElement * element, GstPadTemplate * templ,
    const gchar * name)
{

  return NULL;
}

static void
gst_millisoverlay_release_pad (GstElement * element, GstPad * pad)
{

}

static GstStateChangeReturn
gst_millisoverlay_change_state (GstElement * element, GstStateChange transition)
{
  GstMillisoverlay *millisoverlay;
  GstStateChangeReturn ret;

  g_return_val_if_fail (GST_IS_MILLISOVERLAY (element),
      GST_STATE_CHANGE_FAILURE);
  millisoverlay = GST_MILLISOVERLAY (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }

  return ret;
}

static GstClock *
gst_millisoverlay_provide_clock (GstElement * element)
{

  return NULL;
}

static gboolean
gst_millisoverlay_set_clock (GstElement * element, GstClock * clock)
{

  return GST_ELEMENT_CLASS (parent_class)->set_clock (element, clock);
}

static GstIndex *
gst_millisoverlay_get_index (GstElement * element)
{

  return NULL;
}

static void
gst_millisoverlay_set_index (GstElement * element, GstIndex * index)
{

}

static gboolean
gst_millisoverlay_send_event (GstElement * element, GstEvent * event)
{

  return TRUE;
}

static gboolean
gst_millisoverlay_query (GstElement * element, GstQuery * query)
{
  GstMillisoverlay *millisoverlay = GST_MILLISOVERLAY (element);
  gboolean ret;

  GST_DEBUG_OBJECT (millisoverlay, "query");

  switch (GST_QUERY_TYPE (query)) {
    default:
      ret = GST_ELEMENT_CLASS (parent_class)->query (element, query);
      break;
  }

  return ret;
}

static GstCaps *
gst_millisoverlay_sink_getcaps (GstPad * pad)
{
  GstMillisoverlay *millisoverlay;
  GstCaps *caps;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "getcaps");

  caps = gst_caps_copy (gst_pad_get_pad_template_caps (pad));

  gst_object_unref (millisoverlay);
  return caps;
}

static gboolean
gst_millisoverlay_sink_setcaps (GstPad * pad, GstCaps * caps)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "setcaps");


  gst_object_unref (millisoverlay);
  return TRUE;
}

static gboolean
gst_millisoverlay_sink_acceptcaps (GstPad * pad, GstCaps * caps)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "acceptcaps");


  gst_object_unref (millisoverlay);
  return TRUE;
}

static void
gst_millisoverlay_sink_fixatecaps (GstPad * pad, GstCaps * caps)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "fixatecaps");


  gst_object_unref (millisoverlay);
}

static gboolean
gst_millisoverlay_sink_activate (GstPad * pad)
{
  GstMillisoverlay *millisoverlay;
  gboolean ret;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "activate");

  if (gst_pad_check_pull_range (pad)) {
    GST_DEBUG_OBJECT (pad, "activating pull");
    ret = gst_pad_activate_pull (pad, TRUE);
  } else {
    GST_DEBUG_OBJECT (pad, "activating push");
    ret = gst_pad_activate_push (pad, TRUE);
  }

  gst_object_unref (millisoverlay);
  return ret;
}

static gboolean
gst_millisoverlay_sink_activatepush (GstPad * pad, gboolean active)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "activatepush");


  gst_object_unref (millisoverlay);
  return TRUE;
}

static gboolean
gst_millisoverlay_sink_activatepull (GstPad * pad, gboolean active)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "activatepull");


  gst_object_unref (millisoverlay);
  return TRUE;
}

static GstPadLinkReturn
gst_millisoverlay_sink_link (GstPad * pad, GstPad * peer)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "link");


  gst_object_unref (millisoverlay);
  return GST_PAD_LINK_OK;
}

static void
gst_millisoverlay_sink_unlink (GstPad * pad)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "unlink");


  gst_object_unref (millisoverlay);
}

static GstFlowReturn
gst_millisoverlay_sink_chain (GstPad * pad, GstBuffer * buffer)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "chain");


  gst_object_unref (millisoverlay);
  return GST_FLOW_OK;
}

static GstFlowReturn
gst_millisoverlay_sink_chainlist (GstPad * pad, GstBufferList * bufferlist)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "chainlist");


  gst_object_unref (millisoverlay);
  return GST_FLOW_OK;
}

static gboolean
gst_millisoverlay_sink_event (GstPad * pad, GstEvent * event)
{
  gboolean res;
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "event");

  switch (GST_EVENT_TYPE (event)) {
    default:
      res = gst_pad_event_default (pad, event);
      break;
  }

  gst_object_unref (millisoverlay);
  return res;
}

static gboolean
gst_millisoverlay_sink_query (GstPad * pad, GstQuery * query)
{
  gboolean res;
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "query");

  switch (GST_QUERY_TYPE (query)) {
    default:
      res = gst_pad_query_default (pad, query);
      break;
  }

  gst_object_unref (millisoverlay);
  return res;
}

static GstFlowReturn
gst_millisoverlay_sink_bufferalloc (GstPad * pad, guint64 offset, guint size,
    GstCaps * caps, GstBuffer ** buf)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "bufferalloc");


  *buf = gst_buffer_new_and_alloc (size);
  gst_buffer_set_caps (*buf, caps);

  gst_object_unref (millisoverlay);
  return GST_FLOW_OK;
}

static GstIterator *
gst_millisoverlay_sink_iterintlink (GstPad * pad)
{
  GstMillisoverlay *millisoverlay;
  GstIterator *iter;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "iterintlink");

  iter = gst_pad_iterate_internal_links_default (pad);

  gst_object_unref (millisoverlay);
  return iter;
}


static GstCaps *
gst_millisoverlay_src_getcaps (GstPad * pad)
{
  GstMillisoverlay *millisoverlay;
  GstCaps *caps;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "getcaps");

  caps = gst_pad_get_pad_template_caps (pad);

  gst_object_unref (millisoverlay);
  return caps;
}

static gboolean
gst_millisoverlay_src_setcaps (GstPad * pad, GstCaps * caps)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "setcaps");


  gst_object_unref (millisoverlay);
  return TRUE;
}

static gboolean
gst_millisoverlay_src_acceptcaps (GstPad * pad, GstCaps * caps)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "acceptcaps");


  gst_object_unref (millisoverlay);
  return TRUE;
}

static void
gst_millisoverlay_src_fixatecaps (GstPad * pad, GstCaps * caps)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "fixatecaps");


  gst_object_unref (millisoverlay);
}

static gboolean
gst_millisoverlay_src_activate (GstPad * pad)
{
  GstMillisoverlay *millisoverlay;
  gboolean ret;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "activate");

  if (gst_pad_check_pull_range (pad)) {
    GST_DEBUG_OBJECT (pad, "activating pull");
    ret = gst_pad_activate_pull (pad, TRUE);
  } else {
    GST_DEBUG_OBJECT (pad, "activating push");
    ret = gst_pad_activate_push (pad, TRUE);
  }

  gst_object_unref (millisoverlay);
  return ret;
}

static gboolean
gst_millisoverlay_src_activatepush (GstPad * pad, gboolean active)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "activatepush");


  gst_object_unref (millisoverlay);
  return TRUE;
}

static gboolean
gst_millisoverlay_src_activatepull (GstPad * pad, gboolean active)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "activatepull");


  gst_object_unref (millisoverlay);
  return TRUE;
}

static GstPadLinkReturn
gst_millisoverlay_src_link (GstPad * pad, GstPad * peer)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "link");


  gst_object_unref (millisoverlay);
  return GST_PAD_LINK_OK;
}

static void
gst_millisoverlay_src_unlink (GstPad * pad)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "unlink");


  gst_object_unref (millisoverlay);
}

static GstFlowReturn
gst_millisoverlay_src_getrange (GstPad * pad, guint64 offset, guint length,
    GstBuffer ** buffer)
{
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "getrange");


  gst_object_unref (millisoverlay);
  return GST_FLOW_OK;
}

static gboolean
gst_millisoverlay_src_event (GstPad * pad, GstEvent * event)
{
  gboolean res;
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "event");

  switch (GST_EVENT_TYPE (event)) {
    default:
      res = gst_pad_event_default (pad, event);
      break;
  }

  gst_object_unref (millisoverlay);
  return res;
}

static gboolean
gst_millisoverlay_src_query (GstPad * pad, GstQuery * query)
{
  gboolean res;
  GstMillisoverlay *millisoverlay;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "query");

  switch (GST_QUERY_TYPE (query)) {
    default:
      res = gst_pad_query_default (pad, query);
      break;
  }

  gst_object_unref (millisoverlay);
  return res;
}

static GstIterator *
gst_millisoverlay_src_iterintlink (GstPad * pad)
{
  GstMillisoverlay *millisoverlay;
  GstIterator *iter;

  millisoverlay = GST_MILLISOVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (millisoverlay, "iterintlink");

  iter = gst_pad_iterate_internal_links_default (pad);

  gst_object_unref (millisoverlay);
  return iter;
}


static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "millisoverlay", GST_RANK_NONE,
      GST_TYPE_MILLISOVERLAY);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.FIXME"
#endif
#ifndef PACKAGE
#define PACKAGE "FIXME_package"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "FIXME_package_name"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    millisoverlay,
    "FIXME plugin description",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)
