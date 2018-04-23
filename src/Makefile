OBJS=util.o config.o data.o text_renderer.o engine.o main.o

all: $(OBJS)

util.o: util.cpp util.h
	$(TRANSLATE_TO_OBJ) util.cpp

config.o: config.cpp config.h
	$(TRANSLATE_TO_OBJ) config.cpp

data.o: data.cpp data.h
	$(TRANSLATE_TO_OBJ) data.cpp

text_renderer.o: text_renderer.cpp text_renderer.h
	$(TRANSLATE_TO_OBJ) text_renderer.cpp

engine.o: engine.cpp engine.h
	$(TRANSLATE_TO_OBJ) engine.cpp

main.o: main.cpp
	$(TRANSLATE_TO_OBJ) main.cpp

clean:
	rm $(OBJS)