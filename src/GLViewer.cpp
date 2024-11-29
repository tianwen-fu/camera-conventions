#include <cstdio>
#include <iostream>

#include <glad/gl.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

int main() {
    if(!glfwInit()){
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(640, 480, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }

    // Successfully loaded OpenGL
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    return 0;
}
