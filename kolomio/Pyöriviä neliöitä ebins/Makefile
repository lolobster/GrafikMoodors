CPPFLAGS=-I/usr/include/GLFW -I.
LIBS=-lGL -lglfw -lGLEW

all:
	g++ -c common/shader.cpp -o common/shader.o $(CPPFLAGS)
	g++ -c main.cpp -o main.o $(CPPFLAGS)
	g++ -c renderer.cpp -o renderer.o $(CPPFLAGS)
	g++ common/shader.o main.o renderer.o -o test $(LIBS)
	./test
