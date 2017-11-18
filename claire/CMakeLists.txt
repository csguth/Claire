add_library(claire Claire.hpp Claire.cpp Light.hpp Light.cpp Plant.hpp Plant.cpp Sensor.hpp Sensor.cpp Integer.hpp)
target_include_directories(claire PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
