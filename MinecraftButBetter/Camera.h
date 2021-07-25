#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glfw/glfw3.h>

class CCamera
{
public: 
	CCamera();
	CCamera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat sartTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	//get the camera position to get specular lighting to work
	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();

	//calculate view matrix
	glm::mat4 calculateViewMatrix();
	~CCamera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;
	//GLfloat roll;

	GLfloat moveSpeed;
	GLfloat turnSpeed;
	
	void update();
};

