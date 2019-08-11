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
#ifndef _GST_ABSOLUTETIMESTAMP_H_
#define _GST_ABSOLUTETIMESTAMP_H_

#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_ABSOLUTETIMESTAMP   (gst_absolutetimestamp_get_type())
#define GST_ABSOLUTETIMESTAMP(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ABSOLUTETIMESTAMP,GstAbsolutetimestamp))
#define GST_ABSOLUTETIMESTAMP_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ABSOLUTETIMESTAMP,GstAbsolutetimestampClass))
#define GST_IS_ABSOLUTETIMESTAMP(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ABSOLUTETIMESTAMP))
#define GST_IS_ABSOLUTETIMESTAMP_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ABSOLUTETIMESTAMP))

typedef struct _GstAbsolutetimestamp GstAbsolutetimestamp;
typedef struct _GstAbsolutetimestampClass GstAbsolutetimestampClass;

struct _GstAbsolutetimestamp
{
  GstBaseTransform base_absolutetimestamp;

};

struct _GstAbsolutetimestampClass
{
  GstBaseTransformClass base_absolutetimestamp_class;
};

GType gst_absolutetimestamp_get_type (void);

G_END_DECLS

#endif
