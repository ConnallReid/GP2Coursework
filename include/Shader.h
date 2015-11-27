#ifndef _Shader_h
#define _Shader_h

#include "Common.h"

enum SHADER_TYPE
{
	VERTEX_SHADER = GL_VERTEX_SHADER,
	FRAGMENT_SHADER = GL_FRAGMENT_SHADER
};

GLuint loadShaderFromMemory(const char * pMem, SHADER_TYPE shaderType);

GLuint loadShaderFromFile(const string& filename, SHADER_TYPE shaderType);

bool checkForCompilerErrors(GLuint shaderProgram);

bool checkForLinkErrors(GLuint program);

#endif