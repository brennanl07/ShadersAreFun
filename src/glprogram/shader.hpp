#pragma once

#include <array>
#include <iostream>
#include <map>
#include <string>
#include <filesystem>

#include <glad/glad.h>

#include "glm/gtc/type_ptr.hpp"
#include "glprogram/gl_program.hpp"

namespace fs = std::filesystem;

#define SHADER_TYPES \
    X(COMPUTE)   \
    X(GEOMETRY)  \
    X(FRAGMENT)  \
    X(VERTEX)    \

#define X(type) type = GL_ ## type ## _SHADER,
enum class ShaderType
{
    SHADER_TYPES
};
#undef X


struct ShaderInfo
{
    ShaderType type;
    std::string path;

    ShaderInfo(ShaderType type, std::string path);
};

class Shader : public ClientGLProgram
{
    // To make c++ shut up.
public: struct Uniform;
private:
    static inline bool s_initialized = false;

    std::map<std::string, Uniform> namedUniforms;

    
    /**
     * @brief Initializes a map of uniform names to their internal locations,
     *        to reduce observation of OpenGL state
     */
    void initialize_uniforms();

    /**
    * @brief Compiles shader code at a given path.
    * s
    * @param shaderType Any shader type
    * @param path Path to shader file
    * @note Variadic functions cannot include non-trivial types (such as const std::string&), so this uses a C string
    * @return GLuint Location of compiled shader
    */
    static GLuint compile_shader_from_path(ShaderType shaderType, const char* path);
    // static void add_all_shader_include_strings();
    

public:
    struct Uniform
    {
        GLint location;
        GLenum type;
    };


    static const inline fs::path shaderIncludePath = "shaders/include";

    // stringize the types as an array
    #define X(type) std::string(#type),
    static const inline std::array shaderTypeStrings{ SHADER_TYPES };
    #undef X
  

    // might be useful if shaders will be created outside of this class
    // Shader(GLuint program);

    // reads and compiles the shader
    Shader(std::initializer_list<ShaderInfo> shaderInfos);


    // utility uniform functions
    /**
     * @brief Get the GL location of a named uniform
     * 
     * @param name Name of the uniform
     * @return GLint The location of the uniform
     */
    Uniform get_uniform_from_name(const std::string& name) const;

    /**
     * @brief Returns true if the provided program ID is a shader
     * @remark Non-trivial processing time. GLFW state is not effeciently observed
     * 
     * @param programID
     */
    static bool is_shader(GLuint programID);

    /**
     * @brief Get the name of the inputted type of shader
     * 
     * @param shaderType Any shader type
     * @return std::string ShaderType stringized at preprocessing time
     */
    static const std::string& get_shader_type_string(ShaderType shaderType);


    template <typename T>
    void set_uniform(const std::string &name, T value)
    {
        Shader::Uniform uniform = this->get_uniform_from_name(name);

        GLenum valueGlType = 0;

        // For some reason, C++ makes me want to write code like this.
        // If the inputted value matches the type "matchType", check if the corresponding
        //  GLenum type matches the cached uniform type (associated with the inputted name),
        //  then set the uniform using a GLAD macro and store the corresponding GLenum type
#define SET_FROM_TYPE_MATCH(matchType, glType, macroType, finalValue, ...) \
        if constexpr(std::is_same_v<T, matchType>) { if (glType == uniform.type) \
            { glUniform ## macroType ## v (uniform.location, 1, __VA_ARGS__ finalValue); valueGlType = glType; } }

#define SET_1_FROM_TYPE_MATCH(matchType, glType, macroType) \
    SET_FROM_TYPE_MATCH(matchType, glType, macroType, value)
#define SET_V_FROM_TYPE_MATCH(matchType, glType, macroType) \
    SET_FROM_TYPE_MATCH(matchType, glType, macroType, glm::value_ptr(value))
#define SET_M_FROM_TYPE_MATCH(matchType, glType, macroType) \
    SET_FROM_TYPE_MATCH(matchType, glType, macroType, glm::value_ptr(value), false,)

        SET_1_FROM_TYPE_MATCH(     GLfloat,   GL_FLOAT,       1f)
        else SET_V_FROM_TYPE_MATCH(glm::vec2, GL_FLOAT_VEC2,  2f)
        else SET_V_FROM_TYPE_MATCH(glm::vec3, GL_FLOAT_VEC3,  3f)
        else SET_V_FROM_TYPE_MATCH(glm::vec4, GL_FLOAT_VEC4,  4f)
        // else SET_1_FROM_TYPE_MATCH(GLdouble,   GL_DOUBLE,       1d)
        // else SET_V_FROM_TYPE_MATCH(glm::dvec2, GL_DOUBLE_VEC2,  2d)
        // else SET_V_FROM_TYPE_MATCH(glm::dvec3, GL_DOUBLE_VEC3,  3d)
        // else SET_V_FROM_TYPE_MATCH(glm::dvec4, GL_DOUBLE_VEC4,  4d)
        else SET_1_FROM_TYPE_MATCH(GLint,        GL_INT,       1i)
        else SET_V_FROM_TYPE_MATCH(glm::ivec2,   GL_INT_VEC2,  2i)
        else SET_V_FROM_TYPE_MATCH(glm::ivec3,   GL_INT_VEC3,  3i)
        else SET_V_FROM_TYPE_MATCH(glm::ivec4,   GL_INT_VEC4,  4i)
        else SET_1_FROM_TYPE_MATCH(GLuint,     GL_UNSIGNED_INT,      1ui)
        else SET_V_FROM_TYPE_MATCH(glm::uvec2, GL_UNSIGNED_INT_VEC2, 2ui)
        else SET_V_FROM_TYPE_MATCH(glm::uvec3, GL_UNSIGNED_INT_VEC3, 3ui)
        else SET_V_FROM_TYPE_MATCH(glm::uvec4, GL_UNSIGNED_INT_VEC4, 4ui)
        else SET_1_FROM_TYPE_MATCH(GLboolean,   GL_BOOL,       1i)
        else SET_V_FROM_TYPE_MATCH(glm::bvec2,  GL_BOOL_VEC2,  2i)
        else SET_V_FROM_TYPE_MATCH(glm::bvec3,  GL_BOOL_VEC3,  3i)
        else SET_V_FROM_TYPE_MATCH(glm::bvec4,  GL_BOOL_VEC4,  4i)
        else SET_M_FROM_TYPE_MATCH(glm::mat2,  GL_FLOAT_MAT2,  Matrix2f)
        else SET_M_FROM_TYPE_MATCH(glm::mat3,  GL_FLOAT_MAT3,  Matrix3f)
        else SET_M_FROM_TYPE_MATCH(glm::mat4,  GL_FLOAT_MAT4,  Matrix4f)
        else SET_M_FROM_TYPE_MATCH(glm::mat2x3, GL_FLOAT_MAT2x3, Matrix2x3f)
        else SET_M_FROM_TYPE_MATCH(glm::mat2x4, GL_FLOAT_MAT2x4, Matrix2x4f)
        else SET_M_FROM_TYPE_MATCH(glm::mat3x2, GL_FLOAT_MAT3x2, Matrix3x2f)
        else SET_M_FROM_TYPE_MATCH(glm::mat3x4, GL_FLOAT_MAT3x4, Matrix3x4f)
        else SET_M_FROM_TYPE_MATCH(glm::mat4x2, GL_FLOAT_MAT4x2, Matrix4x2f)
        else SET_M_FROM_TYPE_MATCH(glm::mat4x3, GL_FLOAT_MAT4x3, Matrix4x3f)
        
#undef SET_FROM_TYPE_MATCH
#undef SET1_FROM_TYPE_MATCH
#undef SETV_FROM_TYPE_MATCH

        if (valueGlType != uniform.type)
        {
            std::cerr << "Uniform type does not match inputted type \"" << typeid(T).name() << "\"!";
        }
    }
};

class PipelineShader
{
private:
    Shader pipelineShader;

public:
    PipelineShader(const std::string& vertexPath, const std::string& fragmentPath);

    Shader get_shader() { return pipelineShader; };
};

class ComputeShader
{
private:
    Shader computeShader;

public:
    ComputeShader(const std::string& computePath);

    Shader get_shader() { return computeShader; };

    void dispatch_indirect();
    void dispatch(int32_t x = 1, int32_t y = 1, int32_t z = 1);
};