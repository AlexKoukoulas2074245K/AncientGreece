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
#include "../common/utils/MathUtils.h"
#include "../common/utils/ColorUtils.h"

#include <SDL_stdinc.h>
#include <SDL_surface.h>

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
    colors::RGBTriplet GetRGBatPixel(const int x, const int y) const;

private:
    TextureResource
    (
        SDL_Surface* const surface,
        const int width, 
        const int height,
        GLuint glTextureId
    );
    
private:
    SDL_Surface* const mSurface;
    const int mWidth;
    const int mHeight;
    const GLuint mGLTextureId;
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* TextureResource_h */
