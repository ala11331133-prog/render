#include <iostream>
#include <cstdlib>
#include <cmath>

#define TEST_ENABLED 1
#define TEST_ABORT_ON_FAIL 1

extern int g_TestCount;
extern int g_TestErrors;

#pragma once

#if TEST_ENABLED == 1
#define TEST(name) \
    void name(); \
    static bool _##name = (std::cout << "TEST: " << #name << ": ", name(), std::cout << "OK\n", true); \
    void name()
#else

#define TEST(name) void name(); void name()

#endif
//CHECK(x > 0);
//CHECK(ptr != nullptr);
//CHECK(sizeof(TLine) == 22); 

#if TEST_ABORT_ON_FAIL == 1
#define CHECK(condition) do { \
        if (!(condition)) { \
            std::cerr << "TEST FAILED: " \
                      << #condition << std::endl; \
            std::abort(); \
        } \
    } while (0)
#endif

#if TEST_ABORT_ON_FAIL == 0
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            ++g_TestErrors; \
            std::cerr << "FAILED: " << #condition << std::endl; \
        } \
    } while (0)
#endif

//CHECK_NEAR(line.x1, 10.5f, 0.001f);
#define CHECK_NEAR(a, b, eps) CHECK(std::fabs((a) - (b)) <= (eps))
#define CHECK_THROW(code) do { bool thrown = false; try { code; } catch (...) { thrown = true; } CHECK(thrown); } while (0)
//CHECK_NOT_THROW(MojaFunkcja());
#define CHECK_NOT_THROW(code) do { bool thrown = false; try { code; } catch (...) { thrown = true; } CHECK(!thrown); } while (0)

void PrintTestResults();
