#pragma once
#include <glad/gl.h>
//#include <glm/detail/type_vec.hpp>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "glm/vec3.hpp"

class VertexArray; // defined in GLStructures.h

enum class ShaderType : GLenum {
    VertexShader = GL_VERTEX_SHADER,
    FragmentShader = GL_FRAGMENT_SHADER,
    ComputeShader = GL_COMPUTE_SHADER
};

enum class ShaderVariableType { Uniform, Attribute };

struct ShaderSource {
    ShaderType type;
    const char *source;
};

struct ShaderPath {
    ShaderType type;
    const std::string_view path;
};

class GLProgram {
  protected:
    GLuint handle;
    std::unordered_set<std::string> attributeNames;

  public:
    explicit GLProgram(const std::vector<ShaderSource> &shaderSources);
    explicit GLProgram(const std::vector<ShaderPath> &shaderPaths);
    void use() const;
    GLint locateVariable(const char *attributeName,
                         ShaderVariableType type) const;

    void setUniformScalar(const char *name, int value) const;
    void setUniformScalar(const char *name, float value) const;
    void setUniformScalar(const char *name, const int *value,
                          GLsizei count) const;
    void setUniformScalar(const char *name, const float *value,
                          GLsizei count) const;
    void setUniform2fv(const char *name, const float *value,
                       GLsizei count = 1) const;
    void setUniform3fv(const char *name, const float *value,
                       GLsizei count = 1) const;
    void setUniform4fv(const char *name, const float *value,
                       GLsizei count = 1) const;
    void setUniformMatrix4fv(const char *name, const float *value,
                             GLboolean transpose) const;

    void validateAllAttributesSet(const VertexArray &vao) const;
};

class ComputeProgram : public GLProgram {
    GLuint *textures;
    const size_t numTextures;

  public:
    ComputeProgram(const ShaderSource &source, size_t numTextures);
    ComputeProgram(const ShaderPath &path, size_t numTextures);
    ~ComputeProgram();

    /// set the dimension and value of a param
    ///
    /// NOTE: even if some texture is output-only, a call to setValue is still
    /// needed
    /// @param value set it to nullptr if no value is needed
    void setValue(unsigned int index, const float *value, GLsizei width,
                  GLsizei height, GLenum internalFormat,
                  GLenum dataFormat) const;

    void getValue(unsigned int index, GLfloat *value, GLenum dataFormat) const;

    /// generate and bind textures, and dispatch compute
    /// @param data the pointer to store output
    /// @param numGroups the number of groups to dispatch
    void dispatchCompute(const glm::ivec3 &numGroups) const;
};