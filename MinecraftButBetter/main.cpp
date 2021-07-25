#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Camera.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "Mesh.h"
#include "PerlinNoise.h"
#include "PointLight.h"
#include "Shader.h"
#include "SpotLight.h"
#include "Texture.h"
#include "Window.h"


const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

//window
CWindow mainWindow;

//shaders
//list of shaders for the scene itself
std::vector<CShader> shaderList;
//first pass for the dierctional shadow
CShader directionalShadowShader;
// first two passes for point lights and spot lights
CShader omniShadowShader;
//Vertex shaderID
static const char* vShader = "../Shaders/shader.vert";

//Fragment shaderID
static const char* fShader = "../Shaders/shader.frag";

//camera
CCamera camera;

//textures
CTexture minecraftTexture;
//materials
CMaterial shinyMaterial; 
CMaterial dullMaterial;

//lights
CDirectionalLight mainLight;
CPointLight pointLights[MAX_POINT_LIGHTS];
CSpotLight spotLights[MAX_SPOT_LIGHTS];
unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

//time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//cube 
CMesh* cube;
CMesh* floorMesh;

//perlinNoise size
unsigned int m_nWidth = 128;
unsigned int m_nHeight = 128;
unsigned char* m_pData;

//averaging the normals
//-------
//loop through the vertices of each triangle
//calculate the distance between the vertices
//get the cross product for the angle to find the normal
//normalize it 
// -----------
// loop through the normal vectors and fill in the normalized normals
void CalcAverageNormals(unsigned int* indices, unsigned int indicesCount, GLfloat* vertices, unsigned int verticesCount, 
						unsigned int vLenght, unsigned int normalOffset)
{
	//for loop jumps from triangle to trinagle 
	for (size_t i = 0; i < indicesCount; i+=3)
	{
		unsigned int in0 = indices[i] * vLenght;
		unsigned int in1 = indices[i+1] * vLenght;
		unsigned int in2 = indices[i+2] * vLenght;

		// v1 = in1.x - in0.x, in1.y - in0.y, in1.z - in0.z
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset, in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticesCount/vLenght; i++)
	{
		unsigned int nOffset = i * vLenght + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects() 
{

	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2, 
		2, 3, 0, 
		0, 1, 2
	};

	// the triangle has three vertices, each with three values
	// plus the textures uv coordinates
	GLfloat vertices[] = {
	//	x		y		z		u	  v		normals
		-0.0f,	-1.0f,	-0.6f,	0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
		0.0f,	-1.0f,	1.0f,	0.5f, 0.0f,	0.0f, 0.0f, 0.0f,
		1.0f,	-1.0f,	-0.6f,	1.0f, 0.0f,	0.0f, 0.0f, 0.0f,
		0.0f,	1.0f,	0.0f,	0.5f, 1.0f,	0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndeces[] = {
		0, 2, 1,
		1, 2, 3,
	};

	GLfloat floorVertices[] = {
		-10.0f,	0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f,	0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.f,		0.0f, -1.0f, 0.0f,
		10.0f,	0.0f, 10.0f,	10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	//define triangles counter-clockwise
	unsigned int cubeIndices[] = {

		//front
		0,  2,  3,		//bottom left > top left > top right
		0,  3,	1,		//bottom left > top right > bottom right
		// right
		4,  6,  7,
		4,  7,  5,		
		// back
		8,  10, 11,
		8, 11,  9,	
		// left
		12, 14, 15,
		12, 15, 13,	
		// top
		16, 18, 19,
		16, 19, 17,	
		// bottom
		20, 22, 23,
		20, 23, 21,
		
	};

	GLfloat cubeVertices[] =
	{
		// front
		//x		y		z		u	  v			normals
		0.0,	0.0,	1.0,	0.0, 0.33,		0.0, 0.0, -1.0, //bottom left
		1.0,	0.0,	1.0,	0.0, 0.66,		0.0, 0.0, -1.0,	//bottom right
		0.0,	1.0,	1.0,	0.25, 0.33,		0.0, 0.0, -1.0,	//top left
		1.0,	1.0,	1.0,	0.25, 0.66,		0.0, 0.0, -1.0,	//top right
		// right
		1.0,	0.0,	1.0,	0.25, 1.0,		-1.0, 0.0, 0.0,
		1.0,	0.0,	0.0,	0.5, 1.0,		-1.0, 0.0, 0.0,
		1.0,	1.0,	1.0,	0.25, 0.66,		-1.0, 0.0, 0.0,
		1.0,	1.0,	0.0,	0.5, 0.66,		-1.0, 0.0, 0.0,
		// back
		1.0,	0.0,	0.0,	0.75, 0.33,		0.0, 0.0, 1.0,			
		0.0,	0.0,	0.0,	0.75, 0.66,		0.0, 0.0, 1.0,
		1.0,	1.0,	0.0,	0.5, 0.33,		0.0, 0.0, 1.0,
		0.0,	1.0,	0.0,	0.5, 0.66,		0.0, 0.0, 1.0,
		// left
		0.0,	0.0,	0.0,	0.25, 0.0,		1.0, 0.0, 0.0,
		0.0,	0.0,	1.0,	0.5, 0.0,		1.0, 0.0, 0.0,
		0.0,	1.0,	0.0,	0.25, 0.33,		1.0, 0.0, 0.0,
		0.0,	1.0,	1.0,	0.5, 0.33,		1.0, 0.0, 0.0,
		//top
		0.0,	1.0,	1.0,	0.25, 0.66,		0.0, -1.0, 0.0,
		1.0,	1.0,	1.0,	0.5, 0.66,		0.0, -1.0, 0.0,
		0.0,	1.0,	0.0,	0.25, 0.33,		0.0, -1.0, 0.0,
		1.0,	1.0,	0.0,	0.5, 0.33,		0.0, -1.0, 0.0,
		// bottom
		0.0,	0.0,	1.0,	1.0, 0.66,		0.0, 1.0, 0.0,		
		1.0,	0.0,	1.0,	1.0, 0.33,		0.0, 1.0, 0.0,
		0.0,	0.0,	0.0,	0.75, 0.66,		0.0, 1.0, 0.0,
		1.0,	0.0,	0.0,	0.75, 0.33,		0.0, 1.0, 0.0,

	};

	CalcAverageNormals(cubeIndices, 36, cubeVertices, 192, 8, 5);


	floorMesh = new CMesh();
	floorMesh->CreateMesh(floorVertices, floorIndeces, 32, 6);

	cube = new CMesh();
	cube->CreateMesh(cubeVertices, cubeIndices, 192, 36);

}

void CreateShaders()
{
	CShader *shaderOne = new CShader();
	shaderOne->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shaderOne);

	directionalShadowShader = CShader();
	directionalShadowShader.CreateFromFiles("../Shaders/directional_shadow_map.vert", "../Shaders/directional_shadow_map.frag");
	omniShadowShader = CShader();
	omniShadowShader.CreateFromFiles("../Shaders/omni_shadow_map.vert", "../Shaders/omni_shadow_map.geom",  "../Shaders/omni_shadow_map.frag");

}

void RenderScene()
{

	glm::mat4 model(1.0f);

	// render one cube
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, -0.0f, -2.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//
	//minecraftTexture.UseTexture();
	//shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
	//cube->RenderMesh();

	//render perlin noise map

	for (unsigned int x = 0; x < m_nWidth; x++) {
		for (unsigned int z = 0; z < m_nHeight; z++) {
			//glPushMatrix();

			model = glm::mat4(1.0f);

			//glTranslatef(-0.5f + (float)x - (float)m_nWidth / 2.0f, -2.5f - (float)m_pData[z * m_nHeight + x], -0.5f - float(z) + (float)m_nHeight / 2.0f);
			model = glm::translate(model, glm::vec3(-0.5f + (float)x - (float)m_nWidth / 2.0f, -2.5f - (float)m_pData[z * m_nHeight + x], -0.5f - float(z) + (float)m_nHeight / 2.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			minecraftTexture.UseTexture();
			shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
			cube->RenderMesh();
			//glPopMatrix();
		}
	}

	//glPopMatrix();
	//glFlush();

}



void DirectionalShadowMapPass(CDirectionalLight* light)
{
	directionalShadowShader.UseShader();

	//add the shadow map to the main light
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	//clear all te depth buffer information
	glClear(GL_DEPTH_BUFFER_BIT);

	//projection and view calculation 
	uniformModel = directionalShadowShader.GetModelLocation();
	glm::mat4 lightTransform = light->CalculateLightTransform();
	directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(CPointLight* light)
{
	omniShadowShader.UseShader();

	//add the shadow map to the main light
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	//clear all te depth buffer information
	glClear(GL_DEPTH_BUFFER_BIT);

	//projection and view calculation 
	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glViewport(0, 0, 1366, 768);

	//clear window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount);
	shaderList[0].SetSpotLights(spotLights, spotLightCount);
	glm::mat4 mainLightTransform = mainLight.CalculateLightTransform();
	shaderList[0].SetDirectionalLightTransform(&mainLightTransform);

	//GL_TEXTURE0 already has a texture, so we want to use GL_TEXTURE1
	mainLight.GetShadowMap()->Read(GL_TEXTURE1);
	//just put the number of the texture unit, by default it is 0 already 
	shaderList[0].SetTexture(0);
	shaderList[0].SetDirectionalShadowMap(1);


	// use the spotlight as a flash light
	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	RenderScene();
}

int main()
{
	mainWindow = CWindow(1366, 768);
	mainWindow.Initialize();

	CreateObjects();
	CreateShaders();

	camera = CCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	//load textures to memory
	//cube texture
	minecraftTexture = CTexture("../Textures/cube2.png");
	minecraftTexture.LoadTextureAlpha();

	shinyMaterial = CMaterial(4.0f, 256);
	dullMaterial = CMaterial(0.1f, 2);

	//main directional light: 

	mainLight = CDirectionalLight(2048, 2048,			//shadowbuffer
									1.0f, 1.0f, 1.0f,	//color
									0.2f, 0.7f,			//ambientIntensity, diffuseIntensity
									0.0f, -20.0f, -15.0f); //direction

	
	//point lights: 
	// 
	//pointLights[0] = CPointLight(1024, 1024,			//shadowWidth, shadowHeight
	//								0.01f, 100,			//near far plane
	//								0.0f, 0.0f, 1.0f,	//color 
	//								0.0f, 0.1f,			//ambientIntensity, diffuseIntensity
	//								4.0, 0.0f, 0.0f,	//light position
	//								0.3f, 0.2f, 0.1f);	//quadratic equation values
	//pointLightCount++;
	//
	//pointLights[1] = CPointLight(1024, 1024,			//shadowWidth, shadowHeight
	//								0.01f, 100,			//far plane
	//								0.0f, 1.0f, 0.0f,		//color 
	//								0.0f, 0.1f,			//ambientIntensity, diffuseIntensity
	//								-4.0, 2.0f, 0.0f,	//light position
	//								0.3f, 0.1f, 0.1f);	//quadratic equation values
	//pointLightCount++;


	
	// spot light: 

	spotLights[0] = CSpotLight(1024, 1024,			//shadowWidth, shadowHeight
								0.01f, 100,			//near far plane
								1.0f, 1.0f, 1.0f,	//color
								0.0f, 1.0f,			//ambientIntensity, diffuseIntensity
								0.0, 0.0f, 0.0f,	//light position
								0.0f, -1.0f, 0.0f,	//spotLight direction
								1.0f, 0.f, 0.0f,	//quadratic equation values
								20.f);				//edge = angle of the spotlight
	spotLightCount++;
	

	//generate terrain with perlin noise 


	m_pData = (unsigned char*)malloc(m_nWidth * m_nHeight);

	unsigned int seed = 120;
	CPerlinNoise pn(seed);

	unsigned int inc = 0;

	// Fill the grid with values from the noise function

	for (unsigned int i = 0; i < m_nHeight; ++i) {     // y
		for (unsigned int j = 0; j < m_nWidth; ++j) {  // x
			double x = (double)j / ((double)m_nWidth);
			double y = (double)i / ((double)m_nHeight);

			double n = pn.noise(5 * x, 5 * y, 0.8);

			m_pData[inc] = (unsigned char)floor(26 * n); // 26 in this exmaple defines the height of the level

			inc++;
		}
	}

	//projection matrix: 
	glm::mat4 projection(1.0f);
	projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	
	//window loop
	while (!mainWindow.getShouldClose())
	{

		//glfwGetTime returns time in seconds
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// get and handle user input events
		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//update the shadowmap for the main light
		DirectionalShadowMapPass(&mainLight);
		
		for (size_t i = 0; i < pointLightCount; i++)
		{
			OmniShadowMapPass(&pointLights[i]);
		}

		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}
		

		RenderPass(projection, camera.calculateViewMatrix());

		//use the shaderID
		shaderList[0].UseShader();

		glUseProgram(0);

		mainWindow.swapBuffers();


	}

	return 0;
}