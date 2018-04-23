OBJS=pugixml.o util.o config.o data.o text.o engine.o main.o
CC=g++
CFLAGS=-O0 -g3 -Wall -mconsole -mwindows -static-libgcc -m64
LIKER_FLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
OBJ_NAME = bin/engine64.exe

COMPILE=$(CC) $(CFLAGS) -c

engine: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(OBJ_NAME) $(LIKER_FLAGS)

pugixml.o: pugixml.cpp
	$(COMPILE) pugixml.cpp

util.o: util.cpp
	$(COMPILE) util.cpp

config.o: config.cpp
	$(COMPILE) config.cpp

data.o: data.cpp
	$(COMPILE) data.cpp

text.o: text.cpp
	$(COMPILE) text.cpp

engine.o: engine.cpp
	$(COMPILE) engine.cpp

main.o: main.cpp
	$(COMPILE) main.cpp

clean:
	rm $(OBJ_NAME) $(OBJS)