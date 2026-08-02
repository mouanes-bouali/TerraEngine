#include "Shaders.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
    : m_vertexPath(vertexPath), m_fragmentPath(fragmentPath)
{
    // 1. Read source files
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vStream, fStream;
        vStream << vShaderFile.rdbuf();
        fStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode   = vStream.str();
        fragmentCode = fStream.str();
    } catch(std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << vertexPath << " or " << fragmentPath << std::endl;
    }

    // 2. Compile shaders
    GLuint vertex = compileStage(GL_VERTEX_SHADER, vertexCode, "VERTEX");
    GLuint fragment = compileStage(GL_FRAGMENT_SHADER, fragmentCode, "FRAGMENT");

    // 3. Link program
    ID = linkProgram(vertex, fragment);

    // 4. Clean up shader objects
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

GLuint Shader::compileStage(GLenum type, const std::string& source, const std::string& typeName)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, typeName);
    return shader;
}

GLuint Shader::linkProgram(GLuint vertex, GLuint fragment)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkCompileErrors(program, "PROGRAM");
    return program;
}

bool Shader::reload()
{
    // Read source files
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(m_vertexPath);
        fShaderFile.open(m_fragmentPath);
        std::stringstream vStream, fStream;
        vStream << vShaderFile.rdbuf();
        fStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode   = vStream.str();
        fragmentCode = fStream.str();
    } catch(std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::RELOAD::FILE_NOT_READ: " << m_vertexPath << " or " << m_fragmentPath << std::endl;
        return false;
    }

    // Compile new stages
    GLuint vertex = compileStage(GL_VERTEX_SHADER, vertexCode, "VERTEX");
    GLuint fragment = compileStage(GL_FRAGMENT_SHADER, fragmentCode, "FRAGMENT");

    // Link new program
    GLuint newProgram = linkProgram(vertex, fragment);

    // Clean up stages
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Verify the new program is valid before swapping
    GLint status = 0;
    glGetProgramiv(newProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        std::cerr << "ERROR::SHADER::RELOAD::LINK_FAILED — keeping old program\n";
        glDeleteProgram(newProgram);
        return false;
    }

    // Swap: delete old program, use new one
    glDeleteProgram(ID);
    ID = newProgram;

    // Invalidate uniform cache (locations may have changed)
    m_uniformCache.clear();

    std::cout << "Shader hot-reloaded: " << m_vertexPath << " + " << m_fragmentPath << "\n";
    return true;
}

GLint Shader::getUniformLocation(const std::string& name) const
{
    auto it = m_uniformCache.find(name);
    if (it != m_uniformCache.end())
        return it->second;

    GLint loc = glGetUniformLocation(ID, name.c_str());
    m_uniformCache[name] = loc;
    return loc;
}

void Shader::checkCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];
    if (type == "PROGRAM") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    } else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    }
}

// Uniform setters (cached locations)
void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(getUniformLocation(name), (int)value);
}
void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}
void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}