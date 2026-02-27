#include "client.hpp"
#include "message.hpp"
#include "app.hpp"


int main(int argc, char** argv) {
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
    app.create(800, 400, "Window");
    app.run();

    return 0;
}