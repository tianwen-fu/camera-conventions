#include "GLProgram.h"

#include "GLStructures.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

constexpr const char *toString(const ShaderType &type) {
    switch (type) {
    case ShaderType::VertexShader:
        return "Vertex Shader";
    case ShaderType::FragmentShader:
        return "Fragment Shader";
    case ShaderType::ComputeShader:
        return "Compute Shader";
    }
    return nullptr; // make -Wall happy
}

static std::string readFile(const std::string_view &filePath) {
    std::ifstream file((filePath.data()));
    if (!file.is_open())
        throw std::runtime_error(std::string("Could not open file") +
                                 filePath.data());
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint loadShader(const char *source, const ShaderType type) {
    const auto shader = glCreateShader(static_cast<GLenum>(type));
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        std::unique_ptr<char> msg(new char[1024]);
        glGetShaderInfoLog(shader, 1024, nullptr, msg.get());
        std::clog << "Cannot compile " << toString(type) << ":" << msg.get()
                  << std::endl;
        throw std::runtime_error("Shader compilation failed");
    }

    return shader;
}

static void
initializeGLProgram(const std::vector<ShaderSource> &shaderSources,
                    const GLuint handle,
                    std::unordered_set<std::string> &attributeNames) {
    if (!handle)
        throw std::runtime_error("Failed to create program");
    std::set<ShaderType> typesLoaded;
    std::vector<GLuint> shaderIDs;
    for (const auto &[type, source] : shaderSources) {
        if (typesLoaded.find(type) != typesLoaded.end())
            throw std::runtime_error("Shader type already exists");
        const auto shader = loadShader(source, type);
        glAttachShader(handle, shader);
        typesLoaded.insert(type);
        shaderIDs.push_back(shader);
    }
    glLinkProgram(handle);

    GLint status;
    glGetProgramiv(handle, GL_LINK_STATUS, &status);
    if (!status) {
        const std::unique_ptr<char> msg(new char[1024]);
        glGetProgramInfoLog(handle, 1024, nullptr, msg.get());
        std::clog << "Program linking failed: " << msg.get() << std::endl;
        throw std::runtime_error("Program linking failed");
    }

    for (const auto shader : shaderIDs) {
        glDeleteShader(shader);
    }

    std::clog << "Active attributes:" << std::endl;
    GLint count;
    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &count);
    for (GLint i = 0; i < count; i++) {
        std::unique_ptr<char[]> name(new char[256]);
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveAttrib(handle, i, 256, &length, &size, &type, name.get());
        std::clog << '\t' << name.get() << std::endl;
        attributeNames.insert(name.get());
    }
}

GLProgram::GLProgram(const std::vector<ShaderSource> &shaderSources)
    : handle(glCreateProgram()) {
    initializeGLProgram(shaderSources, handle, attributeNames);
}

GLProgram::GLProgram(const std::vector<ShaderPath> &shaderPaths)
    : handle(glCreateProgram()) {
    std::vector<std::string> sources; // temporarily hold the string ownerships
    sources.reserve(shaderPaths.size());
    std::vector<ShaderSource> shaderSources;
    shaderSources.reserve(shaderPaths.size());

    for (const auto &[type, path] : shaderPaths) {
        sources.emplace_back(readFile(path));
        shaderSources.emplace_back(ShaderSource{type, sources.back().c_str()});
    }

    initializeGLProgram(shaderSources, handle, attributeNames);

    std::clog << "Linked program with shaders:" << std::endl;
    for (const auto &[type, path] : shaderPaths) {
        std::clog << '\t' << toString(type) << " at " << path << std::endl;
    }
}

void GLProgram::use() const { glUseProgram(handle); }

GLint GLProgram::locateVariable(const char *attributeName,
                                const ShaderVariableType type) const {
    GLint location;
    switch (type) {
    case ShaderVariableType::Attribute:
        location = glGetAttribLocation(handle, attributeName);
        break;
    case ShaderVariableType::Uniform:
        location = glGetUniformLocation(handle, attributeName);
        break;
    default:
        throw std::runtime_error("Unknown shader variable type");
    }
    if (location == -1)
        throw std::runtime_error(
            std::string("Failed to get attribute location for variable ") +
            attributeName);
    return location;
}

void GLProgram::setUniformScalar(const char *name, const int value) const {
    glUniform1i(locateVariable(name, ShaderVariableType::Uniform), value);
}

void GLProgram::setUniformScalar(const char *name, const float value) const {
    glUniform1f(locateVariable(name, ShaderVariableType::Uniform), value);
}
void GLProgram::setUniformScalar(const char *name, const int *value,
                                 const GLsizei count) const {
    glUniform1iv(locateVariable(name, ShaderVariableType::Uniform), count,
                 value);
}
void GLProgram::setUniformScalar(const char *name, const float *value,
                                 const GLsizei count) const {
    glUniform1fv(locateVariable(name, ShaderVariableType::Uniform), count,
                 value);
}

void GLProgram::setUniform2fv(const char *name, const float *value,
                              GLsizei count) const {
    glUniform2fv(locateVariable(name, ShaderVariableType::Uniform), count,
                 value);
}

void GLProgram::setUniform3fv(const char *name, const float *value,
                              const GLsizei count) const {
    int currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    assert(static_cast<decltype(handle)>(currentProgram) == handle);
    // count set to 1 if the target variable is not an array
    glUniform3fv(locateVariable(name, ShaderVariableType::Uniform), count,
                 value);
}

void GLProgram::setUniform4fv(const char *name, const float *value,
                              GLsizei count) const {
    glUniform4fv(locateVariable(name, ShaderVariableType::Uniform), count,
                 value);
}

void GLProgram::setUniformMatrix4fv(const char *name, const float *value,
                                    const GLboolean transpose) const {
    glUniformMatrix4fv(locateVariable(name, ShaderVariableType::Uniform), 1,
                       transpose, value);
}

void GLProgram::validateAllAttributesSet(const VertexArray &vao) const {
    for (const auto &name : attributeNames) {
        if (!vao.isShaderVariableBound(name))
            throw std::runtime_error(std::string("Attribute ") + name +
                                     " not set in VAO");
    }
}

ComputeProgram::ComputeProgram(const ShaderSource &source,
                               const size_t numTextures)
    : GLProgram({source}), numTextures(numTextures) {
    assert(source.type == ShaderType::ComputeShader);
    textures = new GLuint[numTextures];
    glGenTextures(numTextures, textures);
}

ComputeProgram::ComputeProgram(const ShaderPath &path, const size_t numTextures)
    : GLProgram({path}), numTextures(numTextures) {
    assert(path.type == ShaderType::ComputeShader);
    textures = new GLuint[numTextures];
    glGenTextures(numTextures, textures);
}

ComputeProgram::~ComputeProgram() {
    glDeleteTextures(numTextures, textures);
    delete[] textures;
}

void ComputeProgram::setValue(const unsigned int index, const float *value,
                              const GLsizei width, const GLsizei height,
                              const GLenum internalFormat,
                              const GLenum dataFormat) const {
    // first see if we can get rid of all these constants
    static_assert(GL_TEXTURE0 + 1 == GL_TEXTURE1);
    static_assert(GL_TEXTURE0 + 2 == GL_TEXTURE2);
    static_assert(GL_TEXTURE0 + 3 == GL_TEXTURE3);
    static_assert(GL_TEXTURE0 + 4 == GL_TEXTURE4);
    // should be enough

    use();
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), width,
                 height, 0, dataFormat, GL_FLOAT, value);
    glBindImageTexture(index, textures[index], 0, GL_FALSE, 0, GL_READ_WRITE,
                       internalFormat);
}

void ComputeProgram::getValue(const unsigned int index, GLfloat *value,
                              const GLenum dataFormat) const {
    use();
    glActiveTexture(GL_TEXTURE0 + index);
    glGetTexImage(GL_TEXTURE_2D, 0, dataFormat, GL_FLOAT, value);
}

void ComputeProgram::dispatchCompute(const glm::ivec3 &numGroups) const {
    use();
    glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
