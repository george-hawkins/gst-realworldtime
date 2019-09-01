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


static gchar * gst_time_overlay_render_time (GstMillisoverlay * overlay, GstClockTime time);
static gchar * gst_time_overlay_get_text (GstBaseTextOverlay * overlay, GstBuffer * video_frame);


enum
{
  PROP_0
};


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstMillisoverlay, gst_millisoverlay, GST_TYPE_ELEMENT,
    GST_DEBUG_CATEGORY_INIT (gst_millisoverlay_debug_category, "millisoverlay",
        0, "debug category for millisoverlay element"));

static void
gst_millisoverlay_class_init (GstMillisoverlayClass * klass)
{
  GstBaseTextOverlayClass *base_text_overlay_class = GST_BASE_TEXT_OVERLAY_CLASS (klass);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
      "Millis Overlay", "Filter/Editor/Video",
      "Overlays times and time stamps on a video stream",
      "George Hawkins <https://github.com/george-hawkins>");

  base_text_overlay_class->get_text = gst_time_overlay_get_text;

  g_mutex_lock (base_text_overlay_class->pango_lock);
  PangoContext *context = base_text_overlay_class->pango_context;

  pango_context_set_language (context, pango_language_from_string ("en_US"));
  pango_context_set_base_dir (context, PANGO_DIRECTION_LTR);

  PangoFontDescription *font_description = pango_font_description_new ();
  pango_font_description_set_family_static (font_description, "Monospace");
  pango_font_description_set_style (font_description, PANGO_STYLE_NORMAL);
  pango_font_description_set_variant (font_description, PANGO_VARIANT_NORMAL);
  pango_font_description_set_weight (font_description, PANGO_WEIGHT_NORMAL);
  pango_font_description_set_stretch (font_description, PANGO_STRETCH_NORMAL);
  pango_font_description_set_size (font_description, 18 * PANGO_SCALE);
  pango_context_set_font_description (context, font_description);
  pango_font_description_free (font_description);
  g_mutex_unlock (base_text_overlay_class->pango_lock);
}

static void
gst_millisoverlay_init (GstMillisoverlay * millisoverlay)
{
  GstBaseTextOverlay *gst_base_text_overlay = GST_BASE_TEXT_OVERLAY (millisoverlay);

  gst_base_text_overlay->valign = GST_BASE_TEXT_OVERLAY_VALIGN_TOP;
  gst_base_text_overlay->halign = GST_BASE_TEXT_OVERLAY_HALIGN_LEFT;
}

/* Called with lock held */
static gchar *
gst_time_overlay_get_text (GstBaseTextOverlay * overlay, GstBuffer * video_frame)
{
  overlay->need_render = TRUE;

  GstClockTime ts_buffer = GST_BUFFER_TIMESTAMP (video_frame);

  if (!GST_CLOCK_TIME_IS_VALID (ts_buffer)) {
    GST_DEBUG ("buffer without valid timestamp");
    return g_strdup ("");
  }

  GST_DEBUG ("buffer with timestamp %" GST_TIME_FORMAT, GST_TIME_ARGS (ts_buffer));

  gchar * time_str = gst_time_overlay_render_time (GST_MILLISOVERLAY (overlay), ts_buffer);

  gchar * txt = g_strdup (overlay->default_text);

  gchar *ret;

  if (txt != NULL && *txt != '\0') {
    ret = g_strdup_printf ("%s %s", txt, time_str);
  } else {
    ret = time_str;
    time_str = NULL;
  }

  g_free (txt);
  g_free (time_str);

  return ret;
}

static gchar *
gst_time_overlay_render_time (GstMillisoverlay * overlay, GstClockTime time)
{
  if (!GST_CLOCK_TIME_IS_VALID (time))
    return g_strdup ("");

  guint hours = (guint) (time / (GST_SECOND * 60 * 60));
  guint mins = (guint) ((time / (GST_SECOND * 60)) % 60);
  guint secs = (guint) ((time / GST_SECOND) % 60);
  guint msecs = (guint) ((time % GST_SECOND) / (1000 * 1000));

  return g_strdup_printf ("%u:%02u:%02u.%03u", hours, mins, secs, msecs);
}
