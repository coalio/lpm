#include "install.h"
#include "lpm/macros.h"
#include "lpm/manifests.h"
#include "lpm/pathfinder.h"
#include "lpm/env.h"

using namespace LPM;

void Handlers::install(args_t& args) {
    Manifests::Packages package(
        LPM_DEFAULT_PACKAGES_PATH
    );

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

        std::string cache_path = Env::fill_env_vars(cache);

        if (cache_path.empty()) {
            LPM_PRINT_DEBUG("No cache path found for repository: " << repository.first);
            LPM_PRINT_DEBUG("Downloading manifest from: " << repository.second["url"]);

            // TODO: Download with curl and set cache_path to the downloaded file
        }

        LPM_PRINT_DEBUG("Cache path found for repository: " << repository.first);
        LPM_PRINT_DEBUG("Loading manifest from: " << cache_path);

        Manifests::Repository repository_manifest(cache_path);
    }
}