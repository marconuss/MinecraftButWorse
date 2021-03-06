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
#include "Skybox.h"
#include "SpotLight.h"
#include "Texture.h"
#include "Window.h"

//declaration of all the uniforms
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0;

//window
CWindow mainWindow;

//shaders
//
// list of shaders for the scene itself
std::vector<CShader> shaderList;
// first pass for the dierctional shadow
CShader directionalShadowShader;
// Vertex shaderID
static const char* vShader = "../Shaders/shader.vert";
// Fragment shaderID
static const char* fShader = "../Shaders/shader.frag";

// camera
CCamera camera;

// textures
CTexture minecraftTexture;
CTexture minecraftTextureS;
CTexture minecraftTextureW;
CTexture minecraftTextureI;

// materials
CMaterial shinyMaterial; 

//lights
CDirectionalLight mainLight;
CPointLight pointLights[MAX_POINT_LIGHTS];
CSpotLight spotLights[MAX_SPOT_LIGHTS];
unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

//skybox
CSkybox skybox;

//time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//cube 
CMesh* cube;

//perlinNoise size
unsigned int m_nWidth = 128;
unsigned int m_nHeight = 128;
unsigned char* m_pData;
unsigned char* m_tData;
unsigned int waterlevel = 9;


void CreateObjects() 
{

	//define triangles counter-clockwise
	unsigned int cubeIndices[] = {

		//front
		2,  0,  3,		//bottom left > top left > top right
		3,  0,	1,		//bottom left > top right > bottom right
		// right
		6,  4,  7,
		7,  4,  5,		
		// back
		10,  8, 11,
		11,	 8,  9,	
		// left
		14, 12, 15,
		15, 12, 13,	
		// top
		18, 16, 19,
		19, 16, 17,	
		// bottom
		22, 20, 23,
		23, 20, 21,
		
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

	// create the cube mesh
	cube = new CMesh();
	cube->CreateMesh(cubeVertices, cubeIndices, 192, 36);


	//generate terrain with perlin noise 
	m_pData = (unsigned char*)malloc(m_nWidth * m_nHeight);
	m_tData = (unsigned char*)malloc(m_nWidth * m_nHeight);

	srand(time(NULL));

	//max level height
	unsigned int levelHeight = 15;
	//random seed
	unsigned int seed = rand() % 100;
	CPerlinNoise pn(seed);

	unsigned int inc = 0;

	// Fill the grid with values from the noise function
	for (unsigned int i = 0; i < m_nHeight; ++i) {     // y
		for (unsigned int j = 0; j < m_nWidth; ++j) {  // x
			double x = (double)j / ((double)m_nWidth);
			double y = (double)i / ((double)m_nHeight);

			double n = pn.noise(5 * x, 5 * y, 0.8);

			m_pData[inc] = (unsigned char)floor(levelHeight * n); 
			m_tData[inc] = 0;

			if (m_pData[inc] > waterlevel) {
				m_pData[inc] = waterlevel;
				m_tData[inc] = 1;
			}

			if (m_pData[inc] == (waterlevel) && m_tData[inc] != 1){
				m_tData[inc] = 2;
			}

			if (m_pData[inc] < 4) {
				m_tData[inc] = 3;
			}

			inc++;
		}
	}
}

void CreateShaders()
{
	//create shader
	CShader *shaderOne = new CShader();
	shaderOne->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shaderOne);

	directionalShadowShader = CShader();
	directionalShadowShader.CreateFromFiles("../Shaders/directional_shadow_map.vert", "../Shaders/directional_shadow_map.frag");
}

void CreateLights()
{
	//main directional light: 

	mainLight = CDirectionalLight(
				2048, 2048,					//shadowbuffer
				1.0f, 1.0f, 1.0f,			//color
				0.2f, 0.7f,					//ambientIntensity, diffuseIntensity
				0.0f, -20.0f, -15.0f);		//direction


	// spot light: 

	spotLights[0] = CSpotLight(
				1.0f, 1.0f, 1.0f,			//color
				0.0f, 1.0f,					//ambientIntensity, diffuseIntensity
				0.0, 0.0f, 0.0f,			//light position
				0.0f, -1.0f, 0.0f,			//spotLight direction
				1.0f, 0.f, 0.0f,			//quadratic equation values
				20.f);						//edge = angle of the spotlight
	//add to the spotlights count
	spotLightCount++;
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

	// render perlin noise map
	//----------------------------
	// it renders each cube 
	// should be changed
	//----------------------------
	for (unsigned int x = 0; x < m_nWidth; x++) {
		for (unsigned int z = 0; z < m_nHeight; z++) {

			model = glm::mat4(1.0f);
			
			//glTranslatef(-0.5f + (float)x - (float)m_nWidth / 2.0f, -2.5f - (float)m_pData[z * m_nHeight + x], -0.5f - float(z) + (float)m_nHeight / 2.0f);
			model = glm::translate(model, glm::vec3(-0.5f + (float)x - (float)m_nWidth / 2.0f, -2.5f - (float)m_pData[z * m_nHeight + x], -0.5f - float(z) + (float)m_nHeight / 2.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (m_tData[z * m_nHeight + x] == 0) {
				minecraftTexture.UseTexture();
			}
			if (m_tData[z * m_nHeight + x] == 1) {
				minecraftTextureW.UseTexture();
			}
			if (m_tData[z * m_nHeight + x] == 2) {
				minecraftTextureS.UseTexture();
			}
			if (m_tData[z * m_nHeight + x] == 3) {
				minecraftTextureI.UseTexture();
			}
			
			shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
			cube->RenderMesh();

		}
	}

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



void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{

	glViewport(0, 0, 1366, 768);

	//clear window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	// render lights in the scene
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
	//initialize window
	mainWindow = CWindow(1366, 768);
	mainWindow.Initialize();

	CreateObjects();
	CreateShaders();
	CreateLights();

	//skybox
	std::vector<std::string> skyboxFaces;

	//load teh texture for each face
	//the order matters and it is +x, -x, +y, -y, +z, -z
	skyboxFaces.push_back("../Textures/Skybox/skybox_right.tga");
	skyboxFaces.push_back("../Textures/Skybox/skybox_left.tga");
	skyboxFaces.push_back("../Textures/Skybox/skybox_top.tga");
	skyboxFaces.push_back("../Textures/Skybox/skybox_bottom.tga");
	skyboxFaces.push_back("../Textures/Skybox/skybox_front.tga");
	skyboxFaces.push_back("../Textures/Skybox/skybox_back.tga");

	skybox = CSkybox(skyboxFaces);

	//initialize the camera
	camera = CCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	//load textures 
	minecraftTexture = CTexture("../Textures/cube2.png");
	minecraftTexture.LoadTextureAlpha();
	minecraftTextureI = CTexture("../Textures/ice.png");
	minecraftTextureI.LoadTextureAlpha();
	minecraftTextureW = CTexture("../Textures/water.png");
	minecraftTextureW.LoadTextureAlpha();
	minecraftTextureS = CTexture("../Textures/sand.png");
	minecraftTextureS.LoadTextureAlpha();
	//set material properties
	shinyMaterial = CMaterial(4.0f, 256);

	//projection matrix: 
	glm::mat4 projection(1.0f);
	projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	//set up time for frame time count
	GLfloat frameTime = glfwGetTime();
	int nbFrames = 0;
	
	//window loop
	while (!mainWindow.getShouldClose())
	{

		// glfwGetTime returns time in seconds
		GLfloat currentTime = glfwGetTime();
		// get the delta time since last frame
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		//frame time counter
		nbFrames++;
		if (currentTime - frameTime >= 1.0) { // If last prinf() was more than 1 sec ago
		// printf and reset timer
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			frameTime += 1.0;
		}

		// get and handle user input events
		glfwPollEvents();

		//camera controls
		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//update the shadowmap for the main light
		DirectionalShadowMapPass(&mainLight);

		// render game
		RenderPass(projection, camera.calculateViewMatrix());

		//use the shader
		shaderList[0].UseShader();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}