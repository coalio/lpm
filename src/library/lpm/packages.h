#pragma once
#include <string>
#include <vector>
#include "toml_lib.h"

namespace LPM {
    class PackagesData {
    public:
        PackagesData(const std::string& path) {
            this->path = path;
            this->load();
        }

        std::string path;

        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string license;
        std::string homepage;
        std::string repository;
        std::string main;
        std::string lua_version;

        std::vector<std::string> dependencies;

        void load();
    };
}