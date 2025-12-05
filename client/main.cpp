#include "client.hpp"
#include "message.hpp"
#include "app.hpp"


int main(int argc, char** argv) {
    // if(argc != 3) {
    //     printf("Introdu ip si porul\n");
    //     return -1;
    // }
    // Client client;
    // int port = atoi(argv[2]);
    // auto r = client.bind_connection(argv[1], port);
    // if(r.is_err()) {
    //     log_error(r.unwrap_err().c_str());
    //     return -1;
    // }

    // auto r1 = client.write_full(Message::Accident("Strada Lapusneanu Nr 12"));
    // if(r1.is_err()) {
    //     log_error(r1.unwrap_err().c_str());
    //     return -1;
    // }

    // auto r2 = client.read_full();
    // if(r2.is_err()) {
    //     log_error(r2.unwrap_err().c_str());
    //     return -1;
    // }
    // printf("Mesaj primit: %s\n", r2.unwrap().c_str());

    // client.close_connection();
    if (argc != 3) {
        printf("Introdu ip si porul\n");
        return -1;
    }
    App app;
    auto r = app.bind(argv[1], argv[2]);
    if(r.is_err()) {
        printf("%s\n", r.unwrap_err().c_str());
        return -1;
    }
    log_message("%s", "Conectat cu succes!");
    app.create(1000, 800, "Window");
    app.run();

    return 0;
}