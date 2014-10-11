.NOTPARALLEL: 
swig_source = freej_py.cpp
swig_header = freej_py.h

python_module = freej.py

BUILT_SOURCES = $(swig_source) $(swig_header)

$(swig_source): $(SWIG_IFACE)
		$(SWIG_COMMAND) $(SWIG_PYTHON_OPT) -interface libfreej_py -threads -shadow -o $(swig_source) $(SWIG_IFACE)

pkgpython_PYTHON = __init__.py
nodist_pkgpython_PYTHON = $(python_module)
pkgpyexec_LTLIBRARIES = libfreej_py.la

nodist_libfreej_py_la_SOURCES = $(swig_source)
libfreej_py_la_CXXFLAGS = $(FREEJ_CFLAGS) $(SWIG_PYTHON_CPPFLAGS) -Wno-sometimes-uninitialized
libfreej_py_la_LDFLAGS = -module -avoid-version -Wl,-no-undefined $(SWIG_PYTHON_LDFLAGS)
libfreej_py_la_LIBADD = $(top_builddir)/src/libfreej.la

CLEANFILES = $(BUILT_SOURCES) $(python_module)
