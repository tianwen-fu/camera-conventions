#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>

#include <glad/gl.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include "GLStructures.h"
#include "readCamera.h"
#include "readGeometry.h"

using std::cout, std::endl, std::clog;
using std::unique_ptr;

constexpr auto WIDTH = 800;
constexpr auto HEIGHT = 600;

static void framebufferSizeCallback(GLFWwindow *window, const int width,
                                    const int height) {
    glViewport(0, 0, width, height);
}

static void messageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam) {
    if (type == GL_DEBUG_TYPE_ERROR)
        clog << "GL ERROR [";
    else
        clog << "GL Message [";
    clog << "severity " << severity << "] " << message << endl;
}

namespace SceneObjects {
// collect the opengl global variables together
std::vector<std::array<float, 3>> vertices;
std::vector<std::array<float, 3>> colors;

unique_ptr<VertexArray> vao;
unique_ptr<GLProgram> program;
CameraParams cameraParams;
float perspectiveMatrix[16];
} // namespace SceneObjects

static void initializeScene() {
    using namespace SceneObjects;
    readGeometry("assets/geometries/pyramid.json", vertices, colors);
    assert(vertices.size() == colors.size());
    auto verticesVBO = std::make_unique<VBO>(
        reinterpret_cast<const float *>(vertices.data()), 3, vertices.size(),
        GL_STATIC_DRAW);
    auto colorsVBO = std::make_unique<VBO>(
        reinterpret_cast<const float *>(colors.data()), 3,
        colors.size(), GL_STATIC_DRAW);
    program = std::make_unique<GLProgram>(std::vector{
        ShaderPath(ShaderType::VertexShader, "assets/shaders/camView.vert"),
        ShaderPath(ShaderType::FragmentShader, "assets/shaders/camView.frag")});
    program->use();
    vao = std::make_unique<VertexArray>();
    vao->bind();
    vao->addVBO(std::move(verticesVBO), *program, "position");
    vao->addVBO(std::move(colorsVBO), *program, "color");
    program->validateAllAttributesSet(*vao);
    glPointSize(10.0f);
    cameraParams =
        CameraParams::fromFile("assets/cameras/testCam.json", WIDTH, HEIGHT);
    program->use();
    program->setUniformMatrix4fv(
        "extrinsics",
        reinterpret_cast<const float *>(cameraParams.T), GL_TRUE);
    cameraParams.toPerspectiveMatrix(perspectiveMatrix, 0.1f, 3.0f);
    program->setUniformMatrix4fv("intrinsics", perspectiveMatrix, GL_FALSE);
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
    GLFWwindow *window =
        glfwCreateWindow(WIDTH, HEIGHT, "CameraView", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }
    clog << "OpenGL Version: " << GLAD_VERSION_MAJOR(version) << "."
        << GLAD_VERSION_MINOR(version) << endl;
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
        glDrawArrays(GL_TRIANGLES, 0, SceneObjects::vertices.size());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}