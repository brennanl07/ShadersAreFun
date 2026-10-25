#pragma once

#include <glad/glad.h>

class ClientGLProgram
{
private:
    inline static GLint activeProgramId = -1;

protected:
    GLint programId;

public:
    void use();

    bool is_active_program() { return programId == activeProgramId; }

    GLint get_active_program_id() { return activeProgramId; }
};