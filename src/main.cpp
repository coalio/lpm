#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include "lpm/types.h"
#include "lpm/macros.h"
#include "args.h"
#include "argparser.h"
#include "static/messages.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        LPM_PRINT(MESSAGES_HELP);
        return 1;
    }

    std::string command = argv[1];
    args_t args = ArgParser::parse(argc, argv, 2);

    // Manually parse obligatory arguments.
    if (
        command == "add" ||
        command == "remove" ||
        command == "show"
    ) {
        if (argc < 3) {
            LPM_PRINT_ERROR("Missing argument: package_name");
            LPM_PRINT(MESSAGES_HELP);

            return 1;
        }

        args["package_name"] = argv[2];
    }

    if (!ArgParser::validate(command, args, LPM_VALID_ARGS)) {
        LPM_PRINT(MESSAGES_HELP);
        return 1;
    }
}