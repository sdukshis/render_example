cmake_minimum_required(VERSION 2.8)

project(render C)

add_executable(${PROJECT_NAME} draw.c geometry.c main.c model.c tga.c)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} m)
endif()

enable_testing()

add_test(NAME Cat COMMAND ${PROJECT_NAME} ../cat/cat.obj ../cat/cat_diff.tga cat.tga)