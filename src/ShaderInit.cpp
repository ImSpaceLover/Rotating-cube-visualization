#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

static std::string ParseShader(const std::string filepath) {

    std::ifstream stream(filepath);

    std::string line;
    std::stringstream strstream;

    while (getline(stream, line)) {
        strstream << line << "\n";
    }

    return strstream.str();
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Error while compiling " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader" << "\n";
        std::cout << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vertsh = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fragsh = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vertsh);
    glAttachShader(program, fragsh);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertsh);
    glDeleteShader(fragsh);

    return program;
}