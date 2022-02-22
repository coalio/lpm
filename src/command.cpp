#include <string>
#include "lpm/types.h"
#include "lpm/macros.h"
#include "command.h"
#include "errorhandling.h"
#include "handlers/handlers.h"

bool Command::execute(Type command_type, args_t& args) {
    try {
        switch (command_type) {
            case Type::INSTALL:
                Handlers::install(args);
                break;
            default:
                LPM_PRINT_ERROR("Unsupported command: " << command_type);
                return false;
        }
    } catch (...) {
        LPM_PRINT_ERROR("Failed to execute command: " << command_type);
        LPM_PRINT_DEBUG("Exception:\n" << ErrorHandling::what());
    }

    return true;
}