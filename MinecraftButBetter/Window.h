#pragma once

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class CWindow
{
public:

	CWindow();
	CWindow(GLint windowWidth, GLint windowHeight);

	int Initialize();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	//not really the proper way, but good enough for this course
	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~CWindow();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;

	//1024 is the number of ASCII characters
	bool keys[1024];

	//mouse position variables
	GLfloat lastX, lastY, xChange, yChange;
	bool mouseFirstMoved;

	void createCallbacks();

	//calback function 
	//I have to define all parameters to make the function work, 
	//but I'm going to use only key and action
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);

};

