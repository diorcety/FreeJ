#!/usr/bin/env python
# System wide useful modules
import threading
import time
import freej
# initializes FreeJ creating a Context
cx = freej.Context()
# creates a screen (different to docs)
scr = freej.SdlScreen()
# Initialise the screen to the given size (I chose Widescreen PAL resolution) (not in docs)
scr.init(1024,576,32)
# adds the screen
cx.add_screen(scr)
# create an instance of a TextLayer
txt = freej.TextLayer()
# initializes the new layer with the freej context (different to docs - optionally takes x,y,bpp size)
txt.init()
# writes the hello world text inside the layer
txt.write("Hello World!")
# start the layer
txt.start()
# add the layer to the screen
cx.add_layer(txt)
# starts freej in a separate thread
th = threading.Thread(target = cx.start , name = "freej")
th.start()
