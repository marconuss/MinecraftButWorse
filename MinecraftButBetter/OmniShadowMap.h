#pragma once

#include "ShadowMap.h"

class OmniShadowMap : public CShadowMap
{
public:
    OmniShadowMap();
    
    bool Init(unsigned int width, unsigned int height);

    void Write();
    
    void Read(GLenum TextureUnit);
    
    ~OmniShadowMap();

};

