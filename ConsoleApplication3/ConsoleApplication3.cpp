// ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <GL/glew.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Window.h"

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection;

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
static const char* vShader = "                                   \n\
#version 330                                                     \n\
layout (location = 0) in vec3 pos;                               \n\
uniform mat4 model;										         \n\
uniform mat4 projection;										 \n\
out vec4 vCol;  												 \n\
void main()                                                      \n\
{                                                                \n\
	gl_Position = projection * model * vec4(pos,1.0);			 \n\
	vCol = vec4(clamp(pos,0.0f,1.0f), 1.0f);					 \n\
}";

// Fragment shader
static const char* fShader = "                                   \n\
#version 330                                                     \n\
out vec4 color;						                             \n\
in vec4 vCol;  												     \n\
void main()                                                      \n\
{                                                                \n\
	color = vCol;												 \n\
}";

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
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main()
{
	// initialize window
	Window myWindow = Window(800,600);
	myWindow.Initialise();
	//if (!glfwInit()) {
	//	printf("GLFW initialization failed");
	//	glfwTerminate();
	//	return 1;
	//}

	//// set up glfw window
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//// core profile = No Backwards Compatibility
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//// allow forward compatible
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", nullptr, nullptr);
	//if (!mainWindow) {
	//	printf("GLFW window creation failed");
	//	glfwTerminate();
	//	return 1;
	//}

	//int bufferWidth, bufferHeight;
	//glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//// set context for GLEW to use
	//glfwMakeContextCurrent(mainWindow);

	//// allow modern extensions features
	//glewExperimental = GL_TRUE;

	//if (glewInit() != GLEW_OK) {
	//	printf("GLEW initialization failed!");
	//	glfwDestroyWindow(mainWindow);
	//	glfwTerminate();
	//}

	//// Set up veiwport size
	//glEnable(GL_DEPTH_TEST);
	//glViewport(0, 0, bufferWidth, bufferHeight);
	int bufferWidth = myWindow.getBufferWidth();
	int bufferHeight = myWindow.getBufferHeight();
	createTriangle();
	compileShaders();
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

		glUseProgram(shader);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		

		glUseProgram(0);
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
