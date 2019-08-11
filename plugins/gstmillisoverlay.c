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
}
static void
gst_millisoverlay_init (GstMillisoverlay * millisoverlay)
{
  millisoverlay->sinkpad =
      gst_pad_new_from_static_template (&gst_millisoverlay_sink_template,
      "sink");

  millisoverlay->srcpad =
      gst_pad_new_from_static_template (&gst_millisoverlay_src_template, "src");
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
