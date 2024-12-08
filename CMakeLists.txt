cmake_minimum_required(VERSION 3.22)
project(cameraParameters)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

find_package(OpenGL REQUIRED)

add_executable(camViewer src/camViewer.cpp src/GLProgram.cpp src/GLProgram.h src/GLStructures.cpp src/GLStructures.h
        src/readCamera.cpp
        src/readCamera.h)
target_link_libraries(camViewer ${OPENGL_LIBRARIES})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(GLFW3 QUIET)
if(GLFW3_FOUND)
    message(STATUS "GLFW found, using system version")
    target_include_directories(camViewer PUBLIC ${GLFW3_INCLUDE_DIR})
    target_link_libraries(camViewer ${GLFW3_LIBRARY})
else ()
    message(STATUS "GLFW not found, using bundled version")
    set(GLFW_LIBRARY_TYPE SHARED)
    add_subdirectory(external/glfw)
    target_include_directories(camViewer PUBLIC "external/glfw/include")
    target_link_libraries(camViewer glfw)
endif ()

add_subdirectory(external/glm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(external/json)

if (MSVC)
    # this would require adding/copying the DLL from a nontrivial path, on which I don't want to work now
    # See https://stackoverflow.com/questions/66531482/application-crashes-when-using-address-sanitizer-with-msvc
    # set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /fsanitize=address")
elseif (UNIX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif ()

target_link_libraries(camViewer glm::glm nlohmann_json::nlohmann_json)
target_include_directories(camViewer PUBLIC
        ${OPENGL_INCLUDE_DIR}
        "external/glad/include"
        "external/glm")
target_sources(camViewer PUBLIC external/glad/src/gl.c external/glad/src/egl.c)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(camViewer copy_assets)
if(WIN32)
    add_custom_target(copy_dlls
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:camViewer> $<TARGET_FILE_DIR:camViewer>
            COMMAND_EXPAND_LISTS
    )
    add_dependencies(camViewer copy_dlls)
    # add this line to avoid CMake failing to copy the (nonexistent) DLLs
    add_dependencies(copy_dlls glfw glm::glm nlohmann_json::nlohmann_json)
endif ()