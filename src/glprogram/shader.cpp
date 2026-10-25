#include "shader.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>

#include "glad/glad.h"

ShaderInfo::ShaderInfo(ShaderType type, std::string path)
{
    this->type = type;
    this->path = path;
}

Shader::Shader(std::initializer_list<ShaderInfo> shaderInfos)
{
    // // this will only happen once, tell the compiler that this is verrry rare
    // if (!s_initialized) [[ unlikely ]]
    // {
    //     Shader::add_all_shader_include_strings();

    //     s_initialized = true;
    // }

    int16_t shaderCount = shaderInfos.size();
    GLuint shaderIds[16];
    for (int16_t i = 0; i < shaderCount; i++) {
        ShaderInfo shaderInfo = shaderInfos.begin()[i];
        shaderIds[i] = compile_shader_from_path(shaderInfo.type, shaderInfo.path.c_str());
    }

    // shader Program
    GLuint programId = glCreateProgram();
    for (int16_t i = 0; i < shaderCount; i++) {
        GLuint shaderId = shaderIds[i];
        glAttachShader(programId, shaderId);
    }
    glLinkProgram(programId);

    GLint success = 0;
    // BUG: When switching the vert path for the frag path and vice versa, this seg faults!
    //  Maybe glIsShader could prevent glGetProgramiv from segfaulting?
    //  (though this is on a very small scale, querying opengl state can lead to lower performance)
    glGetProgramiv(programId, GL_LINK_STATUS, &success);

    // Print linking errors if any
    if (!success)
    {
        char infoLog[2048];
        glGetProgramInfoLog(programId, 2048, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        exit(1);
    }
   
    // delete the shaders as they're linked into our program now and no longer necessary
    for (int16_t i = 0; i < shaderCount; i++) {
        GLuint shaderId = shaderIds[i];
        glDeleteShader(shaderId);
    }

    this->programId = programId;
    this->initialize_uniforms();
}

PipelineShader::PipelineShader(const std::string& vertexPath, const std::string& fragmentPath)
    : pipelineShader({
        ShaderInfo(ShaderType::VERTEX, vertexPath),
        ShaderInfo(ShaderType::FRAGMENT, fragmentPath)
    })
{}

ComputeShader::ComputeShader(const std::string& computePath) 
    : computeShader({
        ShaderInfo(ShaderType::COMPUTE, computePath)
    })
{}

void ComputeShader::dispatch_indirect()
{
    computeShader.use();
    glDispatchComputeIndirect(0);
}

void ComputeShader::dispatch(int32_t x, int32_t y, int32_t z)
{
    computeShader.use();
    glDispatchCompute(x, y, z);
}

GLuint Shader::compile_shader_from_path(ShaderType shaderType, const char* path)
{
    std::string code;
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        shaderFile.open(path);
        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into string
        code = shaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    const char* shaderCode = code.c_str();

    // compile shader
    uint shaderID = glCreateShader(static_cast<GLenum>(shaderType));
    glShaderSource(shaderID, 1, &shaderCode, NULL);

    // fs::path absoluteIncludePath = fs::absolute(shaderIncludePath);
    // const char* includePathStr = absoluteIncludePath.c_str();
    // const char* searchDirectories[] = { includePathStr };
    // glCompileShaderIncludeARB(shaderID, glm::countof(searchDirectories), searchDirectories, nullptr);
    glCompileShader(shaderID);

    // print compile errors if any
    int32_t success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        // int32_t infoLogLength;
        // glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

        // std::string infoLog;
        // if (infoLogLength)
        // {
        //     char infoLogBuffer[1024];
        //     glGetShaderInfoLog(shaderID, 1024, NULL, infoLogBuffer);
        //     infoLog = std::string(infoLogBuffer);
        // }
        // else
        // {
        //     infoLog = std::string("Info log was empty. Are you sure the shader was compiled?");
        // }

        char infoLogBuffer[1024];
        glGetShaderInfoLog(shaderID, 1024, NULL, infoLogBuffer);
        // std::string infoLog = std::string(infoLogBuffer);

        std::cerr << "ERROR::SHADER::" << Shader::get_shader_type_string(shaderType) << "::COMPILATION_FAILED" << "\n" << infoLogBuffer << std::endl;
        exit(1);
    }

    return shaderID;
}

void Shader::initialize_uniforms()
{
    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 64;
    // glGetProgramiv(programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufSize);
    // Buffer size doesn't matter here; one 64 byte array on the stack won't change memory usage 
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    GLint count;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &count);

    namedUniforms = {};
    for (GLint i = 0; i < count; i++)
    {
        glGetActiveUniform(programId, i, bufSize, &length, &size, &type, name);

        namedUniforms.insert({std::string(name), {i, type}});
    }
}

const std::string& Shader::get_shader_type_string(ShaderType shaderType)
{
    return Shader::shaderTypeStrings.at(static_cast<size_t>(shaderType));
}

// Include strings are unimplemented in this project so far cuz they're annoying :/
// void Shader::add_all_shader_include_strings()
// {
//     for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(shaderIncludePath))
//     {
//         const fs::path& path = dir_entry.path();

//         std::fstream file{path};
//         std::string line;
//         std::stringstream sourceStream{};
//         while (std::getline(file, line))
//         {
//             sourceStream << line << "\n";
//         }
//         const std::string& source = sourceStream.str();

//         // offset string to get a substring - "/directory_name/included.glsl"
//         u_int16_t offset = shaderIncludePath.string().length();
//         std::string includeString = std::string(offset + path.c_str());
//         glNamedStringARB(GL_SHADER_INCLUDE_ARB, includeString.length(), includeString.c_str(), source.length(), source.c_str());
//     }
// }


Shader::Uniform Shader::get_uniform_from_name(const std::string &name) const
{
    auto possibleLocation = namedUniforms.find(name);
    if (possibleLocation != namedUniforms.end())
    {
        return possibleLocation->second;
    }
    else
    {
        GLint location = glGetUniformLocation(programId, name.c_str());
        if (location < 0)
        {
            std::cerr << "Uniform with name \"" << name << "\" was not found in cache. Did you spell it wrong, or is this uniform unused?\n";
        }
        else
        {
            std::cout << "Uniform with name \"" << name << "\" was not found in cache. Fallback provided.\n";
        }
        return { location, 0 };
    }
}