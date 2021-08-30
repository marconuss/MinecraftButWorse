#include "Skybox.h"


CSkybox::CSkybox()
{
}

CSkybox::CSkybox(std::vector<std::string> faceLocations)
{
	//create the shader for the skybox
	skyShader = new CShader();
	skyShader->CreateFromFiles("../Shaders/skyBox.vert", "../Shaders/skyBox.frag");

	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();

	//create cube map
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, bitDepth;

	for (size_t i = 0; i < 6; i++)
	{

		unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);

		if (!texData) {
			printf("Failed to find: %s\n", faceLocations[i].c_str());
			return;
		}
		//the face location needs to match the prefefined order as it is in GL_TEXTURE_CUBE_MAP
		//starting with positive X it goes: +x, -x, +y, -y, +z, -z
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//create the mesh for the cube

	unsigned int skyIndices[] = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	float skyVertices[] = {
		//uv and normals are left null as we don't need them for the skybox
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};


	skyMesh = new CMesh();
	skyMesh->CreateMesh(skyVertices, skyIndices, 64, 36);
}

void CSkybox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{

	//as the skybox shóuld't have any changes except rotation
	//I convert the viewMatrix to a 3x3 matrix cutting off the tranformations I don't need
	//but the view matrix needs to be 4x4 so I just convert it ba
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	//disable the depthMask before drawing the skybox
	glDepthMask(GL_FALSE);

	skyShader->UseShader();

	//attach projection and view matrix
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	//render the skybox 
	skyMesh->RenderMesh();

	//turn on the depth maska again 
	glDepthMask(GL_TRUE);

}

CSkybox::~CSkybox()
{
}
