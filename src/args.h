#pragma once
#include "lpm/types.h"

args_map_t const LPM_VALID_ARGS {
    {"audit", {
        "recursive"
    }},
    {"configure", {}},
    {"help", {}},
    {"init", {
        "name",
        "version",
        "description",
        "author",
        "license",
        "homepage",
        "repository",
        "main",
        "lua-version",
    }},
    {"install", {
        "package_name",
        "global",
        "version",
        "lua-version",
        "recursive"
    }},
    {"list", {
        "global"
    }},
    {"purge", {
        "recursive"
    }},
    {"remove", {
        "global",
        "version",
        "lua-version"
    }},
    {"repository", {
        "action"
    }},
    {"run", {
        "package_name",
        "lua-version"
    }},
    {"search", {
        "package_name"
    }},
    {"show", {
        "package_name",
        "global",
        "version"
    }},
    {"update", {
        "package_name",
        "global"
        "lua-version"
    }}
};