#include <curl/curl.h>
#include "lpm/macros.h"
#include "lpm/manifests.h"
#include "lpm/pathfinder.h"
#include "lpm/scope_destructor.h"
#include "lpm/requests.h"
#include "lpm/env.h"
#include "lpm/utils.h"
#include "lpm/dependencies.h"
#include "lpm/errors.h"
#include "lpm/repositories.h"
#include "lpm/hashing.h"

#include "install.h"

using namespace LPM;
using Response = Requests::Response;

typedef std::map<unsigned long long, std::vector<std::string>> FilesMap;

// todo:: TEST THIS, i already successfully compiled lpm, now we
// have to test it and fix bugs, yay.

// Installation mode
enum InstallMode {
    PROJECT = 0,
    GLOBAL = 1
};

void Handlers::install(args_t& args) {
    // Installation mode (if a packages.toml file exists, it's a project installation)
    InstallMode mode;

    std::string packages_toml_path = PathFinder::locate_packages_toml();
    if (!packages_toml_path.empty()) {
        mode = PROJECT;
    } else {
        if (!args.count("global")) {
            LPM_PRINT_ERROR("No packages.toml file found in the parent directories and no -g [--global] flag provided");
            return;
        }

        mode = GLOBAL;
    }

    // Reusable error variable
    std::string error;

    // Map of installed files per package id
    FilesMap installed_files;

    // List of errors. We'll use this to log all errors that occur during the
    // addation process.
    Errors::ErrorList errors;

    // Lua version to perform the installation for
    std::string lua_version;

    // List of dependencies to install. This list will grow recursively
    std::vector<Dependencies::Dependency> dependencies_list;

    // Locate and load the lpm.toml file
    std::string config_path = PathFinder::locate_config();
    if (config_path.empty()) {
        throw std::runtime_error("Failed to locate lpm.toml file");
    }

    Manifests::Config config(config_path);

    // Format the packages db path
    std::string packages_db_path = config.packages_db;
    Env::fill_env_vars(packages_db_path);

    // Connect to the packages db
    DB::SQLite3 db_connection(packages_db_path);

    if (mode == InstallMode::PROJECT) {
        // Load the packages.toml manifest
        Manifests::Packages packages(
            LPM_PACKAGES_MANIFEST_NAME
        );

        if (args.count("lua_version") && args["lua_version"] != packages.lua_version) {
            LPM_PRINT_ERROR("Lua version specified in the command line is not supported by the project");
            return;
        }

        lua_version = packages.lua_version;

        // Copy packages.dependencies into dependencies_list
        for (auto& dependency : packages.dependencies) {
            dependencies_list.push_back(dependency);
        }
    } else {
        lua_version = args["lua_version"];

        // TODO:: Set the list of packages to install from the arguments
        // i left it here for reference
    }

    // We need to get a list of the repositories
    // Let's get them from the database
    std::vector<
        std::map<std::string, std::string>
    > repositories = Repositories::get_repositories(db_connection);

    LPM_PRINT_DEBUG("Repositories size: " + std::to_string(repositories.size()));

    // For every repository, check if we have a cache path for it
    // If not, we'll download it from the repository url
    for (auto& repository : repositories) {
        std::string repository_cache_path = repository["cache"];

        if (repository_cache_path.empty() || !Utils::fs::exists(repository_cache_path)) {
            LPM_PRINT_DEBUG("No cache path found for repository: " << repository["name"]);
            LPM_PRINT_DEBUG("Downloading manifest from: " << repository["url"]);

            Response response;
            std::string repository_manifest_content;

            try {
                // Initialize a new curl handle and give it a scope_destructor, so
                // we can call curl_easy_cleanup() on it when it goes out of scope
                scope_destructor<CURL*> curl_handle(curl_easy_init(), curl_easy_cleanup);
                response = Requests::get(repository["url"], curl_handle.get());

                if (response.status_code != 200) {
                    error =
                        "(" + repository["name"] + ") cant download from: " + repository["url"] +
                        ", status code " + std::to_string(response.status_code);
                }

                if (response.body.empty()) {
                    error = "(" + repository["name"] + ") empty response";
                } else {
                    repository_manifest_content = response.body;
                }
            } catch (const std::exception& e) {
                error =
                    "(" + repository["name"] + ") cant download from: " + repository["url"] +
                    ", error: " + e.what();
            }

            if (!error.empty()) {
                errors.add(
                    "repository fetching",
                    error
                );
            } else {
                error = "";
            }

            // Copy config.repositories_cache to cache_path
            repository_cache_path = config.repositories_cache;

            // Apply env vars without replacing empty values
            Env::fill_env_vars(repository_cache_path, false);

            // Apply the repository name
            Utils::format(repository_cache_path, {
                {"?", repository["name"]}
            });

            LPM_PRINT_DEBUG("Writing manifest to: " << repository_cache_path);

            // Write the manifest to the cache path
            if (!Utils::write_file(repository_cache_path, repository_manifest_content)) {
                errors.add(
                    "repository fetching",
                    "(" + repository["name"] + ") cant write to: " + repository_cache_path
                );

                continue;
            }

            // Update the repository cache path in the database
            repository["cache"] = repository_cache_path;
            Repositories::update_repository_cache(
                db_connection,
                std::stoi(repository["id"]),
                repository_cache_path
            );

            LPM_PRINT_DEBUG("Updated the repository cache path in the database");
        }
    }

    std::string packages_path = Env::get("LPM_PACKAGES_PATH", config.packages_path);
    std::vector<Dependencies::PackageInformation*> packages_to_install;

    // For every dependency, call find_dependency
    // TODO:: i left it here
    // now i am supposed to implement all of the database procedures
    // and make sure that packages are added to the database
    // and that the changes i did to the repository::update_repository_cache
    // are working as expected.

    // i also need to check if the arg parsing is also working,
    // i removed the "add" command as we're gonna use "install"
    // for installing a packages.toml and a new package or packages
    // so, we need to check if we can parse a packages list.

    // BUT FOR NOW, lets just get the sqlite part working and
    // let's compile and test it to see if it's going as expected.
    for (size_t i = 0; i < dependencies_list.size(); i++) {
        // Find the dependency
        auto& dependency = dependencies_list[i];

        // Get the expected package path for this dependency
        // so we can check beforehand if it exists
        std::string curr_package_path = packages_path;
        Env::fill_env_vars(curr_package_path, false);
        Utils::format(curr_package_path, {
            {"package_name", dependency.first},
            {"lua_version", lua_version}
        });

        // If the dependency is already added, skip it
        if (Dependencies::is_added(db_connection, curr_package_path)) {
            LPM_PRINT_DEBUG("Dependency " << dependency.first << " is already added");
            continue;
        }

        auto& package = (
            packages_to_install.push_back(
                Dependencies::find_dependency(
                    dependency,
                    repositories
                )
            ),
            packages_to_install.back()
        );

        if (package == NULL) {
            errors.add(
                "dependency browsing",
                "(" + dependency.first + ") cant find dependency in any of currently available repositories"
            );

            break;
        }

        // Download the manifest
        Requests::Response response;
        std::string package_manifest_content;

        try {
            // Initialize a new curl handle and give it a scope_destructor, so
            // we can call curl_easy_cleanup() on it when it goes out of scope
            scope_destructor<CURL*> curl_handle(curl_easy_init(), curl_easy_cleanup);
            response = Requests::get(package->manifest_url, curl_handle.get());

            if (response.status_code != 200) {
                error =
                    "(" + package->name + "@" + package->version + ") cant download from: " + package->manifest_url +
                    ", status code " + std::to_string(response.status_code);
            }

            if (response.body.empty()) {
                error = "(" + package->name + "@" + package->version + ") empty response";
            } else {
                package_manifest_content = response.body;
            }
        } catch (const std::exception& e) {
            error =
                "(" + package->name + "@" + package->version + ") cant download from: " + package->manifest_url +
                ", error: " + e.what();
        }

        std::string manifest_cache_path = config.packages_cache;

        Env::fill_env_vars(manifest_cache_path, false);
        Utils::format(manifest_cache_path, {
            {"package_name", package->name},
            {"package_version", package->version},
            {"?", package->name + "-" + package->version + "-manifest.toml"}
        });

        if (!Utils::write_file(manifest_cache_path, package_manifest_content)) {
            errors.add(
                "dependency fetching",
                "(" + package->name + "@" + package->version + ") cant write package manifest to: " + manifest_cache_path
            );

            break;
        }

        // We need to analyze package dependencies and see if we can fulfill their requirements,
        // otherwise we cannot add the package
        Manifests::Package package_manifest(manifest_cache_path);

        // Update PackageInformation package_url and package_type with the package_manifest datta
        package->package_url = package_manifest.source.url;
        package->package_type = package_manifest.source.package_type;

        // Check package_manifest to see if its compatible with the current platform and lua version
        bool is_compatible = false;
        [[maybe_unused]] std::string lua_bin;
        for (auto [version, lua_bin] : config.luas) {
            if (version == lua_version || version == "*") {
                is_compatible = true;
                break;
            }
        }

        if (!is_compatible) {
            errors.add(
                "dependency fetching",
                "(" + package->name + "@" + package->version + ") is not compatible with " + lua_version
            );

            break;
        }

        // TODO:: Add platform checking (also deduce installation candidate based on the platform)

        for (auto& package_manifest_dependency : package_manifest.dependencies) {
            // Add the dependency to the dependencies_list list
            dependencies_list.push_back(package_manifest_dependency);
        }
    }

    // Once we're done auditing the dependencies, we can add the packages
    for (auto& package : packages_to_install) {
        // Install the package (add it to the list of installed packages)
        // TODO:: Use Dependencies::add here and pass the PackageInformation to finally install the package
        std::string package_cache_path = config.packages_cache;
        Env::fill_env_vars(package_cache_path, false);

        if (package->package_type == "zip") {
            Utils::format(package_cache_path, {
                {"package_name", package->name},
                {"package_version", package->version},
                {"?", package->name + "-" + package->version + ".zip"}
            });
        }

        std::string curr_package_path = packages_path;
        Env::fill_env_vars(curr_package_path, false);
        Utils::format(curr_package_path, {
            {"package_name", package->name},
            {"lua_version", lua_version}
        });

        // Call Dependencies::add to download and extract the package at the curr_package_path directory
        if (!Dependencies::add(db_connection, package, package_cache_path, curr_package_path, error)) {
            errors.add(
                "dependency installation",
                "(" + package->name + "@" + package->version + ") unable to install: " + error
            );
        } else {
            LPM_PRINT_DEBUG("Installed package: " << package->name << "@" << package->version << " at " << curr_package_path);
        }

        // Delete the package from the list of packages to install
        delete package;
    }

    errors.print();
}
