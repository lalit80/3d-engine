#include "Shader.h"
#include "Logger.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

Shader::Shader(const char* vertexPath,
               const char* fragmentPath,
               const char* geometryPath,
               const char* tessControlPath,
               const char* tessEvalPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::string tessControlCode;
    std::string tessEvalCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    std::ifstream tcShaderFile;
    std::ifstream teShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Vertex + Fragment
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vStream, fStream;
        vStream << vShaderFile.rdbuf();
        fStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vStream.str();
        fragmentCode = fStream.str();

        // Geometry
        if (geometryPath)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gStream;
            gStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gStream.str();
        }

        // Tess Control
        if (tessControlPath)
        {
            tcShaderFile.open(tessControlPath);
            std::stringstream tcStream;
            tcStream << tcShaderFile.rdbuf();
            tcShaderFile.close();
            tessControlCode = tcStream.str();
        }

        // Tess Evaluation
        if (tessEvalPath)
        {
            teShaderFile.open(tessEvalPath);
            std::stringstream teStream;
            teStream << teShaderFile.rdbuf();
            teShaderFile.close();
            tessEvalCode = teStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        LOG_ERROR("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: %s", e.what());
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    unsigned int geometry = 0;
    unsigned int tessControl = 0;
    unsigned int tessEval = 0;

    if (geometryPath)
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    if (tessControlPath)
    {
        const char* tcShaderCode = tessControlCode.c_str();
        tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tessControl, 1, &tcShaderCode, nullptr);
        glCompileShader(tessControl);
        checkCompileErrors(tessControl, "TESS_CONTROL");
    }

    if (tessEvalPath)
    {
        const char* teShaderCode = tessEvalCode.c_str();
        tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tessEval, 1, &teShaderCode, nullptr);
        glCompileShader(tessEval);
        checkCompileErrors(tessEval, "TESS_EVALUATION");
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    if (geometryPath)
        glAttachShader(ID, geometry);
    if (tessControlPath)
        glAttachShader(ID, tessControl);
    if (tessEvalPath)
        glAttachShader(ID, tessEval);

    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (geometryPath)
        glDeleteShader(geometry);
    if (tessControlPath)
        glDeleteShader(tessControl);
    if (tessEvalPath)
        glDeleteShader(tessEval);
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::unuse()
{
    glUseProgram(0);
}

unsigned int Shader::getID() const
{
    return ID;
}

// ------------------- Uniforms -------------------

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------- Error Checking -------------------

void Shader::checkCompileErrors(GLuint shader, const std::string& type)
{
    GLint success;
    GLint infoLogLength = 0;

    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0)
            {
                char* infoLog = (char*)malloc(infoLogLength);
                glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);

                LOG_ERROR("ERROR::SHADER_COMPILATION_ERROR (%s)\n%s", type.c_str(), infoLog);

                free(infoLog);
            }
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0)
            {
                char* infoLog = (char*)malloc(infoLogLength);
                glGetProgramInfoLog(shader, infoLogLength, nullptr, infoLog);

                LOG_ERROR("ERROR::PROGRAM_LINKING_ERROR\n%s", infoLog);

                free(infoLog);
            }
        }
    }
}
