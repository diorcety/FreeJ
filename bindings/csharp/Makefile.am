# XXX(godog) what's missing? compilation and jar packaging?
.NOTPARALLEL: 
swig_source = freej_csharp.cpp
swig_header = freej_csharp.h

BUILT_SOURCES = $(swig_source) $(swig_header)

SWIG_CFLAGS = -DIN_FREEJ

$(swig_source): $(SWIG_IFACE)
	$(SWIG_COMMAND) $(SWIG_CFLAGS) -csharp -o $(swig_source) $(SWIG_IFACE)

pkgcsharpexecdir = ${libdir}/jni
pkgcsharpexec_LTLIBRARIES = libfreej_csharp.la

nodist_libfreej_csharp_la_SOURCES = $(swig_source)
libfreej_csharp_la_CXXFLAGS = $(FREEJ_CFLAGS) -Wno-strict-aliasing
libfreej_csharp_la_LDFLAGS = -Wl,-no-undefined
libfreej_csharp_la_LIBADD = $(top_builddir)/src/libfreej.la

CLEANFILES = $(BUILT_SOURCES)

clean-local:
	rm -f *.cs
