#include <iostream>
#include <string>
#include "packages.h"

#include "static/messages.h"

void print_help() {
    std::cout << MESSAGES_HELP << std::endl;
}

int main(int argc, char *argv[]) {
    print_help();
}