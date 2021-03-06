# LPM: The versatile Lua Package Manager

LPM is a package manager for the Lua language, designed to be flexible, extensible and easy to use.

---

Last Commit ([lpm](https://github.com/coalio/lpm/commits/master))

[![Last Commit (lpm)](https://img.shields.io/github/last-commit/coalio/lpm)](https://github.com/coalio/lpm/commits/master)

Last Commit ([lib-lpm](https://github.com/coalio/lib-lpm))

[![Last Commit (lib-lpm)](https://img.shields.io/github/last-commit/coalio/lib-lpm)](https://github.com/coalio/lib-lpm/commits/master)

```
Usage: lpm <command> [... args]
           ├── init: Iniializes a packages.toml with basic information
           ├── install: Finds and installs a packages.toml
           ├── add: Adds a new package
           ├── remove: Removes a module
           ├── audit: Checks the integrity of all installed packages
           ├── show: Displays information about a package
           └── purge: Uninstalls lua_modules

Options:
    --help [-h]: Show this screen.

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

    add <package_name>: Installs a package
        --global [-g]: Install package globally. Only available for compiled packages.
        --version: Install specific version of this package.
        --lua-version=<lua-version>: Installs the latest version of this package compliant with this lua version.

    remove <package_name>: Removes a package
        --global [-g]: Removes package globally. Only available for compiled packages.
        --version: Removes specific version of this package.

    audit: Checks the integrity of all installed packages in the current directory.
        --recursive [-r]: Does the above but recursively, starting at current directory.

    show <package_name>: Displays information about a package
        --global [-g]: Displays information about a package globally. Only available for compiled packages.
        --version: Displays information about a specific version of this package.

    purge: Uninstalls all lua_modules
        --recursive [-r]: Uninstalls all lua_modules recursively, starting at current directory.
```
