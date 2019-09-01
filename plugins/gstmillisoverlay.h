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
#ifndef _GST_MILLISOVERLAY_H_
#define _GST_MILLISOVERLAY_H_

#include "gstbasetextoverlay.h"

G_BEGIN_DECLS

#define GST_TYPE_MILLISOVERLAY   (gst_millisoverlay_get_type())
#define GST_MILLISOVERLAY(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MILLISOVERLAY,GstMillisoverlay))
#define GST_MILLISOVERLAY_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MILLISOVERLAY,GstMillisoverlayClass))
#define GST_IS_MILLISOVERLAY(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MILLISOVERLAY))
#define GST_IS_MILLISOVERLAY_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MILLISOVERLAY))

typedef struct _GstMillisoverlay GstMillisoverlay;
typedef struct _GstMillisoverlayClass GstMillisoverlayClass;

struct _GstMillisoverlay
{
  GstBaseTextOverlay base_millisoverlay;

  GstPad *sinkpad;
  GstPad *srcpad;
};

struct _GstMillisoverlayClass
{
  GstBaseTextOverlayClass base_millisoverlay_class;
};

GType gst_millisoverlay_get_type (void);

G_END_DECLS

#endif
