CC = g++-4.9
INCLUDE = -I/usr/local/include -D_THREAD_SAFE
LIB = -L/usr/local/lib -lSDL2 -lGLEW -lSOIL -lASSIMP
FRAMEWORK = -framework OpenGL
OPTS = -std=c++1y

QT_DEFINES       = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
QT_INCLUDE = -I/usr/local/Cellar/qt/4.8.6/mkspecs/unsupported/macx-clang-libc++ -I. -I/usr/local/Cellar/qt/4.8.6/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.6/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.6/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.6/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.6/include -I. -I. -F/usr/local/Cellar/qt/4.8.6/lib
QT_LIBS          = $(SUBLIBS) -F/usr/local/Cellar/qt/4.8.6/lib -L/usr/local/Cellar/qt/4.8.6/lib -framework QtGui -L/usr/local/Cellar/qt/4.8.6/lib -F/usr/local/Cellar/qt/4.8.6/lib -framework QtCore 

CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(notdir $(CPP_FILES:.cpp=.o))

all: main

main: $(OBJ_FILES)
	$(CC) $(FRAMEWORK) $(INCLUDE) $(QT_DEFINES) $(QT_INCLUDE) $(QT_LIBS) $(LIB) $(OPTS) -o $@ $^

%.o: %.cpp
	$(CC) $(FRAMEWORK) $(INCLUDE) $(QT_DEFINES) $(QT_INCLUDE) $(QT_LIBS) $(LIB) $(OPTS) -c -o $@ $<

clean:
	@- rm -f *.o
	@- rm -f main
