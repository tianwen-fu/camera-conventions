#pragma once

#include <glad/gl.h>

#include <memory>
#include <vector>

#include "GLProgram.h"

enum class DISPLAY_MODE : uint8_t {
    POINTS = 0,
    LINES = 1,
    TRIANGLES = 2,
    SMOOTH = 3
};

constexpr GLenum GL_DRAW_MODE_FROM_DISPLAY_MODE[] = {
    GL_POINTS, GL_LINES,
    GL_TRIANGLES
};

template<GLenum GL_BUFFER_TYPE, typename DataType>
class ArrayBuffer {
protected:
    GLuint handle{};
    GLsizei size;
    static constexpr GLenum BUFFER_TYPE = GL_BUFFER_TYPE;

public:
    ArrayBuffer(const DataType *data, GLsizeiptr count, GLenum usage);

    void bind() const;

    virtual void render(GLenum mode) const = 0;

    virtual ~ArrayBuffer();
};

class VBO final : public ArrayBuffer<GL_ARRAY_BUFFER, GLfloat> {
    GLint _dimension; // components per attribute

public:
    VBO(const GLfloat *data, GLint dimension, GLsizeiptr numPoints,
        GLenum usage);

    void render(GLenum mode) const override;

    GLint dimension() const;

    static constexpr GLenum ARRAY_DATATYPE = GL_FLOAT;
};

class EBO final : public ArrayBuffer<GL_ELEMENT_ARRAY_BUFFER, GLuint> {
public:
    using DataType = GLuint;

    EBO(const DataType *data, GLsizeiptr count, GLenum usage);

    void render(GLenum mode) const override;

    static constexpr GLenum ARRAY_DATATYPE = GL_UNSIGNED_INT;
};

class VertexArray final {
    std::vector<std::unique_ptr<VBO> > vboList;
    std::unordered_set<std::string> shaderVariableNames;
    GLuint handle{};

public:
    VertexArray();

    void addVBO(std::unique_ptr<VBO> &&vbo, const GLProgram &program,
                const char *shaderVariable);

    void bind() const;

    bool isShaderVariableBound(const std::string &shaderVariable) const;

    ~VertexArray();
};

class Texture final {
    GLuint handle{};
    const GLenum target;

public:
    Texture(GLenum target, const std::unique_ptr<unsigned char[]> &pixels, int width, int height,
                     int bytesPerPixel);

    void bind() const;

    void setSampler(const GLProgram &program, const char *shaderVariable) const;

    ~Texture();
};
