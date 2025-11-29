#pragma once

#define DEBUG true
#define DEBUG_LOG_LEVEL 1

#if DEBUG && DEBUG_LOG_LEVEL >= 1
    #include <iostream>
    #define DEBUG_LOG(msg) std::cerr << "[DEBUG] " << msg << std::endl;
#else
    #define DEBUG_LOG(msg)
#endif
    