if BUILD_SWIG_PYTHON
SWIG_PYTHON_DIR = python
endif

if BUILD_SWIG_RUBY
SWIG_RUBY_DIR = ruby
endif

if BUILD_SWIG_JAVA
SWIG_JAVA_DIR = java
endif

if BUILD_SWIG_CSHARP
SWIG_CSHARP_DIR = csharp
endif

if BUILD_SWIG_PERL
SWIG_PERL_DIR = perl
endif

if BUILD_SWIG_JAVASCRIPT
SWIG_JAVASCRIPT_DIR = javascript
endif

SUBDIRS = $(SWIG_PYTHON_DIR) \
          $(SWIG_RUBY_DIR) \
          $(SWIG_JAVA_DIR) \
          $(SWIG_CSHARP_DIR) \
          $(SWIG_PERL_DIR) \
          $(SWIG_JAVASCRIPT_DIR)

otherincludedir = $(includedir)/freej_swig/
otherinclude_HEADERS = \
    pypost.i \
    pypre.i \
    javapost.i \
    javapre.i \
    csharpost.i \
    csharppre.i \
    perlpost.i \
    perlpre.i \
    rbpost.i \
    rbpre.i \
    luapost.i \
    luapre.i \
    freej_sharedptr_pre.i \
    freej_list_pre.i \
    freej.i \
    freej_sharedptr_post.i \
    freej_list_post.i \
    macros.i \
    headers.i

