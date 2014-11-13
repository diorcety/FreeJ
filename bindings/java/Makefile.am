# XXX(godog) what's missing? compilation and jar packaging?
.NOTPARALLEL: 
swig_source = freej_java.cpp
swig_header = freej_java.h

BUILT_SOURCES = $(swig_source) $(swig_header)

SWIG_CFLAGS = -DIN_FREEJ

$(swig_source): $(SWIG_IFACE)
	$(SWIG_COMMAND) $(SWIG_CFLAGS) -java -o $(swig_source) $(SWIG_IFACE)

pkgjavaexecdir = ${libdir}/jni
pkgjavaexec_LTLIBRARIES = libfreej_java.la

nodist_libfreej_java_la_SOURCES = $(swig_source)
libfreej_java_la_CXXFLAGS = $(FREEJ_CFLAGS) $(JAVA_CFLAGS) -Wno-strict-aliasing
libfreej_java_la_LDFLAGS = $(JAVA_LDFLAGS) -Wl,-no-undefined
libfreej_java_la_LIBADD = $(top_builddir)/src/libfreej.la

CLEANFILES = $(BUILT_SOURCES)

clean-local:
	rm -f *.class
	rm -f *.java
