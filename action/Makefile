OBJS = main.cpp
# FLAGS = -g -D_WIN -std=c++20 -lSDL2main -lSDL2 -lSDL2_image -lShell32 -lWs2_32 -Xlinker /subsystem:console
FLAGS = -g -D_WIN -std=c++20
INC = -I./../core -I./../net -I./../action

CC = clang++
BLDPATH = ./build

.PHONY: client server

client:
	$(CC) ./client/$(OBJS) $(INC) $(LIB) $(FLAGS) -o $(BLDPATH)/client.exe

server:
	$(CC) ./server/$(OBJS) $(INC) $(LIB) $(FLAGS) -o $(BLDPATH)/server.exe

all:
	client server
