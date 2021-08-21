#include "Window.h"

CWindow::CWindow()
{
	width = 800; 
	height = 600;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}

	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMoved = false;
}

CWindow::CWindow(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}

	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMoved = false;
}

int CWindow::Initialize()
{
	//initialize GLFW 
	if (!glfwInit())
	{
		printf("GLFW failed to initialize");
		glfwTerminate();
		return 1;
	}

	//Setup GLFW window properties
	//OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//core profile trows an error if backwards compatibility isn't working anymnore
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	//get Buffer size info
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	//handle keyboard inputs
	createCallbacks();
	//hide cursor
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//allow modern extension feature
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//Depth test
	glEnable(GL_DEPTH_TEST);

	//setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	//specifies the window where the calback handleKeys will happen
	glfwSetWindowUserPointer(mainWindow, this);
}

void CWindow::createCallbacks()
{
	//when a key is pressed on the main window check for it
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat CWindow::getXChange()
{
	GLfloat posChange = xChange;
	xChange = 0.0f;
	return posChange;
}

GLfloat CWindow::getYChange()
{
	GLfloat posChange = yChange;
	yChange = 0.0f;
	return posChange;
}

void CWindow::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	CWindow* myWindow = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
	
	//if the escape key is pressed close the window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		//when the user presses the key, key is true
		if (action == GLFW_PRESS)
		{
			myWindow->keys[key] = true;
			//printf("presses: %d\n", key);
		}
		//false whe the user releases
		else if (action == GLFW_RELEASE)
		{
			myWindow->keys[key] = false;
			//printf("released: %d\n", key);
		}
	}
}

void CWindow::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	CWindow* myWindow = static_cast<CWindow*>(glfwGetWindowUserPointer(window));

	if (myWindow->mouseFirstMoved)
	{
		myWindow->lastX = xPos;
		myWindow->lastY = yPos;
		myWindow->mouseFirstMoved = false;
	}
	myWindow->xChange = xPos - myWindow->lastX;
	// I like the pitch to be inverted when looking up and down
	myWindow->yChange = myWindow->lastY - yPos;
	
	myWindow->lastX = xPos;
	myWindow->lastY = yPos;

}


CWindow::~CWindow()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
