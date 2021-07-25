#include "Material.h"

CMaterial::CMaterial()
{
	specularIntensity = 0.0f;
	shininess = 0.0f;
}

CMaterial::CMaterial(GLfloat sIntensity, GLfloat shine)
{
	specularIntensity = sIntensity;
	shininess = shine;
}

void CMaterial::useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);

}

CMaterial::~CMaterial()
{
}
