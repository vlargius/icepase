FLAGS = -g -std=c++20
INC = -I./../net
CC = clang++
BLDPATH = ./build


client:
	$(CC) ./client.cpp $(INC) $(FLAGS) -D_LIN -o $(BLDPATH)/client

server:
	$(CC) ./server.cpp $(INC) $(FLAGS) -D_LIN -o $(BLDPATH)/server

clean:
	rm -rf $(BLDPATH)/*
