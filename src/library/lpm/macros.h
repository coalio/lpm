#pragma once
#include <iostream>

#ifndef LPM_SHOULD_PRINT_ERRORS
    #define LPM_SHOULD_PRINT_ERRORS 1
#endif

#if LPM_SHOULD_PRINT_ERRORS
    #define LPM_PRINT_ERROR(msg) std::cerr << msg << std::endl
#else
    #define LPM_PRINT_ERROR(msg)
#endif

#define LPM_PRINT(msg) std::cout << msg << std::endl