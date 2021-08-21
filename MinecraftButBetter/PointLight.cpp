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

CPointLight::CPointLight(GLuint shadowWidth, GLuint shadowHeight,
							GLfloat near, GLfloat far, 
							GLfloat red, GLfloat green, GLfloat blue,
							GLfloat aIntensity, GLfloat dIntensity,
							GLfloat xPos, GLfloat yPos, GLfloat zPos,
							GLfloat con, GLfloat lin, GLfloat exp) : CLight(shadowWidth, shadowHeight, red, green, blue, aIntensity, dIntensity)
{
	position = glm::vec3(xPos, yPos, zPos);
	constant = con;
	linear = lin;
	exponent = exp;

	farPlane = far;
	// aspect ratio calculation 
	// in order to have a cube ideally the aspet should be 1
	// that means shadowHight == shadowWidth
	float aspect = (float)shadowWidth / (float)shadowHeight;

	lightProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

	shadowMap = new OmniShadowMap();
	shadowMap->Init(shadowWidth, shadowHeight);
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

std::vector<glm::mat4> CPointLight::CalculateLightTransform()
{
	//setup light transform matrices 

	std::vector<glm::mat4> lightMatrices;
	// +x, -x
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
																	   
	// +y, -y														   
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
																	   
	// +z, -z														   
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	
	return lightMatrices;
}

GLfloat CPointLight::GetFarPlane()
{
	return farPlane;
}

glm::vec3 CPointLight::GetPosition()
{
	return position;
}

CPointLight::~CPointLight()
{
}
