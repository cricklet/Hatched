CC = g++-4.9
INCLUDE = -I/usr/local/include -D_THREAD_SAFE
LIB = -L/usr/local/lib -lSDL2 -lGLEW -lSOIL -lASSIMP
FRAMEWORK = -framework OpenGL
OPTS = -std=c++11

CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(notdir $(CPP_FILES:.cpp=.o))

all: main

main: $(OBJ_FILES)
	$(CC) $(FRAMEWORK) $(INCLUDE) $(LIB) $(OPTS) -o $@ $^

%.o: %.cpp
	$(CC) $(FRAMEWORK) $(INCLUDE) $(LIB) $(OPTS) -c -o $@ $<

clean:
	@- rm -f *.o
	@- rm -f main
