add_library(catch INTERFACE)
target_include_directories(catch INTERFACE .)

add_executable(tests main.cpp)
target_link_libraries(tests catch claire)
