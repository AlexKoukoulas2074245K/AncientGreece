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
    if (mSurface) SDL_FreeSurface(mSurface);
}

///------------------------------------------------------------------------------------------------

void TextureResource::ChangeTexture(SDL_Surface* const surface)
{
    GL_CHECK(glDeleteTextures(1, &mGLTextureId));
    SDL_FreeSurface(mSurface);
    
    GL_CHECK(glGenTextures(1, &mGLTextureId));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGLTextureId));
    
    GL_CHECK(glTexImage2D
    (
        GL_TEXTURE_2D,
        0,
        mMode,
        surface->w,
        surface->h,
        0,
        mFormat,
        GL_UNSIGNED_BYTE,
        surface->pixels
     ));
    
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    
    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    
    mSurface = surface;
    mDimensions = glm::ivec2(surface->w, surface->h);
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

colors::RgbTriplet<int> TextureResource::GetRgbAtPixel(const int x, const int y) const
{
    Uint8 r,g,b;
    auto pixel = *(Uint32*)((Uint8*)mSurface->pixels + y * mSurface->pitch + x * mSurface->format->BytesPerPixel);
    SDL_GetRGB(pixel, mSurface->format, &r, &g, &b);
    return colors::RgbTriplet<int>(r, g, b);
}

///------------------------------------------------------------------------------------------------

TextureResource::TextureResource
(
    SDL_Surface* const surface,
    const int width,
    const int height,
    const int mode,
    const int format,
    GLuint glTextureId
)
    : mSurface(surface)
    , mDimensions(width, height)
    , mMode(mode)
    , mFormat(format)
    , mGLTextureId(glTextureId)
{
}

///------------------------------------------------------------------------------------------------

}

}
