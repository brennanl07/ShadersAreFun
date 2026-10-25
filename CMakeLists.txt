cmake_minimum_required(VERSION 4.2.0)

if (NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_C_COMPILER   clang)
    set(CMAKE_CXX_COMPILER clang++)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ShadersAreFun VERSION 0.0.0)

set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_LIB_DIR     "${PROJECT_SOURCE_DIR}/lib")
set(PROJECT_SCRIPTS_DIR "${PROJECT_SOURCE_DIR}/src")

set(DEMO_SOURCE_DIR "${PROJECT_SOURCE_DIR}/demo")
set(DEMO_SCRIPTS_DIR "${DEMO_SOURCE_DIR}/src")

file(GLOB_RECURSE PROJECT_SCRIPTS "${PROJECT_SCRIPTS_DIR}/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "${PROJECT_SCRIPTS_DIR}/*.hpp")
# file(GLOB_RECURSE PROJECT_IMPL    "${PROJECT_SCRIPTS_DIR}/*.hxx")
# file(GLOB_RECURSE PROJECT_MODULES "${PROJECT_SCRIPTS_DIR}/*.cppm")

# add_library(ShadersAreFun STATIC ${PROJECT_SCRIPTS})
add_executable(ShadersAreFun ${PROJECT_SCRIPTS} ${PROJECT_HEADERS})

if(MSVC)
    target_compile_options(ShadersAreFun PRIVATE /W2)
else()
    target_compile_options(ShadersAreFun PRIVATE -Wall -Wextra)
endif()

target_sources(ShadersAreFun
    PRIVATE ${PROJECT_SCRIPTS}

    PUBLIC
        FILE_SET HEADERS TYPE HEADERS
        FILES ${PROJECT_HEADERS}
)

target_include_directories(ShadersAreFun PRIVATE
    "${PROJECT_SCRIPTS_DIR}" "${PROJECT_INCLUDE_DIR}" "${PROJECT_BINARY_DIR}")

# libraries
find_package(OpenGL REQUIRED)

add_subdirectory("${PROJECT_LIB_DIR}/glm")
add_subdirectory("${PROJECT_LIB_DIR}/glfw")

# set(IMGUI_DIR "${PROJECT_LIB_DIR}/ImGui")
# file(GLOB IMGUI_SCRIPTS
#     "${IMGUI_DIR}/*.cpp"
#     "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
#     "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
# )
# add_library(ImGui STATIC ${IMGUI_SCRIPTS})
# target_include_directories(ImGui PUBLIC "${PROJECT_INCLUDE_DIR}")

add_library(glad STATIC "${PROJECT_LIB_DIR}/glad/glad.c")
target_include_directories(glad PUBLIC "${PROJECT_INCLUDE_DIR}")
# end libraries

target_link_libraries(ShadersAreFun PUBLIC
    glfw OpenGL::GL glad)
    # glfw OpenGL::GL ImGui glad)

# file(GLOB_RECURSE DEMO_SCRIPTS "${DEMO_SOURCE_DIR}/*.cpp")
# add_executable(SAFdemo ${DEMO_SCRIPTS})
# target_link_libraries(SAFdemo PRIVATE ShadersAreFun)

# target_include_directories(SAFdemo PRIVATE
#     "${PROJECT_SCRIPTS_DIR}" "${PROJECT_INCLUDE_DIR}" "${PROJECT_BINARY_DIR}")

# add_custom_command(TARGET ShadersAreFun PRE_BUILD COMMAND ${CMAKE_COMMAND} -E 
#     rm -rf $<TARGET_FILE_DIR:ShadersAreFun>/assets/)

# add_custom_command(TARGET ShadersAreFun PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
#     copy_directory ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:ShadersAreFun>/assets/)

add_custom_command(TARGET ShadersAreFun PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
    create_symlink ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:ShadersAreFun>/assets)