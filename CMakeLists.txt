cmake_minimum_required(VERSION 3.22)
project(cameraParameters)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

find_package(OpenGL REQUIRED)

add_executable(GLViewer src/GLViewer.cpp)
target_link_libraries(GLViewer ${OPENGL_LIBRARIES})

add_subdirectory(external/glfw)

if(MSVC)
    add_compile_options("/fsanitize=address")
endif ()

target_link_libraries(GLViewer glfw)
target_include_directories(GLViewer PUBLIC ${OPENGL_INCLUDE_DIR} "external/glad/include" "external/glfw/include")
target_sources(GLViewer PUBLIC external/glad/src/gl.c external/glad/src/egl.c)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_custom_target(copy_dlls
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:GLViewer> $<TARGET_FILE_DIR:GLViewer>
        COMMAND_EXPAND_LISTS
)
add_dependencies(GLViewer copy_assets copy_dlls)
