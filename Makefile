EGL = egl.o utils.o

all: $(EGL)
	gcc -o egl egl.o utils.o `pkg-config --cflags --libs glesv2 egl x11`
	rm ./*.o

egl.o : egl.c
	gcc egl.c -Wall -O0 -g -c

utils.o : esUtil.c esUtil.h
	gcc esUtil.c -o utils.o -Wall -O0 -g -c
