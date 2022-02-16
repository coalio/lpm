#define TOML_IMPLEMENTATION
#include <string>
#include <vector>
#include <string_view>
#include "toml_lib.h"
#include "packages.h"
#include "macros.h"

using namespace std::string_view_literals;

void LPM::PackagesData::load() {
    toml::table toml_data;

    try {
        toml_data = toml::parse_file(this->path);
    } catch (...) {
        LPM_PRINT_ERROR("Failed to load TOML file: " << this->path);
    }

    this->name = toml_data["project.name"].value_or(""sv);
    this->version = toml_data["project.version"].value_or(""sv);
    this->description = toml_data["project.description"].value_or(""sv);
    this->author = toml_data["project.author"].value_or(""sv);
    this->license = toml_data["project.license"].value_or(""sv);
    this->homepage = toml_data["project.homepage"].value_or(""sv);
    this->repository = toml_data["project.repository"].value_or(""sv);
    this->main = toml_data["project.main"].value_or(""sv);
    this->lua_version = toml_data["project.lua_version"].value_or(""sv);
}