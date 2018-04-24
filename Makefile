export CC=g++
export COMPILER_FLAGS=-O0 -g3 -Wall -static-libgcc -m64 -std=c++11 -lpthread -ldl -Wcpp -mwindows 
export LIKER_FLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

OBJ_NAME = bin/engine64.exe

export TRANSLATE_TO_OBJ=$(CC) $(COMPILER_FLAGS) -c

all: libs engine
	$(CC) -o $(OBJ_NAME) src/libs/*.o src/*.o $(LIKER_FLAGS)

engine:
	$(MAKE) -C src

libs:
	$(MAKE) -C src/libs

clean:
	$(MAKE) -C src clean
	rm bin/engine64.exe

clean-libs:
	$(MAKE) -C src/libs clean

clean-all: clean-libs clean