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
#ifndef _GST_TIMESTAMPFILTERSINK_H_
#define _GST_TIMESTAMPFILTERSINK_H_

#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS

#define GST_TYPE_TIMESTAMPFILTERSINK   (gst_timestampfiltersink_get_type())
#define GST_TIMESTAMPFILTERSINK(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_TIMESTAMPFILTERSINK,GstTimestampfiltersink))
#define GST_TIMESTAMPFILTERSINK_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_TIMESTAMPFILTERSINK,GstTimestampfiltersinkClass))
#define GST_IS_TIMESTAMPFILTERSINK(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_TIMESTAMPFILTERSINK))
#define GST_IS_TIMESTAMPFILTERSINK_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_TIMESTAMPFILTERSINK))

typedef struct _GstTimestampfiltersink GstTimestampfiltersink;
typedef struct _GstTimestampfiltersinkClass GstTimestampfiltersinkClass;

struct _GstTimestampfiltersink
{
  GstBaseSink base_timestampfiltersink;

  gchar *filename;
  gint index;
};

struct _GstTimestampfiltersinkClass
{
  GstBaseSinkClass base_timestampfiltersink_class;
};

GType gst_timestampfiltersink_get_type (void);

G_END_DECLS

#endif
