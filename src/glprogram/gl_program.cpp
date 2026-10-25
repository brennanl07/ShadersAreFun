#include "glprogram/gl_program.hpp"

void ClientGLProgram::use()
{
    ClientGLProgram::activeProgramId = programId;
    glUseProgram(programId);
}