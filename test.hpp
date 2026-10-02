#include <iostream>
#include <cstdlib>
#include <cmath>

#pragma once

inline int g_TestCount = 0;
inline int g_TestErrors = 0;

#ifdef _DEBUG
    #define TEST_ENABLED 1
#else
    #define TEST_ENABLED 0
#endif

#define TEST_ABORT_ON_FAIL 1
#define PRINT_ONLY_FAIL 0

#pragma once

#if TEST_ENABLED == 1
    #if PRINT_ONLY_FAIL == 1
        #define TEST(name) void name(); static bool _##name = (++g_TestCount, name(), true);; void name()
    #else
        #define TEST(name) void name(); static bool _##name = (++g_TestCount, std::cout << "TEST: " << #name << ": ", name(), std::cout << "OK\n", true); void name()
    #endif
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
                      << #condition << std::endl \
            << "[" << __FILE__ << ":" << __LINE__ << "]" \
            << std::endl; \
            std::abort(); \
        } \
    } while (0)
#endif

#if TEST_ABORT_ON_FAIL == 0
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            ++g_TestErrors; \
            std::cerr << "FAILED: " << #condition << std::endl \
            << "[" << __FILE__ << ":" << __LINE__ << "] " \
            << "ERROR NOT"; \
        } \
    } while (0)
#endif

//CHECK_NEAR(line.x1, 10.5f, 0.001f);
#define CHECK_NEAR(a, b, eps) CHECK(std::fabs((a) - (b)) <= (eps))
#define CHECK_THROW(code) do { bool thrown = false; try { code; } catch (...) { thrown = true; } CHECK(thrown); } while (0)
//CHECK_NOT_THROW(MojaFunkcja());
#define CHECK_NOT_THROW(code) do { bool thrown = false; try { code; } catch (...) { thrown = true; } CHECK(!thrown); } while (0)

inline void PrintTestResults()
{
    std::cout << "\n";
    std::cout << "====================\n";
    std::cout << "Tests:  " << g_TestCount << "\n";
    std::cout << "Errors: " << g_TestErrors << "\n";
    std::cout << "====================\n";
}

struct TestResultsRunner
{
	~TestResultsRunner()
	{
		PrintTestResults();
	}
};

inline TestResultsRunner g_TestResultsRunner;