//
// Created by lzt on 2021/3/27.
//

#ifndef OPENGL_1_SHADER_H
#define OPENGL_1_SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    unsigned int ID;

    /**
     * constructor generates the shader on the fly
     * @param vertexPath
     * @param fragmentPath
     */
    Shader(const char *vertexPath, const char *fragmentPath);

    /**
     * activate the shader
     */
    void use();

    /**
     * utility uniform functions
     * @param name
     * @param value
     */
    void setBool(const std::string &name, bool value) const;

    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const;

    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w);

    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const;


private:
    /**
     * utility function for checking shader compilation/linking errors.
     * @param shader
     * @param type
     */
    void checkCompileErrors(unsigned int shader, std::string type);

    /**
     * read Shader source from .glsl files
     * @param filePath
     * @return
     */
    std::string readShaderSource(const char *filePath);
};


#endif //OPENGL_1_SHADER_H
