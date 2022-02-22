#include "install.h"
#include "lpm/macros.h"
#include "lpm/packages.h"

using namespace LPM;

void Handlers::install(args_t& args) {
    LPM_PRINT("Handlers::install()");
    PackagesData package(LPM_DEFAULT_PACKAGES_PATH);
}