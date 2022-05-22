#pragma once
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include "lpm/types.h"

namespace ArgParser {
    // Check if the arguments provided and the command are valid.
    Command::Type validate(std::string& command, args_t& args, const args_map_t& valid_args) {
        for (int index = 0; auto& valid_arg_pair : valid_args) {
            // Check if the command provided is valid.
            if (valid_arg_pair.first == command) {
                auto& valid_values = valid_arg_pair.second;

                // We need to check if all the user-provided arguments
                // are valid for this command.
                for (auto& user_arg : args) {
                    if (
                        std::find(
                            valid_values.begin(),
                            valid_values.end(),
                            user_arg.first
                        ) == valid_values.end()
                    ) {
                        LPM_PRINT_ERROR("Unrecognized parameter: " << user_arg.first);
                        return Command::Type::UNKNOWN;
                    }
                }

                return (Command::Type) index;
            }

            ++index;
        }

        LPM_PRINT_ERROR("Unrecognized command: " << command);
        return Command::Type::UNKNOWN;
    }

    args_t parse(
        int& argc,
        char* argv[],
        int start_index,
        std::vector<std::string>& remainder
    ) {
        args_t args;

        for (int i = start_index; i < argc; i++) {
            if (argv[i][0] == '-') {
                // Assign argv[i] to a new string and remove all leading dashes.
                std::string key = argv[i];
                size_t pos = key.find_first_not_of('-');
                key = key.substr(pos);

                // Check if there is a next argument and that it doesn't start with a '-'
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args[key] = argv[i + 1];
                } else {
                    args[key] = "";
                }
            } else {
                remainder.push_back(argv[i]);
            }
        }

        return args;
    }
}