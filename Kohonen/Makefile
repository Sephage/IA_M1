all: OGL_kohonen

base_opengl.o: base_opengl.c base_opengl.h kohonen.h
	gcc -c base_opengl.c

kohonen.o: kohonen.c kohonen.h
	gcc -c kohonen.c

read.o : read.c kohonen.h
	gcc -c read.c

OGL_kohonen: kohonen.o read.o base_opengl.o
	gcc -o $@ kohonen.o read.o base_opengl.o -L/usr/X11R6/include/GL -L/usr/X11R6/lib -lGL -lGLU -lglut -lm -lX11 -g

clean:
	rm -fr OGL_kohonen *.o
