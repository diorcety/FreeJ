.NOTPARALLEL: 
swig_source = freej_js.cpp
swig_header =

BUILT_SOURCES = $(swig_source) $(swig_header)

$(swig_source): $(SWIG_IFACE)
		$(SWIG_COMMAND) $(SWIG_JAVASCRIPT_OPT) -o $(swig_source) $(SWIG_IFACE)

lib_LTLIBRARIES = freej_js.la
nodist_freej_js_la_SOURCES = $(swig_source)
freej_js_la_CXXFLAGS = $(FREEJ_CFLAGS) $(SWIG_JAVASCRIPT_CPPFLAGS) -Wno-unused-but-set-variable -Wno-unknown-warning-option -Wno-unused-variable
freej_js_la_LDFLAGS = -module -avoid-version -Wl,-no-undefined
freej_js_la_LIBADD = $(top_builddir)/src/libfreej.la -lv8

CLEANFILES = $(BUILT_SOURCES)
