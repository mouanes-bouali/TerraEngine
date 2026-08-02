// Shader.h
#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    // Constructor reads and builds the shader from file paths
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader() { glDeleteProgram(ID); }

    // Activate the shader program
    void use() const { glUseProgram(ID); }

    // Hot-reload: re-reads the source files, recompiles, and relinks.
    // Returns true on success, false on failure (keeps old program on failure).
    bool reload();

    // Uniform setters (cached locations)
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &value) const;

private:
    std::string m_vertexPath;
    std::string m_fragmentPath;
    mutable std::unordered_map<std::string, GLint> m_uniformCache;

    // Compile a single shader stage from source code
    GLuint compileStage(GLenum type, const std::string& source, const std::string& typeName);

    // Link vertex + fragment shaders into a program
    GLuint linkProgram(GLuint vertex, GLuint fragment);

    // Get (and cache) a uniform location
    GLint getUniformLocation(const std::string& name) const;

    // Helper to check compilation/linking errors
    void checkCompileErrors(unsigned int shader, const std::string& type);
};