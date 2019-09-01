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
#include "gstmillisoverlay.h"
#include "gsttimestampfiltersink.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "absolutetimestamp", GST_RANK_NONE, GST_TYPE_ABSOLUTETIMESTAMP)) {
      return FALSE;
  }
  if (!gst_element_register (plugin, "millisoverlay", GST_RANK_NONE, GST_TYPE_MILLISOVERLAY)) {
      return FALSE;
  }
  if (!gst_element_register (plugin, "timestampfiltersink", GST_RANK_NONE, GST_TYPE_TIMESTAMPFILTERSINK)) {
      return FALSE;
  }

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    realworldtime,
    "record and extract frames based on real-world time",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME,
    "https://github.com/george-hawkins/gst-realworldtime")
