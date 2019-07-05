cmake_minimum_required(VERSION 3.10)
project(Quest)

set(CMAKE_CXX_STANDARD 17)



find_package(Curses REQUIRED)
add_library(Engine BaseObjects.cpp BaseObjects.h Level.cpp Level.h World.cpp World.h)
target_include_directories(Engine PUBLIC ${CURSES_INCLUDE_DIR})
target_link_libraries(Engine ${CURSES_LIBRARIES})

add_executable(Quest main.cpp)
target_link_libraries(Quest Engine)

add_custom_command(TARGET Quest PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/levels/ $<TARGET_FILE_DIR:Quest>)

#add_executable(Generate generate.cpp)
