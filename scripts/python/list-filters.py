#!/usr/bin/env python
# System wide useful modules
import threading
import time
import sys
import freej

# initializes FreeJ creating a Context
cx = freej.Context()
for i in cx.filters:
	print i.name
