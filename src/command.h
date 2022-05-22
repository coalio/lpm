#pragma once
#include <string>
#include "lpm/types.h"
#include "lpm/macros.h"

namespace Command {
    enum Type {
        AUDIT = 0,
        CONFIGURE = 1,
        HELP = 2,
        INIT = 3,
        INSTALL = 4,
        LIST = 5,
        PURGE = 6,
        REMOVE = 7,
        REPOSITORY = 8,
        RUN = 9,
        SEARCH = 10,
        SHOW = 11,
        UPDATE = 12,

        UNKNOWN = -1
    };

    // Pass a command to its respective handler.
    bool execute(Command::Type command_type, args_t& args);
}