#include <iostream>
#include <string>
#include "packages.h"

#include "static/messages.h"

std::string MESSAGE_HELP = std::string(
    reinterpret_cast<char*>(___src_static_MESSAGES_HELP)
);

void print_help() {
    std::cout << MESSAGE_HELP << std::endl;
}

int main(int argc, char *argv[]) {
    print_help();
}