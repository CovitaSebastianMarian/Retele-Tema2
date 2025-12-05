#include "server.hpp"

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Introdu portul!\n");
        return -1;
    }
    int port = atoi(argv[1]);
    Server server(port);
    server.run();
    return 0;
}
