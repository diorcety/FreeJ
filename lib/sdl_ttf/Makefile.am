AM_CPPFLAGS = -I$(top_srcdir) -I/usr/include/freetype2
CFLAGS = @CFLAGS@ @SDL_CFLAGS@ @FT2_CFLAGS@

AUTOMAKE_OPTIONS = foreign

noinst_LTLIBRARIES = libsdl_ttf.la

libsdl_ttf_la_SOURCES = SDL_ttf.c

libsdl_ttf_laincludedir = $(includedir)/freej/libs
libsdl_ttf_lainclude_HEADERS = SDL_ttf.h
