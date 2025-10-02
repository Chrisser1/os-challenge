//
// Created by chris on 9/4/25.
//

#pragma once

#include <stdio.h>

#ifndef NDEBUG
    // The `__FILE__` and `__LINE__` are special macros that give the
    // current filename and line number. `##__VA_ARGS__` handles the
    // variable arguments of the printf format string.
    #define LOG_DEBUG(fmt, ...) \
    fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...)
#endif