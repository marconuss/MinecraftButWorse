#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "ShadowMap.h"

class CLight
{
public:
	CLight();
	CLight(GLfloat shadowWidth, GLfloat shadowHeight,
			GLfloat red, GLfloat green, GLfloat blue, 
			GLfloat aIntensity, GLfloat dIntensity);

	CShadowMap* GetShadowMap() { return shadowMap; };

	~CLight();

//private:
protected:
	glm::vec3 color;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;

	glm::mat4 lightProj;

	CShadowMap* shadowMap; 

	//glm::vec3 direction;
};

 