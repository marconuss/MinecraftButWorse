#pragma once
#include "Light.h"

class CDirectionalLight: public CLight
{
public:
	CDirectionalLight();

	CDirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight, 
						GLfloat red, GLfloat green, GLfloat blue,
						GLfloat aIntensity, GLfloat dIntensity,
						GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
		GLuint diffuseIntensityLocation, GLuint directionLocation);

	glm::mat4 CalculateLightTransform();

	~CDirectionalLight();

private:
	glm::vec3 direction;

};

