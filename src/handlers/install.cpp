#include <curl/curl.h>
#include <fstream>
#include "lpm/macros.h"
#include "lpm/manifests.h"
#include "lpm/pathfinder.h"
#include "lpm/scope_destructor.h"
#include "lpm/requests.h"
#include "lpm/env.h"
#include "lpm/utils.h"
#include "lpm/dependencies.h"
#include "lpm/errors.h"

#include "install.h"

using namespace LPM;
using Response = Requests::Response;

void Handlers::install(args_t& args) {
    Errors::ErrorList errors;
    Manifests::Packages packages(
        LPM_PACKAGES_MANIFEST_NAME
    );

    // Copy packages.dependencies into missing_dependencies
    auto missing_dependencies = packages.dependencies;

    // Check if dependencies are installed
    for (auto& dependency : missing_dependencies) {
        if (Dependencies::is_installed(dependency)) {
            missing_dependencies.erase(dependency.first);
        }
    }

    // Locate and load the lpm.toml file
    std::string config_path = PathFinder::locate_config();
    if (config_path.empty()) {
        throw std::runtime_error("Failed to locate lpm.toml file");
    }

    Manifests::Config config(config_path);

    // Iterate through repositories and check if they have a cache
    // If they dont, download it and then continue
    for (auto& repository : config.repositories) {
        auto& cache = repository.second["cache"];
        std::string cache_path = cache;
        Env::fill_env_vars(cache);

        if (cache_path.empty()) {
            LPM_PRINT_DEBUG("No cache path found for repository: " << repository.first);
            LPM_PRINT_DEBUG("Downloading manifest from: " << repository.second["url"]);

            // Create a new response object, we'll use this to
            // store the response from the request.
            Response response;

            try {
                // Initialize a new curl handle and give it a scope_destructor, so
                // we can call curl_easy_cleanup() on it when it goes out of scope
                scope_destructor<CURL*> curl_handle(curl_easy_init(), curl_easy_cleanup);
                response = Requests::get(repository.second["url"], curl_handle.get());

                if (response.status_code != 200) {
                    errors.add(
                        "repository fetching",
                        "(" + repository.first + ") cant download from: " + repository.second["url"] +
                        ", status code " + std::to_string(response.status_code)
                    );

                    continue;
                }
            } catch (const std::exception& e) {
                errors.add(
                    "repository fetching",
                    "(" + repository.first + ") cant download from: " + repository.second["url"] +
                    ", error: " + e.what()
                );

                continue;
            }

            // Copy config.repositories_cache to cache_path
            cache_path = config.repositories_cache;

            // Apply env vars without replacing empty values
            Env::fill_env_vars(cache_path, false);

            // Apply the repository name
            Utils::format(cache_path, {
                {"?", repository.first}
            });

            LPM_PRINT_DEBUG("Writing manifest to: " << cache_path);

            // Write the manifest to the cache path
            if (!Utils::write_file(cache_path, response.body)) {
                errors.add(
                    "repository fetching",
                    "(" + repository.first + ") cant write to: " + cache_path
                );

                continue;
            }

            // Update the config file
            config.repositories[repository.first]["cache"] = cache_path;
            config.save();
        }

        LPM_PRINT_DEBUG("Loading manifest from: " << cache_path);

        Manifests::Repository repository_manifest(cache_path);

        // Once we have the manifest, we can check if the package we need is defined in it
        // So, lets iterate through missing dependencies
        for (auto& missing_dependency : missing_dependencies) {
            // If the package is defined in the manifest, we can install it
            // and then remove it from the missing_dependencies
            if (
                repository_manifest.packages.find(missing_dependency.first) !=
                repository_manifest.packages.end()
            ) {
                // Install the package

                LPM_PRINT_DEBUG("Package " << missing_dependency.first << " is defined in repository " << repository.first);

                auto& package =
                    repository_manifest.packages[missing_dependency.first];

                // Check if this package has any versions
                if (package.versions.empty()) {
                    errors.add(
                        "package search",
                        "repository " + repository.first + " has no versions of " + missing_dependency.first
                    );

                    continue;
                }

                // Check if the version we need is defined in the manifest
                std::string package_version, package_url;
                if (missing_dependency.second == "latest") {
                    // Sort the keys of package.versions in descending order
                    // so we can get the latest version
                    std::vector<std::string> sorted_versions(package.versions.size());
                    std::transform(
                        package.versions.begin(),
                        package.versions.end(),
                        sorted_versions.begin(),
                        [](const auto& pair) {
                            return pair.first;
                        }
                    );

                    std::sort(
                        sorted_versions.begin(),
                        sorted_versions.end(),
                        [](const auto& a, const auto& b) {
                            // Get the major, minor and patch versions
                            // of the versions
                            std::string token;
                            std::istringstream stream_a(a);
                            std::istringstream stream_b(b);

                            std::vector<std::string> tokens_a;
                            while (std::getline(stream_a, token, '.')) {
                                if (!token.empty()) {
                                    tokens_a.push_back(token);
                                }
                            }

                            std::vector<std::string> tokens_b;
                            while (std::getline(stream_b, token, '.')) {
                                if (!token.empty()) {
                                    tokens_b.push_back(token);
                                }
                            }

                            // Compare the major, minor and patch versions
                            // of the versions
                            size_t max_size = std::max(tokens_a.size(), tokens_b.size());
                            for (size_t i = 0; i < max_size; i++) {
                                if (tokens_a.size() - 1 < i || tokens_b.size() - 1 < i) {
                                    return tokens_a.size() > tokens_b.size();
                                }

                                if (tokens_a[i] != tokens_b[i]) {
                                    return std::stoi(tokens_a[i]) > std::stoi(tokens_b[i]);
                                }
                            }

                            return false;
                        }
                    );

                    // Get the latest version
                    const auto& latest_version = sorted_versions.front();
                    package_url = package.versions[latest_version];
                    package_version = latest_version;

                    LPM_PRINT_DEBUG(
                        "Latest version of " << missing_dependency.first << ":"
                        << latest_version
                    );
                } else {
                    // Check if the dependency version is defined in the package
                    if (package.versions.find(missing_dependency.second) == package.versions.end()) {
                        errors.add(
                            "package search",
                            "Dependency " + missing_dependency.first + ":" +
                            missing_dependency.second + " is not defined in the package"
                        );

                        continue;
                    }

                    package_url = package.versions[missing_dependency.second];
                    package_version = missing_dependency.second;

                    LPM_PRINT_DEBUG("Version " << missing_dependency.second << " of " << missing_dependency.first << " is defined");
                }

                // Install the package
                bool success;
                std::string error;

                // We need to copy and format packages_cache and modules_path
                // to cache_path and module_path respectively.
                std::string cache_path = config.packages_cache;
                std::string module_path;
                if (args.contains("global")) {
                    // config.modules_path specifies the directory for global modules
                    module_path = config.modules_path;
                } else {
                    // whereover LPM_DEFAULT_LOCAL_MODULES_PATH specifies the directory for project modules
                    module_path = LPM_DEFAULT_LOCAL_MODULES_PATH;
                }

                // Dependencies::install doesn't format the paths, so we do it here
                Env::fill_env_vars(cache_path, false);
                Utils::format(cache_path, {
                    {"package_name", package.name},
                    {"package_version", package_version},
                    {"?", package.name + "-" + package_version}
                });

                Env::fill_env_vars(module_path, false);
                Utils::format(module_path, {
                    {"module_name", package.name},
                    {"module_version", package_version},
                    {"lua_version", config.luas["default"]}
                });



                success = Dependencies::install(
                    {package.name, package_version},

                    package,
                    cache_path,
                    module_path,
                    error
                );

                if (!success) {
                    errors.add(
                        "dependency installation",
                        "("
                            + missing_dependency.first + ":" + missing_dependency.second +
                        ") " + error
                    );

                    continue;
                } else {
                    LPM_PRINT_DEBUG("Installed dependency " << missing_dependency.first << ":" << missing_dependency.second);

                    missing_dependencies.erase(missing_dependency.first);
                }

            }
        }
    }

    errors.print();
}