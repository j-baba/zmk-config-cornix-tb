# Trackball PAW3222 Shield CMake configuration

# Add custom LED indicator source
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/trackball_led.c)
