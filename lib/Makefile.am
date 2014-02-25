if BUILD_FLASH
FLASHDIR = flash
endif

if BUILD_JAVASCRIPT_ENGINE
JAVASCRIPTDIR = javascript
endif

if BUILD_SDLGFX
SDLGFXDIR = sdl_gfx
endif

if BUILD_SDLTTF
SDLTTFDIR = sdl_ttf
endif

if BUILD_SDLIMAGE
SDLIMAGEDIR = sdl_image
endif

if BUILD_CONSOLE
SLW = slw
endif

SUBDIRS = ccvt \
	$(SLW)           \
	$(SDLIMAGEDIR)   \
	$(SDLTTFDIR)     \
	$(SDLGFXDIR)     \
	$(JAVASCRIPTDIR) \
	$(FLASHDIR)

## XXX(godog) after lib/javascript is gone remove this
#DIST_SUBDIRS = ccvt \
#	slw              \
#	sdl_image        \
#	sdl_ttf          \
#	sdl_gfx          \
#	flash
#
#EXTRA_DIST = javascript
