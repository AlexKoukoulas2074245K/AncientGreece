///------------------------------------------------------------------------------------------------
///  SfxLoader.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "SfxLoader.h"
#include "SfxResource.h"
#include "../common/utils/OSMessageBox.h"

#include <fstream>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

void SfxLoader::VInitialize()
{      
}

///------------------------------------------------------------------------------------------------

std::unique_ptr<IResource> SfxLoader::VCreateAndLoadResource(const std::string& resourcePath) const
{
    std::ifstream file(resourcePath);

    if (!file.good())
    {
        ShowMessageBox(MessageBoxType::ERROR, "File could not be found", resourcePath.c_str());
        return nullptr;
    }

    auto* loadedSfx = Mix_LoadWAV(resourcePath.c_str());
    if (!loadedSfx)
    {
        ShowMessageBox(MessageBoxType::ERROR, "SDL_mixer could not load sfx", Mix_GetError());
        return nullptr;
    }

    return std::unique_ptr<IResource>(new SfxResource(loadedSfx));
}

///------------------------------------------------------------------------------------------------

}

}