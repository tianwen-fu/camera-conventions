#include "GLStructures.h"

#include <iostream>
#include <memory>
#include <cstring>
#include <cassert>

template<GLenum GL_BUFFER_TYPE, typename DataType>
ArrayBuffer<GL_BUFFER_TYPE, DataType>::ArrayBuffer(const DataType *data,
                                                   const GLsizeiptr count,
                                                   const GLenum usage)
    : size(count) {
    glGenBuffers(1, &handle);
    glBindBuffer(GL_BUFFER_TYPE, handle);
    glBufferData(GL_BUFFER_TYPE, count * sizeof(DataType), data, usage);
}

template<GLenum GL_BUFFER_TYPE, typename DataType>
void ArrayBuffer<GL_BUFFER_TYPE, DataType>::bind() const {
    glBindBuffer(GL_BUFFER_TYPE, handle);
}

template<GLenum GL_BUFFER_TYPE, typename DataType>
ArrayBuffer<GL_BUFFER_TYPE, DataType>::~ArrayBuffer() {
    glDeleteBuffers(1, &handle);
}

VBO::VBO(const float *data, const GLint dimension, const GLsizeiptr numPoints,
         const GLenum usage)
    : ArrayBuffer(data, numPoints * dimension, usage), _dimension(dimension) {
    // count * 3 for three numbers per point
}

void VBO::render(const GLenum mode) const {
    glBindBuffer(BUFFER_TYPE, handle);
    glDrawArrays(mode, 0, size);
}

GLint VBO::dimension() const { return _dimension; }

template ArrayBuffer<GL_ARRAY_BUFFER, GLfloat>::~ArrayBuffer();

EBO::EBO(const DataType *data, const GLsizeiptr count, const GLenum usage)
    : ArrayBuffer(data, count, usage) {
}

void EBO::render(const GLenum mode) const {
    glBindBuffer(BUFFER_TYPE, handle);
    glDrawElements(mode, size, ARRAY_DATATYPE, 0);
}

// otherwise compiler might complain about undefined reference
template ArrayBuffer<GL_ELEMENT_ARRAY_BUFFER, GLuint>::~ArrayBuffer();

VertexArray::VertexArray() {
    glGenVertexArrays(1, &handle);
    glBindVertexArray(handle);
}

void VertexArray::addVBO(std::unique_ptr<VBO> &&vbo, const GLProgram &program,
                         const char *shaderVariable) {
    bind();
    program.use();
    vbo->bind();
    const auto variable =
            program.locateVariable(shaderVariable, ShaderVariableType::Attribute);
    glEnableVertexAttribArray(variable);
    glVertexAttribPointer(variable, vbo->dimension(), VBO::ARRAY_DATATYPE,
                          GL_FALSE, 0, 0);
    vboList.push_back(std::move(vbo));
    shaderVariableNames.insert(shaderVariable);
}

void VertexArray::bind() const {
    glBindVertexArray(handle);
}

bool VertexArray::isShaderVariableBound(
    const std::string &shaderVariable) const {
    return shaderVariableNames.find(shaderVariable) !=
           shaderVariableNames.end();
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &handle);
}

Texture::Texture(const GLenum target, const std::unique_ptr<unsigned char[]> &pixels, const int width, const int height,
                 const int bytesPerPixel) : target(target) {
    // load the image
    assert(!(width * bytesPerPixel % 4));

    // allocate space for the image
    GLenum pixelFormat;
    switch (bytesPerPixel) {
        case 3:
            pixelFormat = GL_RGB;
            break;
        case 4:
            pixelFormat = GL_RGBA;
            break;
        default:
            throw std::logic_error("Unsupported image format");
    }

    glActiveTexture(target);
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, pixelFormat,
                 GL_UNSIGNED_BYTE, pixels.get());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set the texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Query support for anisotropic texture filtering.
    GLfloat fLargest;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
    std::clog << "Max available anisotropic samples: " << fLargest << std::endl;
    // Set anisotropic texture filtering.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                    0.5f * fLargest);
}

void Texture::bind() const {
    glActiveTexture(target);
    glBindTexture(GL_TEXTURE_2D, handle);
}

void Texture::setSampler(const GLProgram &program,
                         const char *shaderVariable) const {
    program.use();
    program.setUniformScalar(shaderVariable,
                             static_cast<int>(target - GL_TEXTURE0));
}

Texture::~Texture() {
    glDeleteTextures(1, &handle);
}
