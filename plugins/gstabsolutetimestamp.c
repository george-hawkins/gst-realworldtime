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

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include "gstabsolutetimestamp.h"

GST_DEBUG_CATEGORY_STATIC (gst_absolutetimestamp_debug_category);
#define GST_CAT_DEFAULT gst_absolutetimestamp_debug_category

/* prototypes */


static void gst_absolutetimestamp_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_absolutetimestamp_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_absolutetimestamp_dispose (GObject * object);
static void gst_absolutetimestamp_finalize (GObject * object);

static GstCaps *gst_absolutetimestamp_transform_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, GstCaps * filter);
static GstCaps *gst_absolutetimestamp_fixate_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, GstCaps * othercaps);
static gboolean gst_absolutetimestamp_accept_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps);
static gboolean gst_absolutetimestamp_set_caps (GstBaseTransform * trans,
    GstCaps * incaps, GstCaps * outcaps);
static gboolean gst_absolutetimestamp_query (GstBaseTransform * trans,
    GstPadDirection direction, GstQuery * query);
static gboolean gst_absolutetimestamp_decide_allocation (GstBaseTransform *
    trans, GstQuery * query);
static gboolean gst_absolutetimestamp_filter_meta (GstBaseTransform * trans,
    GstQuery * query, GType api, const GstStructure * params);
static gboolean gst_absolutetimestamp_propose_allocation (GstBaseTransform *
    trans, GstQuery * decide_query, GstQuery * query);
static gboolean gst_absolutetimestamp_transform_size (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, gsize size, GstCaps * othercaps,
    gsize * othersize);
static gboolean gst_absolutetimestamp_get_unit_size (GstBaseTransform * trans,
    GstCaps * caps, gsize * size);
static gboolean gst_absolutetimestamp_start (GstBaseTransform * trans);
static gboolean gst_absolutetimestamp_stop (GstBaseTransform * trans);
static gboolean gst_absolutetimestamp_sink_event (GstBaseTransform * trans,
    GstEvent * event);
static gboolean gst_absolutetimestamp_src_event (GstBaseTransform * trans,
    GstEvent * event);
static GstFlowReturn
gst_absolutetimestamp_prepare_output_buffer (GstBaseTransform * trans,
    GstBuffer * input, GstBuffer ** outbuf);
static gboolean gst_absolutetimestamp_copy_metadata (GstBaseTransform * trans,
    GstBuffer * input, GstBuffer * outbuf);
static gboolean gst_absolutetimestamp_transform_meta (GstBaseTransform * trans,
    GstBuffer * outbuf, GstMeta * meta, GstBuffer * inbuf);
static void gst_absolutetimestamp_before_transform (GstBaseTransform * trans,
    GstBuffer * buffer);
static GstFlowReturn gst_absolutetimestamp_transform (GstBaseTransform * trans,
    GstBuffer * inbuf, GstBuffer * outbuf);
static GstFlowReturn gst_absolutetimestamp_transform_ip (GstBaseTransform *
    trans, GstBuffer * buf);

enum
{
  PROP_0
};

/* pad templates */

static GstStaticPadTemplate gst_absolutetimestamp_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/unknown")
    );

static GstStaticPadTemplate gst_absolutetimestamp_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/unknown")
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
      "FIXME Long name", "Generic", "FIXME Description",
      "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_absolutetimestamp_set_property;
  gobject_class->get_property = gst_absolutetimestamp_get_property;
  gobject_class->dispose = gst_absolutetimestamp_dispose;
  gobject_class->finalize = gst_absolutetimestamp_finalize;
  base_transform_class->transform_caps =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_transform_caps);
  base_transform_class->fixate_caps =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_fixate_caps);
  base_transform_class->accept_caps =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_accept_caps);
  base_transform_class->set_caps =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_set_caps);
  base_transform_class->query = GST_DEBUG_FUNCPTR (gst_absolutetimestamp_query);
  base_transform_class->decide_allocation =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_decide_allocation);
  base_transform_class->filter_meta =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_filter_meta);
  base_transform_class->propose_allocation =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_propose_allocation);
  base_transform_class->transform_size =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_transform_size);
  base_transform_class->get_unit_size =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_get_unit_size);
  base_transform_class->start = GST_DEBUG_FUNCPTR (gst_absolutetimestamp_start);
  base_transform_class->stop = GST_DEBUG_FUNCPTR (gst_absolutetimestamp_stop);
  base_transform_class->sink_event =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_sink_event);
  base_transform_class->src_event =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_src_event);
  base_transform_class->prepare_output_buffer =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_prepare_output_buffer);
  base_transform_class->copy_metadata =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_copy_metadata);
  base_transform_class->transform_meta =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_transform_meta);
  base_transform_class->before_transform =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_before_transform);
  base_transform_class->transform =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_transform);
  base_transform_class->transform_ip =
      GST_DEBUG_FUNCPTR (gst_absolutetimestamp_transform_ip);

}

static void
gst_absolutetimestamp_init (GstAbsolutetimestamp * absolutetimestamp)
{
}

void
gst_absolutetimestamp_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (object);

  GST_DEBUG_OBJECT (absolutetimestamp, "set_property");

  switch (property_id) {
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

  G_OBJECT_CLASS (gst_absolutetimestamp_parent_class)->dispose (object);
}

void
gst_absolutetimestamp_finalize (GObject * object)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (object);

  GST_DEBUG_OBJECT (absolutetimestamp, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_absolutetimestamp_parent_class)->finalize (object);
}

static GstCaps *
gst_absolutetimestamp_transform_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, GstCaps * filter)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);
  GstCaps *othercaps;

  GST_DEBUG_OBJECT (absolutetimestamp, "transform_caps");

  othercaps = gst_caps_copy (caps);

  /* Copy other caps and modify as appropriate */
  /* This works for the simplest cases, where the transform modifies one
   * or more fields in the caps structure.  It does not work correctly
   * if passthrough caps are preferred. */
  if (direction == GST_PAD_SRC) {
    /* transform caps going upstream */
  } else {
    /* transform caps going downstream */
  }

  if (filter) {
    GstCaps *intersect;

    intersect = gst_caps_intersect (othercaps, filter);
    gst_caps_unref (othercaps);

    return intersect;
  } else {
    return othercaps;
  }
}

static GstCaps *
gst_absolutetimestamp_fixate_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, GstCaps * othercaps)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "fixate_caps");

  return NULL;
}

static gboolean
gst_absolutetimestamp_accept_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "accept_caps");

  return TRUE;
}

static gboolean
gst_absolutetimestamp_set_caps (GstBaseTransform * trans, GstCaps * incaps,
    GstCaps * outcaps)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "set_caps");

  return TRUE;
}

static gboolean
gst_absolutetimestamp_query (GstBaseTransform * trans,
    GstPadDirection direction, GstQuery * query)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "query");

  return TRUE;
}

/* decide allocation query for output buffers */
static gboolean
gst_absolutetimestamp_decide_allocation (GstBaseTransform * trans,
    GstQuery * query)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "decide_allocation");

  return TRUE;
}

static gboolean
gst_absolutetimestamp_filter_meta (GstBaseTransform * trans, GstQuery * query,
    GType api, const GstStructure * params)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "filter_meta");

  return TRUE;
}

/* propose allocation query parameters for input buffers */
static gboolean
gst_absolutetimestamp_propose_allocation (GstBaseTransform * trans,
    GstQuery * decide_query, GstQuery * query)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "propose_allocation");

  return TRUE;
}

/* transform size */
static gboolean
gst_absolutetimestamp_transform_size (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, gsize size, GstCaps * othercaps,
    gsize * othersize)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "transform_size");

  return TRUE;
}

static gboolean
gst_absolutetimestamp_get_unit_size (GstBaseTransform * trans, GstCaps * caps,
    gsize * size)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "get_unit_size");

  return TRUE;
}

/* states */
static gboolean
gst_absolutetimestamp_start (GstBaseTransform * trans)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "start");

  return TRUE;
}

static gboolean
gst_absolutetimestamp_stop (GstBaseTransform * trans)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "stop");

  return TRUE;
}

/* sink and src pad event handlers */
static gboolean
gst_absolutetimestamp_sink_event (GstBaseTransform * trans, GstEvent * event)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "sink_event");

  return GST_BASE_TRANSFORM_CLASS (gst_absolutetimestamp_parent_class)->
      sink_event (trans, event);
}

static gboolean
gst_absolutetimestamp_src_event (GstBaseTransform * trans, GstEvent * event)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "src_event");

  return GST_BASE_TRANSFORM_CLASS (gst_absolutetimestamp_parent_class)->
      src_event (trans, event);
}

static GstFlowReturn
gst_absolutetimestamp_prepare_output_buffer (GstBaseTransform * trans,
    GstBuffer * input, GstBuffer ** outbuf)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "prepare_output_buffer");

  return GST_FLOW_OK;
}

/* metadata */
static gboolean
gst_absolutetimestamp_copy_metadata (GstBaseTransform * trans,
    GstBuffer * input, GstBuffer * outbuf)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "copy_metadata");

  return TRUE;
}

static gboolean
gst_absolutetimestamp_transform_meta (GstBaseTransform * trans,
    GstBuffer * outbuf, GstMeta * meta, GstBuffer * inbuf)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "transform_meta");

  return TRUE;
}

static void
gst_absolutetimestamp_before_transform (GstBaseTransform * trans,
    GstBuffer * buffer)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "before_transform");

}

/* transform */
static GstFlowReturn
gst_absolutetimestamp_transform (GstBaseTransform * trans, GstBuffer * inbuf,
    GstBuffer * outbuf)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "transform");

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_absolutetimestamp_transform_ip (GstBaseTransform * trans, GstBuffer * buf)
{
  GstAbsolutetimestamp *absolutetimestamp = GST_ABSOLUTETIMESTAMP (trans);

  GST_DEBUG_OBJECT (absolutetimestamp, "transform_ip");

  return GST_FLOW_OK;
}
