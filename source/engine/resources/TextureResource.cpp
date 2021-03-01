///------------------------------------------------------------------------------------------------
///  TextureResource.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "TextureResource.h"
#include "../rendering/opengl/Context.h"

#include <cassert>

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
}

///------------------------------------------------------------------------------------------------

GLuint TextureResource::GetGLTextureId() const
{
    return mGLTextureId;
}

///------------------------------------------------------------------------------------------------

int TextureResource::GetWidth() const
{
    return mWidth;
}

///------------------------------------------------------------------------------------------------

int TextureResource::GetHeight() const
{
    return mHeight;
}

///------------------------------------------------------------------------------------------------

TextureResource::TextureResource
(
    const int width,
    const int height,
    GLuint glTextureId
)
    : mWidth(width)
    , mHeight(height)
    , mGLTextureId(glTextureId)    
{

}

///------------------------------------------------------------------------------------------------

}

}
