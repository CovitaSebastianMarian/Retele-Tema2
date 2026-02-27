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
#include "seb.hpp"
#include "client.hpp"
#include <map>


class Server;

struct ThreadContext {
    Server* server;
    int32_t id_thread;
    int32_t client;
};

struct ThreadWorker {
    pthread_t id;
};

struct StraziInfo {
    int viteza_max;
    float pret_benzina, pret_motorina;
    std::string vreme;
    int accident_timer, trafic_timer;
};

pthread_mutex_t strazi_mutex = PTHREAD_MUTEX_INITIALIZER;
std::map<std::string, StraziInfo> strazi = {
    // Zona Centru
    {"Str. Palat", {30, 7.30f, 7.40f, "Se anunta cer variabil", 0, 0}},
    {"Str. Palatul Culturii", {20, 7.30f, 7.40f, "Se anunta cer variabil", 0, 0}},
    {"Str. Sf. Lazar", {30, 7.30f, 7.40f, "Se anunta cer variabil", 0, 0}},
    {"Str. Mitropoliei", {20, 7.30f, 7.40f, "Se anunta cer variabil", 0, 0}},
    {"Str. Alexandru Lăpușneanu", {40, 7.30f, 7.40f, "Se anunta cer variabil", 0, 0}},

    // Zona Copou
    {"Str. Copou", {30, 7.20f, 7.30f, "Vreme buna, partial insorit", 0, 0}},
    {"Str. Vasile Lupu", {30, 7.20f, 7.30f, "Vreme buna, partial insorit", 0, 0}},
    {"Str. Sf. Petru Movilă", {30, 7.20f, 7.30f, "Vreme buna, partial insorit", 0, 0}},

    // Zona Tatarasi
    {"Str. Tătărași", {30, 7.25f, 7.35f, "Posibile ploi slabe", 0, 0}},
    {"Str. Tudor Vladimirescu", {30, 7.25f, 7.35f, "Posibile ploi slabe", 0, 0}},
    {"Str. Horea", {30, 7.25f, 7.35f, "Posibile ploi slabe", 0, 0}},

    // Zona Nicolina
    {"Str. Nicolina", {30, 7.40f, 7.50f, "Cer innorat", 0, 0}},
    {"Str. Canta", {30, 7.40f, 7.50f, "Cer innorat", 0, 0}},
    {"Str. Arcu", {30, 7.40f, 7.50f, "Cer innorat", 0, 0}},

    // Zona Pacurari
    {"Str. Păcurari", {40, 7.15f, 7.25f, "Insorit, vizibilitate buna", 0, 0}},
    {"Str. Moara de Vânt", {30, 7.15f, 7.25f, "Insorit, vizibilitate buna", 0, 0}},

    // Alte strazi
    {"Str. Cuza Vodă", {30, 7.35f, 7.45f, "Cer variabil", 0, 0}},
    {"Str. Ștefan cel Mare", {40, 7.35f, 7.45f, "Cer variabil", 0, 0}},
    {"Str. Anastasie Panu", {30, 7.35f, 7.45f, "Cer variabil", 0, 0}},
    {"Str. Independenței", {30, 7.35f, 7.45f, "Cer variabil", 0, 0}}
};



class Server {
private:
    int32_t sd, port;
    struct sockaddr_in server;
    std::vector<ThreadWorker> threads;
    std::vector<Client*> clients;
    pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

public:
    Server(int32_t port) : port(port) {
        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            log_error("Eroare la socket()");
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
    static void* thread_function_anunturi(void* arg) {
        ThreadContext* tc = (ThreadContext*)arg;
        Server* server = tc->server;
        int32_t id_thread = tc->id_thread;

        delete tc;

        bool run = true;
        int cnt = 0;

        while (run) {
            pthread_mutex_lock(&strazi_mutex);
            for (auto& s : strazi) {
                if (s.second.accident_timer != 0) {
                    s.second.accident_timer -= 1;
                }
                if (s.second.trafic_timer != 0) {
                    s.second.trafic_timer -= 1;
                }
            }
            pthread_mutex_unlock(&strazi_mutex);

            for (auto c : server->clients) {
                switch (cnt) {
                case 0:
                    if (c->abonare_vreme) {
                        std::string msg = "[VREME] Nu avem date!";
                        auto it = strazi.find(c->strada);
                        if (it != strazi.end()) {
                            msg = "[VREME]" + it->second.vreme;
                        }

                        auto r = c->write_full(msg);
                        if (r.is_err()) {
                            log_error(r.unwrap_err().c_str());
                            continue;
                        }
                    }
                    break;
                case 1:
                    if (c->abonare_sport) {
                        auto r = c->write_full("[SPORT] FC Barcelona a invins-o pe Real Madrid cu 3-2.");
                        if (r.is_err()) {
                            log_error(r.unwrap_err().c_str());
                            continue;
                        }
                    }
                    break;
                case 2:
                    if (c->abonare_combustibil) {
                        std::string msg = "[COMBUSTIBIL] Nu avem date despre benzinariile din jurul tau!";
                        auto it = strazi.find(c->strada);
                        if (it != strazi.end()) {
                            msg = it->second.vreme;
                            msg = "[COMBUSTIBIL] Pret benzina: " + std::to_string(it->second.pret_benzina) + ", pret motorina: " + std::to_string(it->second.pret_motorina) + ".";
                        }

                        auto r = c->write_full(msg);
                        if (r.is_err()) {
                            log_error(r.unwrap_err().c_str());
                            continue;
                        }
                    }
                    break;
                }
            }

            cnt++;
            cnt %= 3;

            sleep(10);
        }

        return nullptr;
    }

    static void* thread_function(void* arg) {
        ThreadContext* tc = (ThreadContext*)arg;
        Server* server = tc->server;
        int32_t id_thread = tc->id_thread;

        Client client;
        client.bind_connection(tc->client);

        delete tc;

        pthread_mutex_lock(&server->clients_mutex);
        server->clients.push_back(&client);
        pthread_mutex_unlock(&server->clients_mutex);

        bool run = true;
        auto r4 = client.write_full("[SYSTEM] Conectat cu succes!");
        if (r4.is_err()) {
            run = false;
        }

        for (const auto& a : strazi) {
            if (a.second.accident_timer != 0) {
                auto r = client.write_full("[ACCIDENT] " + a.first);
                if (r.is_err()) {
                    run = false;
                    break;
                }
            }
        }
        for (const auto& a : strazi) {
            if (a.second.trafic_timer != 0) {
                auto r = client.write_full("[TRAFIC] " + a.first);
                if (r.is_err()) {
                    run = false;
                    break;
                }
            }
        }


        while (run) {
            auto r = server->handle_client(server, client, id_thread);
            if (r.is_err()) {
                log_error(r.unwrap_err().c_str());
                break;
            }
            else if(r.is_ok()) {
                if(r.unwrap() == -1) {
                    log_message("Client deconectat! %d", id_thread);
                    break;
                }
            }
        }

        client.close_connection();

        pthread_mutex_lock(&server->clients_mutex);
        for (auto it = server->clients.begin(); it != server->clients.end(); ++it) {
            if ((*it)->cl == client.cl) {
                server->clients.erase(it);
                break;
            }
        }
        pthread_mutex_unlock(&server->clients_mutex);

        return nullptr;
    }
    static Result<int, std::string> handle_client(Server* server, Client& client, int32_t id_thread) {
        typedef Result<int, std::string> Res;
        auto r1 = client.read_full();
        if (r1.is_err()) {
            return Res::err(r1.unwrap_err());
        }

        std::string response = handle_message(server, client, r1.unwrap());
        if(response == "[EXIT]") {
            return Res::ok(-1);
        }
        if (response != "") {
            auto r4 = client.write_full(response);
            if (r4.is_err()) {
                return Res::err(r4.unwrap_err());
            }
            log_message("Trimis: %s", response.c_str());
        }

        return Res::ok(1);
    }
    static std::string handle_message(Server* server, Client& client, std::string msg) {
        if (msg.rfind("[VITEZA] ", 0) == 0) {
            std::string sv = msg.substr(9);
            client.viteza = atoi(sv.c_str());
        
            auto it = strazi.find(client.strada);
            if (it == strazi.end())
                return "";
        
            const StraziInfo& info = it->second;
        
            if (info.accident_timer > 0) {
                return "[ACCIDENT] Circulatia este restrictionata din cauza unui accident pe strada " + client.strada;
            }
        
            if (info.trafic_timer > 0) {
                return "[TRAFIC] Trafic intens pe strada " + client.strada + ". Reduceti viteza!";
            }
        
            if (client.viteza > info.viteza_max) {
                return "[ATENTIE] Viteza maxima este: " + std::to_string(info.viteza_max);
            }
        
            return "";
        }
        else if (msg.rfind("[STRADA] ", 0) == 0) {
            std::string sv = msg.substr(9);
            client.strada = sv;
        }
        else if (msg.rfind("[ACCIDENT] ", 0) == 0) {
            pthread_mutex_lock(&strazi_mutex);
            auto it = strazi.find(msg.substr(11));
            if (it != strazi.end()) {
                it->second.accident_timer = 3;
            }
            pthread_mutex_unlock(&strazi_mutex);


            std::string response = msg;
            pthread_mutex_lock(&server->clients_mutex);
            for (Client* c : server->clients) {
                if (c->cl != client.cl) {
                    auto r3 = c->write_full(response);
                    if (r3.is_err()) {
                        continue;
                    }
                }
            }
            pthread_mutex_unlock(&server->clients_mutex);
            log_message("Trimis la toti clientii: %s", response.c_str());

            return "[SYSTEM] Accident receptionat pe strada " + msg.substr(11);
        }
        else if (msg.rfind("[TRAFIC] ", 0) == 0) {
            pthread_mutex_lock(&strazi_mutex);
            auto it = strazi.find(msg.substr(9));
            if (it != strazi.end()) {
                it->second.trafic_timer = 3;
            }
            pthread_mutex_unlock(&strazi_mutex);

            std::string response = msg;
            pthread_mutex_lock(&server->clients_mutex);
            for (Client* c : server->clients) {
                if (c->cl != client.cl) {
                    auto r3 = c->write_full(response);
                    if (r3.is_err()) {
                        continue;
                    }
                }
            }
            pthread_mutex_unlock(&server->clients_mutex);
            log_message("Trimis la toti clientii: %s", response.c_str());

            return "[SYSTEM] Trafic receptionat pe strada " + msg.substr(9);
        }
        else if (msg.rfind("[ABONARE_VREME]", 0) == 0) {
            client.abonare_vreme = true;
            return "[SYSTEM] Te-am abonat la vreme!";
        }
        else if (msg.rfind("[DEZABONARE_VREME]", 0) == 0) {
            client.abonare_vreme = false;
            return "[SYSTEM] Te-am dezabonat la vreme!";
        }
        else if (msg.rfind("[ABONARE_SPORT]", 0) == 0) {
            client.abonare_sport = true;
            return "[SYSTEM] Te-am abonat la sport!";
        }
        else if (msg.rfind("[DEZABONARE_SPORT]", 0) == 0) {
            client.abonare_sport = false;
            return "[SYSTEM] Te-am dezabonat la sport!";
        }
        else if (msg.rfind("[ABONARE_COMBUSTIBIL]", 0) == 0) {
            client.abonare_combustibil = true;
            return "[SYSTEM] Te-am abonat la combustibil!";
        }
        else if (msg.rfind("[DEZABONARE_COMBUSTIBIL]", 0) == 0) {
            client.abonare_combustibil = false;
            return "[SYSTEM] Te-am dezabonat la combustibil!";
        }
        else if (msg.rfind("[CHAT] ", 0) == 0) {
            std::string response = msg;
            pthread_mutex_lock(&server->clients_mutex);
            for (Client* c : server->clients) {
                if (c->cl != client.cl) {
                    auto r3 = c->write_full(response);
                    if (r3.is_err()) {
                        continue;
                    }
                }
            }
            pthread_mutex_unlock(&server->clients_mutex);
            log_message("Trimis la toti clientii: %s", response.c_str());
            return "[TU] " + msg.substr(7);
        }
        else if (msg.rfind("[EXIT]", 0) == 0) {
            return "[EXIT]";
        }

        return "";
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


        ThreadContext* tc = new ThreadContext;
        tc->id_thread = threads.size();
        tc->server = this;
        tc->client = 0;

        ThreadWorker tw;
        pthread_create(&tw.id, NULL, &Server::thread_function_anunturi, (void*)tc);
        threads.push_back(tw);


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