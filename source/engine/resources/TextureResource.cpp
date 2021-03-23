///------------------------------------------------------------------------------------------------
///  TextureResource.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "TextureResource.h"
#include "../rendering/opengl/Context.h"

#include <cassert>
#include <SDL_pixels.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

TextureResource::~TextureResource()
{
    GL_CHECK(glDeleteTextures(1, &mGLTextureId));
    SDL_FreeSurface(mSurface);
}

///------------------------------------------------------------------------------------------------

GLuint TextureResource::GetGLTextureId() const
{
    return mGLTextureId;
}

///------------------------------------------------------------------------------------------------

const glm::vec2& TextureResource::GetDimensions() const
{
    return mDimensions;
}

///------------------------------------------------------------------------------------------------

colors::RGBTriplet TextureResource::GetRGBatPixel(const int x, const int y) const
{
    Uint8 r,g,b;
    auto pixel = *(Uint32*)((Uint8*)mSurface->pixels + y * mSurface->pitch + x * mSurface->format->BytesPerPixel);
    SDL_GetRGB(pixel, mSurface->format, &r, &g, &b);
    return colors::RGBTriplet(r, g, b);
}

///------------------------------------------------------------------------------------------------

TextureResource::TextureResource
(
    SDL_Surface* const surface,
    const int width,
    const int height,
    GLuint glTextureId
)
    : mSurface(surface)
    , mDimensions(width, height)
    , mGLTextureId(glTextureId)
{
}

///------------------------------------------------------------------------------------------------

}

}
