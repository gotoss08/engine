OBJS=util.o config.o data.o text_renderer.o list_widget.o screen_main_menu.o engine.o main.o

all: $(OBJS)

util.o: util.cpp util.h
	$(TRANSLATE_TO_OBJ) util.cpp

config.o: config.cpp config.h
	$(TRANSLATE_TO_OBJ) config.cpp

data.o: data.cpp data.h
	$(TRANSLATE_TO_OBJ) data.cpp

text_renderer.o: text_renderer.cpp text_renderer.h
	$(TRANSLATE_TO_OBJ) text_renderer.cpp

list_widget.o: list_widget.cpp list_widget.h
	$(TRANSLATE_TO_OBJ) list_widget.cpp

screen_main_menu.o: screen.h screens.h screen_main_menu.cpp screen_main_menu.h
	$(TRANSLATE_TO_OBJ) screen_main_menu.cpp

engine.o: engine.cpp engine.h
	$(TRANSLATE_TO_OBJ) engine.cpp

main.o: main.cpp
	$(TRANSLATE_TO_OBJ) main.cpp

clean:
	rm -f $(OBJS)