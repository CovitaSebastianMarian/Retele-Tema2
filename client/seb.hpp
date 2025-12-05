#pragma once

#if __cplusplus >= 201703L

#include <optional>
#include <stdexcept>
template<typename T, typename E>
class Result {
public:
    static Result ok(T value) { return Result(std::move(value), std::nullopt, true); }
    static Result err(E error) { return Result(std::nullopt, std::move(error), false); }

    bool is_ok() const { return is_value_ok; }
    bool is_err() const { return !is_value_ok; }

    T unwrap() const {
        if (!is_value_ok) throw std::runtime_error("Called unwrap on Err");
        return *value;
    }
    E unwrap_err() const {
        if (is_value_ok) throw std::runtime_error("Called unwrap_err on Ok");
        return *error;
    }

private:
    Result(std::optional<T> v, std::optional<E> e, bool ok) : value(std::move(v)), error(std::move(e)), is_value_ok(ok) {}
    bool is_value_ok;
    std::optional<T> value;
    std::optional<E> error;
};

std::string log_buffer;

// #define log_error(x) \
//     do { \
//         printf("\033[31m[EROARE]\nFisier:%s\nLinia:%d\nMesaj:%s\033[0m\n", __FILE__, __LINE__, x); \
//     } while(0)

// #define log_message(fmt, ...) \
//     do { \
//         printf("\033[34m[SERVER LOG]\n"); \
//         printf(fmt, __VA_ARGS__); \
//         printf("\033[0m\n"); \
//     } while(0)

#define log_error(x) \
    do { \
        std::ostringstream oss; \
        oss << "[EROARE]\nFisier:" << __FILE__ << "\nLinia:" << __LINE__ << "\nMesaj:" << x << "\n"; \
        log_buffer += oss.str(); \
        printf("\033[31m%s\033[0m\n", oss.str().c_str()); \
    } while(0)

#define log_message(fmt, ...) \
    do { \
        char buffer[1024]; \
        snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__); \
        std::ostringstream oss; \
        oss << "[SERVER LOG]\n" << buffer << "\n"; \
        log_buffer += oss.str(); \
        printf("\033[34m%s\033[0m\n", oss.str().c_str()); \
    } while(0)



#else
#error "C++17 sau mai nou este necesar"
#endif
