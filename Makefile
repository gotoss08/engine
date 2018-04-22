OBJS=config.o data.o engine.o main.o pugixml.o text.o util.o
CC=g++
CFLAGS=-O0 -g3 -Wall -mconsole -mwindows -static-libgcc -m64
LIKER_FLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
OBJ_NAME = bin/engine64.exe

COMPILE=$(CC) $(CFLAGS) -c

engine: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(OBJ_NAME) $(LIKER_FLAGS)

config.o: config.cpp
	$(COMPILE) config.cpp

data.o: data.cpp
	$(COMPILE) data.cpp

engine.o: engine.cpp
	$(COMPILE) engine.cpp

main.o: main.cpp
	$(COMPILE) main.cpp

pugixml.o: pugixml.cpp
	$(COMPILE) pugixml.cpp

text.o: text.cpp
	$(COMPILE) text.cpp

util.o: util.cpp
	$(COMPILE) util.cpp

clean:
	rm $(OBJ_NAME) $(OBJS)