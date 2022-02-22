#pragma once
#include <cstdlib>
#include <string>

class Env {
public:
    static std::string get(std::string key, std::string default_value ) {
        const char* value = std::getenv(key.c_str());
        if (value == NULL) {
            return default_value;
        }

        return value;
    }

    static int get(std::string key, int default_value) {
        const char* value = std::getenv(key.c_str());

        if (value == NULL) {
            return default_value;
        }

        return std::atoi(value);
    }
};