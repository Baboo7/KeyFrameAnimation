NAME_EXE=KeyFrameAnimator
FLAGS=-std=c++11 -W -Wall
LIBS=-lsfml-graphics -lsfml-window -lsfml-system
# DEPENDANCES= -I$(SFML_INCLUDE) -L$(SFML_LIBS) $(LIBS)

all: main

main: main.o GUI.o tree.o sprite_manager.o time_line.o
	g++ main.o GUI.o tree.o sprite_manager.o time_line.o -o $(NAME_EXE) $(FLAGS) $(LIBS)

main.o: main.cpp GUI.hpp tree.hpp sprite_manager.hpp time_line.hpp
	g++ -c "main.cpp" -o main.o $(FLAGS)

GUI.o: GUI.cpp GUI.hpp
	g++ -c "GUI.cpp" -o GUI.o $(FLAGS)

tree.o: tree.cpp tree.hpp sprite_manager.hpp
	g++ -c "tree.cpp" -o tree.o $(FLAGS)

sprite_manager.o: sprite_manager.cpp sprite_manager.hpp
	g++ -c "sprite_manager.cpp" -o sprite_manager.o $(FLAGS)

time_line.o: time_line.cpp time_line.hpp
	g++ -c "time_line.cpp" -o time_line.o $(FLAGS)

clear:
	rm *.o
