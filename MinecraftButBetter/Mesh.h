#pragma once

#include <GL/glew.h>

class CMesh
{
public:
	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
	void ClearMesh();

	CMesh();
	~CMesh();

private:
	GLuint VAO, VBO, IBO;
	//GLsizei indexCount;

};

