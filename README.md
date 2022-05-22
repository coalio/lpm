# LPM: The versatile Lua Package Manager

LPM is a package manager for the Lua language, designed to be flexible, extensible and easy to use.

Currently, it's still in (very) early development, and the design is based in feedback from the community.

## Currently working on it!

Last Commit ([lpm](https://github.com/coalio/lpm/commits/master))

[![Last Commit (lpm)](https://img.shields.io/github/last-commit/coalio/lpm)](https://github.com/coalio/lpm/commits/master)

Last Commit ([lib-lpm](https://github.com/coalio/lib-lpm))

[![Last Commit (lib-lpm)](https://img.shields.io/github/last-commit/coalio/lib-lpm)](https://github.com/coalio/lib-lpm/commits/master)

## Build from source

*Assuming you have CMake 3.16.0 or newer installed*

Install LPM dependencies (libzip, libsqlite3, libcurl, libssl, libgmp3):

```bash
sudo apt install libzip-dev libsqlite3-dev libcurl4-openssl-dev libssl-dev libgmp3-dev

```

Then, you can build LPM from source:

```bash
git clone --recurse-submodules https://github.com/coalio/lpm lpm
cd lpm
cd build
bash build.sh
```

## Usage

```
Usage: lpm <command> [... args]
           ├── add: Adds a new package
           ├── audit: Checks the integrity of all installed packages
           ├── configure: Configures lpm
           ├── help: Displays this help message
           ├── init: Initializes a packages.toml with basic information
           ├── install: Finds and installs a packages.toml
           ├── purge: Uninstalls lua_modules
           ├── remove: Removes a package
           ├── run: Runs a package
           ├── show: Displays information about a package
           └── update: Updates a package

Options:
    add <package_name>: Installs a package.
        --global [-g]: Install package globally.
        --version: Install specific version of this package. Defaults to latest version.
        --lua-version <lua-version>: Installs the package for this lua version.

    audit: Checks the integrity of all installed packages in the current project.

    configure: Allows you to configure lpm settings.

    help [--help, -h]: Displays this help message.

    init: Initializes a packages.toml file.
        --name=<name>
        --version=<version>
        --description=<description>
        --author=<author>
        --license=<license>
        --homepage=<homepage>
        --repository=<repository>
        --main=<main>
        --lua-version=<lua-version>

    install: Finds a packages.toml file and installs all modules in it.
        --recursive [-r]: Installs all packages.toml files recursively, starting at the current directory.

    list: Lists all installed packages.
        --global [-g]: Lists all globally installed packages.

    purge: Purges all packages installed by LPM for the current project.
        --recursive [-r]: Purges all packages recursively, starting at current directory.

    remove <package_name>: Removes a package.
        --global [-g]: Removes package globally.

    repository <action>: Manages repositories.
        add <repository>: Adds a repository
        remove <repository>: Removes a repository
        list: Lists all available repositories
        update: Updates all repositories

    run <package_name | path/to/package>: Runs a package's entry point, if runnable.
        --lua-version <lua-version>: Runs the package for this lua version.
                                     Cant be used if path/to/package is specified.

    search <package_name>: Searches for a package in all available repositories.

    show <package_name>: Displays information about a package.
        --global [-g]: Displays information about a package globally.
        --version: Displays information about a specific version of this package.

    update <package_name>: Updates a package.
        --global [-g]: Updates package globally.
```
