#pragma once
#include <string>
#include "lpm/types.h"
#include "lpm/macros.h"

namespace Command {
    enum Type {
        HELP,
        INIT,
        INSTALL,
        ADD,
        RUN,
        REMOVE,
        UPDATE,
        LIST,
        SEARCH,
        REPOSITORY,
        AUDIT,
        SHOW,
        PURGE
    };

    // Pass a command to its respective handler.
    bool execute(Command::Type command_type, args_t& args);
}