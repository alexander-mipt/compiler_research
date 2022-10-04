#pragma once
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
#else
#define LOG(msg)                                                                                   \
    do {                                                                                           \
    } while (0)
#define ERROR(msg)                                                                                 \
    do {                                                                                           \
    } while (0)
#endif