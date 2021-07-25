#pragma once

#include <gl/glew.h>

#include "stb_image.h"

class CTexture
{
public:
	CTexture();
	CTexture(const char* fileLoc);
	~CTexture();

	bool LoadTexture();
	bool LoadTextureAlpha();

	void UseTexture();
	void ClearTexture();

private: 
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;

};

