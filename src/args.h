#pragma once
#include "lpm/types.h"

args_map_t const LPM_VALID_ARGS {
    {"help", {""}},
    {"init", {
        "name"
        "version"
        "description"
        "author"
        "license"
        "homepage"
        "repository"
        "main"
        "lua-version"
    }},
    {"install", {
        "recursive"
    }},
    {"add", {
        "package_name",
        "global",
        "version",
        "lua-version"
    }},
    {"remove", {
        "global",
        "version",
    }},
    {"update", {
        "package_name",
        "global"
    }},
    {"list", {
        "global"
    }},
    {"search", {
        "package_name"
    }},
    {"repository", {
        "action"
    }},
    {"audit", {
        "recursive"
    }},
    {"show", {
        "package_name",
        "global",
        "version"
    }},
    {"purge", {
        "recursive"
    }}
};