#pragma once
#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <string>
#include <optional>
#include <sstream>
#include "seb.hpp"



class Client {
public:
    int32_t cl;
    struct sockaddr_in server;
    int port;
    std::string ip_server;

    Result<bool, std::string> bind_connection(const char* adress, int p) {
        ip_server = adress;
        port = p;
        if ((cl = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            return Result<bool, std::string>::err("Eroare la socket()");
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(adress);
        server.sin_port = htons(port);

        if (connect(cl, (struct sockaddr*)&server, sizeof(server)) == -1) {
            return Result<bool, std::string>::err("Eroare la connect()");
        }

        return Result<bool, std::string>::ok(true);
    }
    void close_connection() {
        auto a = write_full("[EXIT]");
        close(cl);
    }

    Result<uint32_t, std::string> read_len() {
        typedef Result<uint32_t, std::string> Res;
        uint32_t len = 0;
        uint32_t len_bytes_received = 0;

        while (len_bytes_received < 4) {
            uint32_t needed = 4 - len_bytes_received;
            unsigned char* len_ptr = ((unsigned char*)&len) + len_bytes_received;

            ssize_t n = read(cl, len_ptr, needed);

            if (n > 0) {
                len_bytes_received += n;
                continue;
            }

            if (n == 0) {
                return Res::err("Conexiunea s-a intrerupt!");
            }

            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    usleep(100);
                    continue;
                }
                return Res::err(std::string("Read error: ") + strerror(errno));
            }
        }

        len = ntohl(len);
        return Res::ok(len);
    }

    Result<std::string, std::string> read_message(uint32_t len) {
        typedef Result<std::string, std::string> Res;

        std::string message;
        message.resize(len);
        uint32_t message_bytes_received = 0;

        while (message_bytes_received < len) {
            uint32_t needed = len - message_bytes_received;
            char* ptr = message.data() + message_bytes_received;

            ssize_t n = read(cl, ptr, needed);

            if (n > 0) {
                message_bytes_received += n;
                continue;
            }

            if (n == 0) {
                return Res::err("Conexiunea s-a intrerupt!");
            }

            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    usleep(100);
                    continue;
                }

                return Res::err(std::string("Read error: ") + strerror(errno));
            }
        }

        return Res::ok(message);
    }

    Result<int, std::string> write_len(uint32_t len) {
        typedef Result<int, std::string> Res;

        uint32_t net_len = htonl(len);
        uint32_t bytes_sent = 0;

        while (bytes_sent < 4) {
            uint32_t needed = 4 - bytes_sent;
            unsigned char* ptr = ((unsigned char*)&net_len) + bytes_sent;

            ssize_t n = write(cl, ptr, needed);

            if (n > 0) {
                bytes_sent += n;
                continue;
            }

            if (n == 0) {
                return Res::err("Conexiunea s-a intrerupt!");
            }

            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    usleep(100);
                    continue;
                }
                return Res::err(std::string("Write error: ") + strerror(errno));
            }
        }

        return Res::ok(1);
    }

    Result<int, std::string> write_message(const std::string& msg) {
        typedef Result<int, std::string> Res;

        uint32_t len = msg.size();
        uint32_t bytes_sent = 0;

        while (bytes_sent < len) {
            uint32_t needed = len - bytes_sent;
            const char* ptr = msg.data() + bytes_sent;

            ssize_t n = write(cl, ptr, needed);

            if (n > 0) {
                bytes_sent += n;
                continue;
            }

            if (n == 0) {
                return Res::err("Conexiunea s-a intrerupt!");
            }

            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    usleep(100);
                    continue;
                }
                return Res::err(std::string("Write error: ") + strerror(errno));
            }
        }

        return Res::ok(1);
    }

    Result<int, std::string> write_full(const std::string msg) {
        typedef Result<int, std::string> Res;
        auto r1 = write_len(msg.length());
        if (r1.is_err()) {
            return Res::err(r1.unwrap_err());
        }
        auto r2 = write_message(msg);
        if (r2.is_err()) {
            return Res::err(r2.unwrap_err());
        }
        return Res::ok(1);
    }
    Result<std::string, std::string> read_full() {
        typedef Result<std::string, std::string> Res;
        auto r1 = read_len();
        if (r1.is_err()) {
            return Res::err(r1.unwrap_err());
        }
        auto r2 = read_message(r1.unwrap());
        if (r2.is_err()) {
            return Res::err(r2.unwrap_err());
        }
        return Res::ok(r2.unwrap());
    }
};