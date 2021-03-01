///------------------------------------------------------------------------------------------------
///  MeshLoader.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef MeshLoader_h
#define MeshLoader_h

///------------------------------------------------------------------------------------------------

#include "IResourceLoader.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

class MeshLoader final: public IResourceLoader
{
    friend class ResourceLoadingService;
    
public:
    void VInitialize() override;
    std::unique_ptr<IResource> VCreateAndLoadResource(const std::string& path) const override;
    
private:
    MeshLoader() = default;
    
    std::string ExtractAndRemoveInjectedTexCoordsIfAny(std::string& path) const;
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* MeshLoader_h */
