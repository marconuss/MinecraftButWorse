#include "PointLight.h"

CPointLight::CPointLight() : CLight()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	// for the light calculation we will devide by the constant
	// setting it to 0.0f would be a problem, so its set to 1.0f
	constant = 1.0f;
	linear = 0.0f;
	exponent = 0.0f;
}

CPointLight::CPointLight(GLfloat red, GLfloat green, GLfloat blue,
							GLfloat aIntensity, GLfloat dIntensity,
							GLfloat xPos, GLfloat yPos, GLfloat zPos,
							GLfloat con, GLfloat lin, GLfloat exp) : CLight(1024, 1024, red, green, blue, aIntensity, dIntensity)
{
	position = glm::vec3(xPos, yPos, zPos);
	constant = con;
	linear = lin;
	exponent = exp;
}


void CPointLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, 
							GLuint diffuseIntensityLocation, GLuint positionLocation, 
							GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation)
{
	//bind the information to the light ID
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform3f(ambientColorLocation, color.x, color.y, color.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);

	glUniform3f(positionLocation, position.x, position.y, position.x);
	glUniform1f(constantLocation, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(exponentLocation, exponent);
}

CPointLight::~CPointLight()
{
}
