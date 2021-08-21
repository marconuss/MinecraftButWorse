#include "DirectionalLight.h"


CDirectionalLight::CDirectionalLight() : CLight()
{
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
}

CDirectionalLight::CDirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight, 
										GLfloat red, GLfloat green, GLfloat blue,
										GLfloat aIntensity, GLfloat dIntensity,
										GLfloat xDir, GLfloat yDir, GLfloat zDir) : CLight(shadowWidth, shadowHeight, red, green, blue, aIntensity, dIntensity)
{
	direction = glm::vec3(xDir, yDir, zDir);
	//orthogonal perspective for the light projection
	lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

void CDirectionalLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
	GLuint diffuseIntensityLocation, GLuint directionLocation)
{
	//bind the color and intensity to the light ID
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform3f(ambientColorLocation, color.x, color.y, color.z);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

glm::mat4 CDirectionalLight::CalculateLightTransform()
{
	// projection * view
	return lightProj * glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

CDirectionalLight::~CDirectionalLight()
{
}
