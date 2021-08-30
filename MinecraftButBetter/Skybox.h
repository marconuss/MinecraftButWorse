#pragma once
#include <vector>
#include <string>

#include "GL/glew.h"

#include "CommonValues.h" 

//the skybox needs to be able to handle matrix uniforms
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"

class CSkybox
{

public:
	CSkybox();
	CSkybox(std::vector<std::string> faceLocations);

	void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);


	~CSkybox();

private:
	CMesh* skyMesh;
	CShader* skyShader;

	GLuint textureId;
	GLuint uniformProjection, uniformView;

};

