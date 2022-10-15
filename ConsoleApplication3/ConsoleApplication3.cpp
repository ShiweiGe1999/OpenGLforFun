// ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <GL/glew.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Window.h"
#include "Mesh.h"
#include "Shader.h"

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint shader, uniformModel, uniformProjection;

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;


// Vertex shader
static const char* vShader = "Shaders/shader.vert.glsl";

// Fragment shader
static const char* fShader = "Shaders/shader.frag.glsl";

void addShader(GLuint program, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(eLog), nullptr, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType,eLog);
		return;
	}

	glAttachShader(program, theShader);
}

void compileShaders() {
	shader = glCreateProgram();
	if (!shader) {
		printf("Error creating shader program");
		return;
	}
	addShader(shader, vShader, GL_VERTEX_SHADER);
	addShader(shader, fShader, GL_FRAGMENT_SHADER);
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}
	uniformModel = glGetUniformLocation(shader, "model");
	uniformProjection = glGetUniformLocation(shader, "projection");

}

void createTriangle() {
	unsigned int indices[] = {
		0,3,1,
		1,3,2,
		2,3,0,
		0,1,2,
	};
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	Mesh* obj1 = new Mesh();
	obj1->createMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
	Shader* shader1 = new Shader(vShader, fShader);
	shaderList.push_back(shader1);
}

int main()
{
	// initialize window
	Window myWindow = Window(800,600);
	myWindow.Initialise();
	int bufferWidth = myWindow.getBufferWidth();
	int bufferHeight = myWindow.getBufferHeight();
	createTriangle();
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);
	while (!myWindow.getShouldClose()) {
		// get handle user input
		glfwPollEvents();

		if (direction) {
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}
		if (abs(triOffset) >= triMaxoffset) {
			direction = !direction;
		}

		curAngle += 0.01f;
		if (curAngle >= 360.0f) {
			curAngle -= 360.0f;
		}
		if (sizeDirection) {
			curSize += 1.0f;
		}
		else {
			curSize -= 1.0f;
		}
		if (curSize >= maxSize || curSize <= minSize) sizeDirection = !sizeDirection;

		// clear window
		glClearColor(0.0f,0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0]->useShader();

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		
		glUniformMatrix4fv(shaderList[0]->getModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(shaderList[0]->getProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));

		meshList[0]->renderMesh();

		shaderList[0]->clearShader();
		myWindow.swapBuffers();
	}
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
