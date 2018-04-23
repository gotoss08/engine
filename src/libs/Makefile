OBJS=format.o pugixml.o

COMPILE=$(CC) $(COMPILER_FLAGS) -c

all: $(OBJS)

format.o: format.cc
	$(TRANSLATE_TO_OBJ) format.cc

pugixml.o: pugixml.cpp
	$(TRANSLATE_TO_OBJ) pugixml.cpp

clean:
	rm $(OBJS)