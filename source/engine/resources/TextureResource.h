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
    const glm::vec2& GetDimensions() const;
    colors::RgbTriplet<int> GetRgbAtPixel(const int x, const int y) const;

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
    const glm::vec2 mDimensions;
    const GLuint mGLTextureId;
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* TextureResource_h */
