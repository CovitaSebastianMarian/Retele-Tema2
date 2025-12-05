#pragma once
#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <queue>
#include "message.hpp"
#include "seb.hpp"
#include "client.hpp"



class Server;

struct ThreadContext {
    Server* server;
    int32_t id_thread;
    int32_t client;
};

struct ThreadWorker {
    pthread_t id;
};

class Server {
private:
    int32_t sd, port;
    struct sockaddr_in server;
    std::vector<ThreadWorker> threads;
    std::vector<Client> clients;
    pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
    bool active = false;
public:
    Server(int32_t port) : port(port) {
        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            log_error("[server]Eroare la socket()");
            exit(EXIT_FAILURE);
        }

        int32_t on = 1;
        setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(port);

        if (bind(sd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1) {
            log_error("Eroare la bind().\n");
            exit(EXIT_FAILURE);
        }

        if (listen(sd, 5) == -1) {
            log_error("Eroare la listen()");
            exit(EXIT_FAILURE);
        }
    }
    ~Server() {
        close(sd);
        for (auto& tw : threads) {
            pthread_cancel(tw.id);
            pthread_join(tw.id, NULL);
        }
    }
    static void* thread_function(void* arg) {
        ThreadContext* tc = (ThreadContext*)arg;
        Server* server = tc->server;
        int32_t id_thread = tc->id_thread;

        Client client;
        client.bind_connection(tc->client);

        delete tc;

        pthread_mutex_lock(&server->clients_mutex);
        server->clients.push_back(client);
        pthread_mutex_unlock(&server->clients_mutex);

        while (true) {
            auto r = server->handle_client(client, id_thread);
            if (r.is_err()) {
                log_error(r.unwrap_err().c_str());
                break;
            }
        }

        client.close_connection();

        pthread_mutex_lock(&server->clients_mutex);
        for (auto it = server->clients.begin(); it != server->clients.end(); ++it) {
            if (it->cl == client.cl) {
                server->clients.erase(it);
                break;
            }
        }
        pthread_mutex_unlock(&server->clients_mutex);

        return nullptr;
    }
    Result<int, std::string> handle_client(Client& client, int32_t id_thread) {
        typedef Result<int, std::string> Res;
        auto r1 = client.read_full();
        if (r1.is_err()) {
            return Res::err(r1.unwrap_err());
        }

        auto r2 = response_message(client, r1.unwrap());
        if (r2.is_err()) {
            return Res::err(r2.unwrap_err());
        }

        auto r3 = client.write_full(r2.unwrap());
        if (r3.is_err()) {
            return Res::err(r3.unwrap_err());
        }

        log_message("Trimis: %s", r2.unwrap().c_str());

        return Res::ok(1);
    }
    Result<std::string, std::string> response_message(const Client& client, std::string msg) {
        typedef Result<std::string, std::string> Res;
        const std::string response = Message::decode(msg);
        return Res::ok(response);
    }
    Result<int32_t, std::string> accept_client() {
        struct sockaddr_in from;
        socklen_t length = sizeof(from);
        int32_t cl;

        cl = accept(sd, (struct sockaddr*)&from, &length);

        if (cl < 0) {
            return Result<int32_t, std::string>::err("Eroare la accept_client()!");
        }
        return Result<int32_t, std::string>::ok(cl);
    }
    void run() {
        log_message("Serverul ruleaza pe portul: %d", port);
        while (true) {
            auto r1 = accept_client();
            if (r1.is_err()) {
                log_error(r1.unwrap_err().c_str());
            }

            ThreadContext* tc = new ThreadContext;
            tc->id_thread = threads.size();
            tc->server = this;
            tc->client = r1.unwrap();

            ThreadWorker tw;
            pthread_create(&tw.id, NULL, &Server::thread_function, (void*)tc);
            threads.push_back(tw);
        }
    }
};