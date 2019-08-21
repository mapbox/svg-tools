#pragma once

#include <iostream>
#include <cstdlib>

static unsigned long FAILED_EXPECTATIONS = 0;
static unsigned long SUCCEEDED_EXPECTATIONS = 0;

static const char* EXPECT_RED = "\033[1;31m";
static const char* EXPECT_CLEAR = "\033[0m";

static const int EXPECT_SUMMARY = ([] {
    std::atexit([] {
        std::cerr << std::endl
                  << SUCCEEDED_EXPECTATIONS << " expectation"
                  << (SUCCEEDED_EXPECTATIONS != 1 ? "s" : "") << " succeeded" << std::endl
                  << (FAILED_EXPECTATIONS ? EXPECT_RED : "") << FAILED_EXPECTATIONS
                  << " expectation" << (FAILED_EXPECTATIONS != 1 ? "s" : "") << " failed"
                  << EXPECT_CLEAR << std::endl;
        if (FAILED_EXPECTATIONS > 0) {
            exit(1);
        }
    });
    return 0;
})();

#define EXPECT(cond, expression)                                                                   \
    do {                                                                                           \
        if (cond(expression)) {                                                                    \
            FAILED_EXPECTATIONS++;                                                                 \
            std::cerr << std::endl                                                                 \
                      << EXPECT_RED << "Expectation failed in " << __FILE__ << " on line "         \
                      << __LINE__ << EXPECT_CLEAR << std::endl;                                    \
        } else {                                                                                   \
            SUCCEEDED_EXPECTATIONS++;                                                              \
            std::cerr << ".";                                                                      \
        }                                                                                          \
    } while (false);

#define EXPECT_FALSE(expression) EXPECT(, expression)
#define EXPECT_TRUE(expression) EXPECT(!, expression)

#define EXPECT_EQUALS(expected, actual)                                                            \
    do {                                                                                           \
        const auto expected__##__LINE__ = (expected);                                              \
        const auto actual__##__LINE__ = (actual);                                                  \
        if (!(expected__##__LINE__ == actual__##__LINE__)) {                                       \
            FAILED_EXPECTATIONS++;                                                                 \
            std::cerr << std::endl                                                                 \
                      << EXPECT_RED << "Expectation failed in " << __FILE__ << " on line "         \
                      << __LINE__ << ":" << EXPECT_CLEAR << std::endl;                             \
            std::cerr << "Expected value:" << std::endl;                                           \
            std::cerr << expected__##__LINE__ << std::endl;                                        \
            std::cerr << "bot got this value instead:" << std::endl;                               \
            std::cerr << actual__##__LINE__ << std::endl;                                          \
        } else {                                                                                   \
            SUCCEEDED_EXPECTATIONS++;                                                              \
            std::cerr << ".";                                                                      \
        }                                                                                          \
    } while (false)
