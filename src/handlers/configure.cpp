#include <curl/curl.h>
#include <fstream>
#include <tuple>
#include "lpm/macros.h"
#include "lpm/env.h"
#include "lpm/utils.h"
#include "lpm/pathfinder.h"
#include "lpm/env.h"
#include "lpm/manifests.h"
#include "lpm/errors.h"
#include "lpm/db.h"
#include "lpm/chmod.h"
#include "static/defaults.h"

#include "configure.h"

using namespace LPM;

void Handlers::configure(args_t& args) {
    Errors::ErrorList errors;

    // Use pathfinder to locate the lpm.toml file, and load it
    Manifests::Config config_manifest;
    std::string config_path = PathFinder::locate_config();
    if (config_path.empty()) {
        // Create a lpm.toml manifest for the current user
        LPM_PRINT_DEBUG("No lpm.toml file found, creating one");

        const std::vector<std::string> CONFIG_PATHS {
            LPM_CONFIG_PATHS
        };

        std::string lpm_config_path = CONFIG_PATHS[0];
        Env::fill_env_vars(lpm_config_path);
        // Print content of env var LPM_ROOT
        LPM_PRINT_DEBUG("LPM_ROOT: " << Env::get("LPM_ROOT", ""));

        if (
            !Utils::write_file(
                lpm_config_path,
                reinterpret_cast<char*>(LPM_TOML_DEFAULT)
            )
        ) {
            // This error is fatal, I'm sorry
            LPM_PRINT_ERROR("Unable to create lpm.toml file at path: " << lpm_config_path);
            LPM_PRINT_ERROR("You may need to create the file manually");

            throw std::runtime_error("Unable to create lpm.toml file");
        }

        // Load the default config via toml
        config_manifest.path = lpm_config_path;

        // load(bool ignore_missing = true)
        // Since it's a first-time configuration, we don't want to
        // throw an error if some settings aren't found.
        config_manifest.load(true);

        // Time to find the lua interpreters
        // So, let's look in the PATH
        std::vector<std::string> paths = Utils::split(
            Env::get("PATH", ""),
            LPM_PATH_VAR_SEPARATOR
        );

        std::vector<
            std::tuple<std::string, std::string, std::string>
        > luas {
            LPM_LUA_VERSION_TUPLES
        };

        for (auto& [version, name, found_path] : luas) {
            bool found = false;
            std::string lua_path;
            std::string tested_paths = "\n";

            for (auto& path : paths) {
                lua_path = (
                    path + LPM_PATH_SEPARATOR + name
                    #if defined(__windows__) || defined(_WIN32)
                        // Windows is special
                        + ".exe";
                    #endif
                );

                if (Utils::fs::exists(lua_path)) {
                    // We found an interpreter, so we can add it to the config manifest
                    config_manifest.luas[version] = lua_path;

                    break;
                } else {
                    // We should add the tested paths to the error message for
                    // easier troubleshooting
                    tested_paths += "\tno file '" + lua_path + "'\n";
                }
            }

            if (!found) {
                errors.add(
                    "interpreters search",
                    "Unable to find an interpreter for " + version +
                    ": " + tested_paths
                );
            }
        }

        // Save the config
        config_manifest.save();
        LPM_PRINT_DEBUG("Created lpm.toml file at path: " << lpm_config_path);
    } else {
        // We found a lpm.toml file, so let's load it
        config_manifest.path = config_path;
        config_manifest.load();
    }

    // Let's open the database at config_manifest.packages_db
    // and create it if it doesn't exist
    DB::SQLite3 db;
    std::string db_path = config_manifest.packages_db;
    Env::fill_env_vars(db_path);

    try {
        bool is_new = !Utils::fs::exists(db_path);
        db.open(db_path);

        if (!db.is_open) {
            throw std::runtime_error("Unable to open database");
        }

        if (is_new) {
            // Give owner S_IRWXU permissions to the database
            // This is needed for the database to be accessible by
            // external utilities. By default it's read-only.
            wlchmod(db_path.c_str(), S_IRWXU);

            // If the database is new, we need to create the structure,
            // so let's do that by running the initialization migration
            bool success = db.execute(
                reinterpret_cast<char*>(LPM_SQL_INIT_MIGRATION)
            );
        }
    } catch (...) {
        LPM_PRINT_ERROR(
            "Unexpected error occurred while opening the database at: " << db_path <<
            "\n"
            "The database file may be corrupted, or you may not have permission to open it"
        );
    }
}