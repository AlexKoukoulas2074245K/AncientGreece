///------------------------------------------------------------------------------------------------
///  MusicLoader.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "MusicLoader.h"
#include "MusicResource.h"
#include "../common/utils/Logging.h"
#include "../common/utils/OSMessageBox.h"

#include <fstream>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

void MusicLoader::VInitialize()
{    
}

///------------------------------------------------------------------------------------------------

std::unique_ptr<IResource> MusicLoader::VCreateAndLoadResource(const std::string& resourcePath) const
{       
    std::ifstream file(resourcePath);

    if (!file.good())
    {
        ShowMessageBox(MessageBoxType::ERROR, "File could not be found", resourcePath.c_str());
        return nullptr;
    }

    auto* loadedMusic = Mix_LoadMUS(resourcePath.c_str());
    if (!loadedMusic)
    {
        ShowMessageBox(MessageBoxType::ERROR, "SDL_mixer could not load music", Mix_GetError());
        return nullptr;
    }

    return std::unique_ptr<IResource>(new MusicResource(loadedMusic));
}

///------------------------------------------------------------------------------------------------

}

}
