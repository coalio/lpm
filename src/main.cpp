#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cstdlib>
#include "lpm/types.h"
#include "lpm/macros.h"
#include "lpm/env.h"
#include "lpm/utils.h"
#include "command.h"
#include "args.h"
#include "argparser.h"
#include "static/messages.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        LPM_PRINT(MESSAGES_HELP << "\n");
        return 1;
    }

    // Check if LPM_ROOT is not set
    if (std::getenv("LPM_ROOT") == NULL) {
        // Use the default LPM_ROOT path
        std::string* lpm_root = new std::string("LPM_ROOT=" LPM_DEFAULT_ROOT);
        LPM::Env::fill_env_vars(*lpm_root);

        // This memory will be freed by the OS when the process exits.
        putenv(lpm_root->data());
    }

    std::string command = argv[1];
    std::vector<std::string> remainder;
    args_t args = ArgParser::parse(argc, argv, 2, remainder);

    // Manually parse obligatory arguments.
    if (
        command == "remove" || command == "show"
    ) {
        if (argc < 3) {
            LPM_PRINT_ERROR("Missing argument: package_name"  << "\n");
            LPM_PRINT(MESSAGES_HELP  << "\n");

            return 1;
        }

        args["package_name"] = LPM::Utils::join(remainder, ' ');
    }

    Command::Type command_type = ArgParser::validate(command, args, LPM_VALID_ARGS);

    if (
        command_type == Command::Type::UNKNOWN ||
        command_type == Command::Type::HELP
    ) {
        LPM_PRINT(MESSAGES_HELP << "\n");
        return command_type < 0 ? 1 : 0;
    }

    if (!Command::execute(command_type, args)) {
        return 1;
    }
}