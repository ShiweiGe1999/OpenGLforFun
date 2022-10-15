#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <sstream>
class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	void createFromString(const char* vertexCode, const char* fragmentCode);
	GLuint getProjectionLocation();
	GLuint getModelLocation();
	void useShader();
	void clearShader();
private:
	GLuint shaderID, uniformProjection, uniformModel;
	void compileShader(const char* vertexCode, const char* fragmentCode);
	void addShader(GLuint program, const char* shaderCode, GLenum shaderType);
};

