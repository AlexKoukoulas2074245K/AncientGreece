///------------------------------------------------------------------------------------------------
///  TextureResource.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef TextureResource_h
#define TextureResource_h

///------------------------------------------------------------------------------------------------

#include "IResource.h"

#include <SDL_stdinc.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

using GLuint = unsigned int;

///------------------------------------------------------------------------------------------------

class TextureResource final: public IResource
{
    friend class TextureLoader;

public:
    ~TextureResource();
    
    GLuint GetGLTextureId() const;
    int GetWidth() const;
    int GetHeight() const;    

private:
    TextureResource
    (
        const int width, 
        const int height,
        GLuint glTextureId        
    );
    
private:
    const int mWidth;
    const int mHeight;
    const GLuint mGLTextureId;    
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* TextureResource_h */
