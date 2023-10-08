OBJS = main.cpp
# FLAGS = -g -D_WIN -std=c++20 -lSDL2main -lSDL2 -lSDL2_image -lShell32 -lWs2_32 -Xlinker /subsystem:console
FLAGS = -g -D_WIN -std=c++20
INC = -I.\lib\SDL2\include -I.\core -I.\net
LIB = -L.\lib\SDL2\lib\x86
# INC = -I.\lib\SDL2\include -I.\core -I.\net
# LIB = -L.\lib\SDL2\lib\x86
CC = clang++
BLDPATH = .\build

.PHONY: client server visual

visual:
	$(CC) ./client/visual.cpp $(INC) $(FLAGS) -D_LIN  -o $(BLDPATH)/visual

client:
	$(CC) client.cpp $(INC) $(LIB) $(FLAGS) -o $(BLDPATH)\client.exe

server:
	$(CC) server.cpp $(INC) $(LIB) $(FLAGS) -o $(BLDPATH)\server.exe

clean: clean_test

all: make
