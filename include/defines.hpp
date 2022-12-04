#pragma once
#include <cassert>
#include <iostream>

#define $(x) << x
#define $n(x) << x << "\n"
#define $int(x) << std::dec << #x << ": " << static_cast<long>(x)
#define $hex(x) << std::hex << #x << ": " << static_cast<long>(x)
#define $ptr(x) << std::hex << #x << ": " << x
#define $intn(x) $int(x) << "\n"
#define $hexn(x) $hex(x) << "\n"
#define $ptrn(x) $ptr(x) << "\n"

#define ASSERT_DEV(condition, out) do {
std::cerr << "Error: " << out << "\n";
#ifdef NDEBUG
#define NDEBUG 1
assert(condition);
#undef NDEBUG
#else
assert(condition);
#endif } while (0)