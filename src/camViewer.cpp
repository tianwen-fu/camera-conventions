#include <cstdio>
#include <iostream>
#include <memory>

#include <glad/gl.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include "GLStructures.h"
#include "readCamera.h"

using std::cout, std::endl, std::clog;
using std::unique_ptr;

constexpr auto WIDTH = 800;
constexpr auto HEIGHT = 600;

static void framebufferSizeCallback(GLFWwindow *window, const int width,
                                    const int height) {
    glViewport(0, 0, width, height);
}

static void messageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity,
                            GLsizei length, const GLchar *message,
                            const void *userParam) {
    if (type == GL_DEBUG_TYPE_ERROR)
        clog << "GL ERROR [";
    else
        clog << "GL Message [";
    clog << "severity " << severity << "] " << message << endl;
}

namespace SceneObjects {
float vertices[] = {
    0.0, 0.2, 0.5, // +x
    0.2, -0.1, 0.3,
    0.2, -0.1, 0.7,
    0.0, 0.2, 0.5, // +z
    -0.2, -0.1, 0.7,
    0.2, -0.1, 0.7,
    0.0, 0.2, 0.5, // -x
    -0.2, -0.1, 0.3,
    -0.2, -0.1, 0.7,
    0.0, 0.2, 0.5, // -z
    -0.2, -0.1, 0.3,
    0.2, -0.1, 0.3,
    0.2, -0.1, 0.3, // bottom +x +z
    0.2, -0.1, 0.7,
    -0.2, -0.1, 0.7,
    -0.2, -0.1, 0.3, // bottom -x -z
    0.2, -0.1, 0.3,
    -0.2, -0.1, 0.7
};

float colors[] = {
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0
};
static_assert(std::size(vertices) == std::size(colors));

unique_ptr<VertexArray> vao;
unique_ptr<GLProgram> program;
CameraParams cameraParams;
}

static void initializeScene() {
    auto vertices = std::make_unique<VBO>(SceneObjects::vertices, 3,
                                          std::size(SceneObjects::vertices) / 3,
                                          GL_STATIC_DRAW);
    auto colors = std::make_unique<VBO>(SceneObjects::colors, 3,
                                        std::size(SceneObjects::colors) / 3,
                                        GL_STATIC_DRAW);
    SceneObjects::program = std::make_unique<GLProgram>(std::vector{
        ShaderPath(ShaderType::VertexShader, "assets/shaders/camView.vert"),
        ShaderPath(ShaderType::FragmentShader, "assets/shaders/camView.frag")
    });
    SceneObjects::program->use();
    SceneObjects::vao = std::make_unique<VertexArray>();
    SceneObjects::vao->bind();
    SceneObjects::vao->addVBO(std::move(vertices), *SceneObjects::program,
                              "position");
    SceneObjects::vao->addVBO(std::move(colors), *SceneObjects::program,
                              "color");
    SceneObjects::program->validateAllAttributesSet(*SceneObjects::vao);
    glPointSize(10.0f);
    SceneObjects::cameraParams = CameraParams::fromFile(
        "assets/cameras/testCam.txt");
    SceneObjects::program->use();
    SceneObjects::program->
        setUniformMatrix4fv("extrinsics",
                            reinterpret_cast<const float *>(
                                SceneObjects::cameraParams.T), GL_TRUE);
}

int main() {
    if (!glfwInit()) {
        clog << "Failed to initialize GLFW!" << endl;
        return -1;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#endif
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CameraView", nullptr,
                                          nullptr);
    glfwMakeContextCurrent(window);

    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }
    clog << "OpenGL Version: " << GLAD_VERSION_MAJOR(version) << "." <<
        GLAD_VERSION_MINOR(version) << endl;
    clog << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
    clog << "Shading Language Version: "
        << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    glfwSetWindowSize(window, WIDTH, HEIGHT);
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glEnable(GL_DEPTH_TEST);
#ifndef __APPLE__
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, nullptr);
#endif

    initializeScene();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SceneObjects::program->use();
        SceneObjects::vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, std::size(SceneObjects::vertices) / 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}