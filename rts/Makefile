OBJS = main.cpp
INC = -I./../core -I./../net -I./../action
FLAGS = -D_WIN -std=c++20 -g

RENDER_INC = -I./../lib/SDL2/include -L./../lib/SDL2/lib/x86
RENDER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lShell32 -lWs2_32 -Xlinker /subsystem:console

CC = clang++
BLDPATH = ./build

.PHONY: client server

all:
# $(CC) $(OBJS) $(INC) $(RENDER_INC) $(LIB) $(RENDER_FLAGS) $(FLAGS) -o $(BLDPATH)/client.exe
	$(CC) $(OBJS) $(INC) $(FLAGS) -o $(BLDPATH)/client.exe