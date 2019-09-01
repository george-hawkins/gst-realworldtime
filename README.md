Real-world time GStreamer plugin
================================

The timestamps you usually see when dealing with frames in GStreamer are relative to when you started recording or generating data, i.e. they're offsets. I wanted a way to later recover the real-world time at which a frame was captured.

Prerequisites
-------------

In addition to the normal GStreamer dependencies, `libgstreamer1.0-dev` and `libgstreamer-plugins-base1.0-dev`, this plugin also requires `libpango1.0-dev`:

    $ sudo apt install libpango1.0-dev

And it needs the files `gstbasetextoverlay.h` and `gstbasetextoverlay.c` from [gst-plugins-base](https://github.com/GStreamer/gst-plugins-base).

First determine the version of GStreamer on your system:

    $ gst-launch-1.0 --version
    gst-launch-1.0 version 1.14.4
    GStreamer 1.14.4
    https://launchpad.net/distros/ubuntu/+source/gstreamer1.0

Then clone and checkout this version of gst-plugins-base:

    $ git clone https://github.com/GStreamer/gst-plugins-base.git
    $ cd gst-plugins-base
    $ git checkout 1.14.4
    $ cd ..

Then clone and checkout this project and copy over `gstbasetextoverlay.h` and `gstbasetextoverlay.c`:

    $ git clone https://github.com/george-hawkins/gst-realworldtime.git
    $ cp gst-plugins-base/ext/pango/gstbasetextoverlay.* gst-realworldtime/plugins

Unfortunately `gstbasetextoverlay.c`, in addition to defining `GstBaseTextOverlay`, also registers the GStreamer pango elements. This has to be removed:

    $ vim gst-realworldtime/plugins/gstbasetextoverlay.c

And at the start of the file remove the following lines:

    #include "gsttextoverlay.h"
    #include "gsttimeoverlay.h"
    #include "gstclockoverlay.h"
    #include "gsttextrender.h"

And at the end of the file remove the function `plugin_init` and the call to `GST_PLUGIN_DEFINE` below it.


Installation
------------

    $ cd gst-realworldtime
    $ ./autogen.sh
    $ ./configure --prefix=$HOME
    $ make install
    
Usage
-----

    $ export GST_PLUGIN_PATH=$HOME/.gstreamer-1.0/plugins
    $ gst-inspect-1.0 millisoverlay
    $ gst-inspect-1.0 timestampfiltersink
    $ gst-inspect-1.0 absolutetimestamp
    $ gst-launch-1.0 videotestsrc num-buffers=120 ! 'video/x-raw,width=1024,height=768,framerate=24/1' ! millisoverlay ! absolutetimestamp ! jpegenc ! avimux ! filesink sync=true location=out.avi
    $ gst-launch-1.0 filesrc location=out.avi ! avidemux ! timestampfiltersink

Library exports
---------------

Why doesn't `libgstpango.so` export symbols in the same ways as e.g. `libgstvideo-1.0.so`? Because they use different `export-symbols-regex` values:

    $ fgrep -r GST_LIB_LDFLAGS gst-plugins-base
    ...
    gst-plugins-base/configure.ac:GST_LIB_LDFLAGS="-export-symbols-regex ^_?\(gst_\|Gst\|GST_\).*"
    ...
    gst-plugins-base/gst-libs/gst/video/Makefile.am:libgstvideo_@GST_API_VERSION@_la_LDFLAGS = $(GST_LIB_LDFLAGS) $(GST_ALL_LDFLAGS) $(GST_LT_LDFLAGS)
    ...

    $ fgrep -r GST_PLUGIN_LDFLAGS gst-plugins-base/*
    ...
    gst-plugins-base/configure.ac:GST_PLUGIN_LDFLAGS="-module -avoid-version -export-symbols-regex '^[_]*gst_plugin_.*' $GST_ALL_LDFLAGS"
    ...
    gst-plugins-base/ext/pango/Makefile.am:libgstpango_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
    ...

This means `libgstvideo-1.0.so` exports a whole range of symbols, while `libgstpango.so` just export `gst_plugin_desc`:

    $ objdump -TC /usr/lib/x86_64-linux-gnu/libgstvideo-1.0.so | fgrep -w Base
    0000000000059e30 g    DF .text  000000000000005a  Base        gst_video_interlace_mode_get_type
    00000000000125e0 g    DF .text  000000000000003a  Base        gst_color_balance_get_type
    ...
    $ objdump -TC /usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgstpango.so | fgrep -w Base 
    0000000000213760 g    DO .data  0000000000000070  Base        gst_plugin_desc
