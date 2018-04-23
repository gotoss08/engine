OBJS=pugixml.o format.o

COMPILE=$(CC) $(COMPILER_FLAGS) -c

all: $(OBJS)

pugixml.o: pugixml.cpp
	$(TRANSLATE_TO_OBJ) pugixml.cpp

format.o: format.cc format.h
	$(TRANSLATE_TO_OBJ) format.cc

clean:
	rm -f $(OBJS)