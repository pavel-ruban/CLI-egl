OBJECTS = main.o gl.o utils.o input.o render.o renderUtils.o skeleton.o

all: $(OBJECTS)
	g++ -std=c++11 -o egl main.o gl.o utils.o input.o render.o renderUtils.o skeleton.o `pkg-config --cflags --libs x11 freetype2` -lGLESv2 -lEGL -lpcre -lboost_system -lboost_thread
	#g++ -o egl egl.o utils.o input.o `pkg-config --cflags --libs glesv2 egl x11` -lpcre -lboost_system -lboost_thread
	#rm egl.o utils.o input.o

main.o : main.cpp input.h
	g++ -std=c++11 main.cpp -I/usr/include/freetype2 -Wall -O0 -g -c

gl.o : gl.cpp input.h
	g++ -std=c++11 egl.cpp -I/usr/include/freetype2 -Wall -O0 -g -c

input.o : input.cpp input.h
	g++ input.cpp -Wall -O0 -g -c

utils.o : esUtil.cpp esUtil.h
	g++ esUtil.cpp -o utils.o -Wall -O0 -g -c
