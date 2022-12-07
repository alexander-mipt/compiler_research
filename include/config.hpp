#pragma once
#include <cassert>
#include <cstdio>
#include <iostream>

#ifndef NDEBUG
#define OPT(expr) expr
#else
#define OPT(expr)
#endif

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

#define $(x) << x
#define $n(x) << x << "\n"
#define $int(x) << std::dec << #x << ": " << static_cast<long>(x)
#define $hex(x) << std::hex << #x << ": " << static_cast<long>(x)
#define $ptr(x) << std::hex << #x << ": " << x
#define $intn(x) $int(x) << "\n"
#define $hexn(x) $hex(x) << "\n"
#define $ptrn(x) $ptr(x) << "\n"

#define ASSERT_DEV(condition, out)                                                                 \
    do {                                                                                           \
        std::cerr << "Error: " << out << "\n";                                                     \
        assert(condition);                                                                         \
    } while (0)
    