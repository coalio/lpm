# The LPM lib includes abstract functions with all of LPM features.
# You can compile this lib independently and use it in your own projects.
include_directories(./include)
add_library(lpm-lib
    packages.cpp
)