CLIENT_OBJS = main.cpp
SERVER_OBJS = main.cpp server/penguin.cpp server/replication_transmission.cpp
INC = -I./../core -I./../net -I./../action
FLAGS = -D_WIN -std=c++20 -g

RENDER_INC = -I./../lib/SDL2/include -L./../lib/SDL2/lib/x86
RENDER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lShell32 -lWs2_32 -Xlinker /subsystem:console

CC = clang++
BLDPATH = ./build

.PHONY: client server

client:
	$(CC) ./client/$(CLIENT_OBJS) $(INC) $(RENDER_INC) $(LIB) $(RENDER_FLAGS) $(FLAGS) -o $(BLDPATH)/client.exe

server:
	$(CC) ./server/$(SERVER_OBJS) $(INC) $(LIB) $(FLAGS) -o $(BLDPATH)/server.exe

all:
	client server
