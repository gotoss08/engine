export CC=g++
export COMPILER_FLAGS=-O0 -g3 -Wall -mconsole -mwindows -static-libgcc -m64
export LIKER_FLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

OBJ_NAME = bin/engine64.exe

export TRANSLATE_TO_OBJ=$(CC) $(COMPILER_FLAGS) -c

all: libs engine
	$(CC) -o $(OBJ_NAME) src/*.o src/libs/*.o $(LIKER_FLAGS)

engine:
	$(MAKE) -C src

libs:
	$(MAKE) -C src/libs

clean-all: clean clean-libs

clean:
	$(MAKE) -C src clean
	rm bin/engine64.exe

clean-libs:
	$(MAKE) -C src/libs clean 