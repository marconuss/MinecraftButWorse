#pragma once
#include "Light.h"
#include "OmniShadowMap.h"

class CPointLight : public CLight
{
public:
	CPointLight();

	CPointLight(GLfloat red, GLfloat green, GLfloat blue, 
				GLfloat aIntensity, GLfloat dIntensity,
				GLfloat xPos, GLfloat yPos, GLfloat zPos, 
				GLfloat con, GLfloat lin, GLfloat exp);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
					GLuint diffuseIntensityLocation, GLuint positionLocation, 
					GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

	~CPointLight();

protected:
	glm::vec3 position;

	// control the attenuation of the light source
	// ax^2 + bx + c
	GLfloat constant, linear, exponent;

};

