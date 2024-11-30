cmake_minimum_required(VERSION 3.22)
project(cameraParameters)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

find_package(OpenGL REQUIRED)

add_executable(camViewer src/camViewer.cpp src/GLProgram.cpp src/GLProgram.h src/GLStructures.cpp src/GLStructures.h)
target_link_libraries(camViewer ${OPENGL_LIBRARIES})

set(GLFW_LIBRARY_TYPE SHARED)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)

if (MSVC)
    # this would require adding/copying the DLL from a nontrivial path, on which I don't want to work now
    # See https://stackoverflow.com/questions/66531482/application-crashes-when-using-address-sanitizer-with-msvc
    # set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /fsanitize=address")
elseif (UNIX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif ()

target_link_libraries(camViewer glfw glm::glm)
target_include_directories(camViewer PUBLIC
        ${OPENGL_INCLUDE_DIR}
        "external/glad/include"
        "external/glfw/include"
        "external/glm")
target_sources(camViewer PUBLIC external/glad/src/gl.c external/glad/src/egl.c)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_custom_target(copy_dlls
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:camViewer> $<TARGET_FILE_DIR:camViewer>
        COMMAND_EXPAND_LISTS
)
add_dependencies(camViewer copy_assets copy_dlls)
