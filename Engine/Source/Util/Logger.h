#pragma once

#define MON_LOG(message) std::cout << (message) << std::endl
#define MON_WARN(message) std::cerr << "Warning: " << (message) << std::endl
#define MON_ASSERT(condition, message) \
    if (!(condition)) { \
        std::cerr << "Assertion failed: " << (message) << std::endl; \
        assert(condition); \
    }
