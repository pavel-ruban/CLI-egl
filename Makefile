OBJECTS = main.o gl.o utils.o input.o render.o renderUtils.o skeleton.o

all: $(OBJECTS)
	g++ -std=c++11 -o egl main.o gl.o utils.o input.o render.o renderUtils.o skeleton.o `pkg-config --cflags --libs x11 freetype2` -lGLESv2 -lEGL -lpcre -lboost_system -lboost_thread
	#g++ -o egl egl.o utils.o input.o `pkg-config --cflags --libs glesv2 egl x11` -lpcre -lboost_system -lboost_thread
	#rm egl.o utils.o input.o

main.o : main.cpp common.hpp context.hpp gl.hpp render.hpp input.h
	g++ -std=c++11 main.cpp -I/usr/include/freetype2 -Wall -O0 -g -c

gl.o : gl.cpp esUtil.h context.hpp common.hpp input.h
	g++ -std=c++11 gl.cpp -I/usr/include/freetype2 -Wall -O0 -g -c

render.o : render.cpp esUtil.h context.hpp common.hpp
	g++ -std=c++11 render.cpp -I/usr/include/freetype2 -Wall -O0 -g -c

renderUtils.o : renderUtils.cpp esUtil.h context.hpp
	g++ -std=c++11 renderUtils.cpp -I/usr/include/freetype2 -Wall -O0 -g -c

skeleton.o : skeleton.cpp common.hpp skeleton.hpp
	g++ -std=c++11 skeleton.cpp -I/usr/include/freetype2 -Wall -O0 -g -c

input.o : input.cpp input.h
	g++ input.cpp -I/usr/include/freetype2 -Wall -O0 -g -c

utils.o : esUtil.cpp esUtil.h
	g++ esUtil.cpp -o utils.o -I/usr/include/freetype2 -Wall -O0 -g -c
