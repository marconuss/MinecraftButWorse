#include "Camera.h"

CCamera::CCamera()
{
}


CCamera::CCamera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{

	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void CCamera::keyControl(bool* keys, GLfloat deltaTime)
{

	GLfloat velocity = moveSpeed * deltaTime;
	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
	}
	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
	}
	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}
	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}
}

void CCamera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	if (xChange > 50 || yChange > 50) {
		xChange = 0;
		yChange = 0;
	}

	xChange *= turnSpeed;
	yChange *= turnSpeed;

	//looking left and right
	yaw += xChange;
	//looking up and down
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	update();
}

glm::vec3 CCamera::getCameraPosition()
{
	return position;
}

glm::vec3 CCamera::getCameraDirection()
{
	return glm::normalize(front);
}

glm::mat4 CCamera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}


void CCamera::update()
{

	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	//cross product of the front and the world Up
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

}

CCamera::~CCamera()
{
}