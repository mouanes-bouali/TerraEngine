// Shader.h
#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
class Shader {
public:
    unsigned int ID;

    // Constructor reads and builds the shader from file paths
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader() { glDeleteProgram(ID); }

    void use() const { glUseProgram(ID); }

    // Uniform setters (matching the tutorial)
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &value) const;

private:
    // Helper to check compilation/linking errors
    void checkCompileErrors(unsigned int shader, const std::string& type);
};