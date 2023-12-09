#include "client.h"

int main (int argc, char* argv[]) {
    Client client;
    if (client.init())
        return client.run();

    return 0;
}