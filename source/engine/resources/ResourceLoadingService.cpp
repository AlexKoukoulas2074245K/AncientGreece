///------------------------------------------------------------------------------------------------
///  ResourceLoadingService.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "ResourceLoadingService.h"
#include "../resources/DataFileLoader.h"
#include "../resources/IResource.h"
#include "../resources/OBJMeshLoader.h"
#include "../resources/DAEMeshLoader.h"
#include "../resources/MusicLoader.h"
#include "../resources/SfxLoader.h"
#include "../resources/ShaderLoader.h"
#include "../resources/TextureLoader.h"
#include "../common/utils/FileUtils.h"
#include "../common/utils/Logging.h"
#include "../common/utils/OSMessageBox.h"
#include "../common/utils/StringUtils.h"
#include "../common/utils/TypeTraits.h"

#include <fstream>
#include <cassert>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

#ifdef _WIN32
const std::string ResourceLoadingService::RES_ROOT = "../res/";
#else
const std::string ResourceLoadingService::RES_ROOT = "../../res/";
#endif

const std::string ResourceLoadingService::RES_DATA_ROOT          = RES_ROOT + "data/";
const std::string ResourceLoadingService::RES_SCRIPTS_ROOT       = RES_ROOT + "scripts/";
const std::string ResourceLoadingService::RES_MODELS_ROOT        = RES_ROOT + "models/";
const std::string ResourceLoadingService::RES_MUSIC_ROOT         = RES_ROOT + "music/";
const std::string ResourceLoadingService::RES_SFX_ROOT           = RES_ROOT + "sfx/";
const std::string ResourceLoadingService::RES_XML_ROOT           = RES_ROOT + "xml/";
const std::string ResourceLoadingService::RES_SHADERS_ROOT       = RES_ROOT + "shaders/";
const std::string ResourceLoadingService::RES_TEXTURES_ROOT      = RES_ROOT + "textures/";
const std::string ResourceLoadingService::RES_ATLASES_ROOT       = RES_TEXTURES_ROOT + "atlases/";
const std::string ResourceLoadingService::RES_FONT_MAP_DATA_ROOT = RES_DATA_ROOT + "font_maps/";

///------------------------------------------------------------------------------------------------

ResourceLoadingService& ResourceLoadingService::GetInstance()
{
    static ResourceLoadingService instance;
    return instance;
}

///------------------------------------------------------------------------------------------------

ResourceLoadingService::~ResourceLoadingService()
{
}

///------------------------------------------------------------------------------------------------

void ResourceLoadingService::Initialize()
{
    // No make unique due to constructing the loaders with their private constructors
    // via friendship
    mResourceLoaders.push_back(std::unique_ptr<TextureLoader>(new TextureLoader));
    mResourceLoaders.push_back(std::unique_ptr<DataFileLoader>(new DataFileLoader));
    mResourceLoaders.push_back(std::unique_ptr<ShaderLoader>(new ShaderLoader));
    mResourceLoaders.push_back(std::unique_ptr<OBJMeshLoader>(new OBJMeshLoader));
    mResourceLoaders.push_back(std::unique_ptr<DAEMeshLoader>(new DAEMeshLoader));
    mResourceLoaders.push_back(std::unique_ptr<MusicLoader>(new MusicLoader));
    mResourceLoaders.push_back(std::unique_ptr<SfxLoader>(new SfxLoader));

    // Map resource extensions to loaders
    mResourceExtensionsToLoadersMap[StringId("png")]  = mResourceLoaders[0].get();
    mResourceExtensionsToLoadersMap[StringId("json")] = mResourceLoaders[1].get();
    mResourceExtensionsToLoadersMap[StringId("dat")]  = mResourceLoaders[1].get();
    mResourceExtensionsToLoadersMap[StringId("lua")]  = mResourceLoaders[1].get();
    mResourceExtensionsToLoadersMap[StringId("xml")]  = mResourceLoaders[1].get();
    mResourceExtensionsToLoadersMap[StringId("vs")]   = mResourceLoaders[2].get();
    mResourceExtensionsToLoadersMap[StringId("fs")]   = mResourceLoaders[2].get();
    mResourceExtensionsToLoadersMap[StringId("obj")]  = mResourceLoaders[3].get();
    mResourceExtensionsToLoadersMap[StringId("dae")]  = mResourceLoaders[4].get();
    mResourceExtensionsToLoadersMap[StringId("ogg")]  = mResourceLoaders[5].get();
    mResourceExtensionsToLoadersMap[StringId("wav")]  = mResourceLoaders[6].get();
    
    for (auto& resourceLoader: mResourceLoaders)
    {
        resourceLoader->VInitialize();
    }
}

///------------------------------------------------------------------------------------------------

ResourceId ResourceLoadingService::GetResourceIdFromPath(const std::string& path)
{    
    return GetStringHash(AdjustResourcePath(path));
}

///------------------------------------------------------------------------------------------------

ResourceId ResourceLoadingService::LoadResource(const std::string& resourcePath)
{
    const auto adjustedPath = AdjustResourcePath(resourcePath);
    const auto resourceId = GetStringHash(adjustedPath);
    
    if (mResourceMap.count(resourceId))
    {
        return resourceId;
    }
    else
    {
        LoadResourceInternal(adjustedPath, resourceId);
        return resourceId;
    }
}

///------------------------------------------------------------------------------------------------

void ResourceLoadingService::LoadResources(const std::vector<std::string>& resourcePaths)
{
    for (const auto& path: resourcePaths)
    {
        LoadResource(path);
    }
}

///------------------------------------------------------------------------------------------------

bool ResourceLoadingService::DoesResourceExist(const std::string& resourcePath) const
{
    const auto adjustedPath = AdjustResourcePath(resourcePath);
    std::fstream resourceFileCheck(resourcePath);
    return resourceFileCheck.operator bool();
}

///------------------------------------------------------------------------------------------------

bool ResourceLoadingService::HasLoadedResource(const std::string& resourcePath) const
{
    const auto adjustedPath = AdjustResourcePath(resourcePath);
    const auto resourceId = GetStringHash(adjustedPath);
    
    return mResourceMap.count(resourceId) != 0;
}

///------------------------------------------------------------------------------------------------

void ResourceLoadingService::UnloadResource(const std::string& resourcePath)
{
    const auto adjustedPath = AdjustResourcePath(resourcePath);
    const auto resourceId = GetStringHash(adjustedPath);
    mResourceMap.erase(resourceId);
}

///------------------------------------------------------------------------------------------------

void ResourceLoadingService::UnloadResource(const ResourceId resourceId)
{
    mResourceMap.erase(resourceId);
}

///------------------------------------------------------------------------------------------------

IResource& ResourceLoadingService::GetResource(const std::string& resourcePath)
{
    const auto adjustedPath = AdjustResourcePath(resourcePath);
    const auto resourceId = GetStringHash(adjustedPath);
    return GetResource(resourceId);
}

///------------------------------------------------------------------------------------------------

IResource& ResourceLoadingService::GetResource(const ResourceId resourceId)
{
    if (mResourceMap.count(resourceId))
    {
        return *mResourceMap[resourceId];
    }

    assert(false && "Resource could not be found");
    return *mResourceMap[resourceId];
}

///------------------------------------------------------------------------------------------------

void ResourceLoadingService::LoadResourceInternal(const std::string& resourcePath, const ResourceId resourceId)
{
    // Get resource extension
    const auto resourceFileExtension = GetFileExtension(resourcePath);
    
    // Pick appropriate loader
    auto& selectedLoader = mResourceExtensionsToLoadersMap.at(StringId(GetFileExtension(resourcePath)));
    auto loadedResource = selectedLoader->VCreateAndLoadResource(RES_ROOT + resourcePath);
    
    assert(loadedResource != nullptr && "No loader was able to load resource");
    mResourceMap[resourceId] = std::move(loadedResource);
}

///------------------------------------------------------------------------------------------------

std::string ResourceLoadingService::AdjustResourcePath(const std::string& resourcePath) const
{    
    return !StringStartsWith(resourcePath, RES_ROOT) ? resourcePath : resourcePath.substr(RES_ROOT.size(), resourcePath.size() - RES_ROOT.size());
}

///------------------------------------------------------------------------------------------------

}

}
