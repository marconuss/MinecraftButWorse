#include "Mesh.h"

CMesh::CMesh() 
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	//indexCount = 0;
}


void CMesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	//indexCount = numOfIndices;

	//create the VAO and bind it to the ID
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//create and bind the IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);
	//create the VBO and bind it to the ID
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//connect the buffer data to the gl array buffer
	//GL_STATIC_DRAW is used in case we don't want to change our triangle
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

	//for the fifth parameter we need to consider that we have 3 values + 2texture coordinates + 3normals = 8
	//first at position 0, the 3 coordinates values x, y, z
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
	glEnableVertexAttribArray(0);
	//we have to reed the texture coordinates, at location 1 and takes 2 value (u, v)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
	glEnableVertexAttribArray(1);
	//location 2 has 3 values for the normal vector nx, ny, nz
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
	glEnableVertexAttribArray(2);

	//unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind the VAO
	glBindVertexArray(0);
	//unbind the IBO last, because we want it attached to the vao till the end
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CMesh::RenderMesh()
{

	//enable culling for better performance 
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CMesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}
	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	//indexCount = 0;
}

CMesh::~CMesh()
{
	ClearMesh();
}