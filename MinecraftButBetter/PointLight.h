#pragma once
#include "Light.h"
#include "OmniShadowMap.h"

class CPointLight : public CLight
{
public:
	CPointLight();

	CPointLight(GLuint shadowWidth, GLuint shadowHeight,
				GLfloat near, GLfloat far,
				GLfloat red, GLfloat green, GLfloat blue, 
				GLfloat aIntensity, GLfloat dIntensity,
				GLfloat xPos, GLfloat yPos, GLfloat zPos, 
				GLfloat con, GLfloat lin, GLfloat exp);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
					GLuint diffuseIntensityLocation, GLuint positionLocation, 
					GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

	std::vector<glm::mat4> CalculateLightTransform();

	GLfloat GetFarPlane();
	glm::vec3 GetPosition();

	~CPointLight();

protected:
	glm::vec3 position;

	// control the attenuation of the light source
	// ax^2 + bx + c
	GLfloat constant, linear, exponent;

	GLfloat farPlane;
};

