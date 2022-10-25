#pragma once
#include <cstdio>
#include <iostream>
#ifndef NDEBUG
#define LOG(msg)                                                                                   \
    do {                                                                                           \
        std::cerr << "[LOG] " << #msg << "\n";                                                     \
    } while (0)
#define ERROR(msg)                                                                                 \
    do {                                                                                           \
        std::cerr << "[ERROR] " << #msg << "\n";                                                   \
        /* assert(false); */                                                                       \
    } while (0)
#define eprintf(...)                                                                               \
    do {                                                                                           \
        fprintf(stderr, __VA_ARGS__);                                                              \
    } while (0)
#define $(x)                                                                                       \
    do {                                                                                           \
        std::cerr << #x << ": " << static_cast<int>(x) << "\n";                                                      \
    } while (0)
#define $hex(x)                                                                                    \
    do {                                                                                           \
        std::cerr << #x << std::hex << x << "\n";                                                  \
    } while (0)
#else
#define EMPTY_MACRO                                                                                \
    do {                                                                                           \
    } while (0)
#define LOG(msg) EMPTY_MACRO
#define ERROR(msg) EMPTY_MACRO
#define eprintf(...) EMPTY_MACRO
#define $(x) EMPTY_MACRO
#define $hex(x) EMPTY_MACRO
#endif