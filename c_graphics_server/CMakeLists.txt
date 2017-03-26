cmake_minimum_required(VERSION 3.2)
project(graphics_server)

file(GLOB sources src/*.cpp)


add_executable(app ${sources})
target_link_libraries(app SDL2)
