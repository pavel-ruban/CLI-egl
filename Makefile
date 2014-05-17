CC := g++
CFLAGS := -std=c++11 -g -c -O0 -Wall -I/usr/include/freetype2
CCFLAGS := $(shell pkg-config --cflags --libs freetype2) -lm -lGLESv2 -lEGL -lpcre -lboost_system -lboost_thread


# Use DRM platform by default
ifndef X11_FLAG

DEPENDIES := main.o gl.o drm.o input.o render.o renderUtils.o skeleton.o utils.o interface.o
DRM := $(shell pkg-config --cflags --libs libdrm) -lgbm
CFLAGS += $(DRM)
CCFLAGS += $(DRM)
OBJECTS := main.o drm.o gl.o utils.o input.o render.o renderUtils.o skeleton.o interface.o

# Otherwise X11
else

DEPENDIES := main.o gl.o utils.o input.o render.o renderUtils.o skeleton.o 
OBJECTS := main.o gl.o utils.o input.o render.o renderUtils.o skeleton.o
CCFLAGS += $(shell pkg-conig --libs x11)

endif

# Default recipe.
all: $(DEPENDIES)
	$(CC) -o egl $(OBJECTS) $(CCFLAGS)

main.o : main.cpp common.hpp context.hpp gl.hpp render.hpp input.h platform.h
	$(CC) $(CFLAGS) main.cpp

gl.o : gl.cpp esUtil.h context.hpp common.hpp input.h platform.h context.hpp
	$(CC) $(CFLAGS) gl.cpp

render.o : render.cpp esUtil.h context.hpp common.hpp platform.h context.hpp
	$(CC) $(CFLAGS) render.cpp

renderUtils.o : renderUtils.cpp esUtil.h context.hpp platform.h
	$(CC) $(CFLAGS) renderUtils.cpp

interface.o : interface.cpp interface.hpp platform.h context.hpp
	$(CC) $(CFLAGS) interface.cpp

skeleton.o : skeleton.cpp common.hpp skeleton.hpp platform.h context.hpp
	$(CC) $(CFLAGS) skeleton.cpp

input.o : input.cpp input.h platform.h context.hpp
	$(CC) $(CFLAGS) input.cpp

utils.o : esUtil.cpp esUtil.h platform.h context.hpp
	$(CC) $(CFLAGS) esUtil.cpp -o utils.o

drm.o : drm.cpp drm.hpp platform.h context.hpp
	$(CC) $(CFLAGS) drm.cpp
