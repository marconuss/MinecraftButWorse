#pragma once

#include <GL/glew.h>

class CMaterial
{
public:
	CMaterial();
	CMaterial(GLfloat sIntensity, GLfloat shine);

	void useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);
	~CMaterial();

private:
	GLfloat specularIntensity;
	GLfloat shininess;

};

