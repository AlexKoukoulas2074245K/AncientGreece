///------------------------------------------------------------------------------------------------
///  DAEMeshLoader.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef DAEMeshLoader_h
#define DAEMeshLoader_h

///------------------------------------------------------------------------------------------------

#include "IResourceLoader.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

class DAEMeshLoader final: public IResourceLoader
{
    friend class ResourceLoadingService;
    
public:
    void VInitialize() override;
    std::unique_ptr<IResource> VCreateAndLoadResource(const std::string& path) const override;
    
private:
    DAEMeshLoader() = default;
    
    std::string ExtractAndRemoveInjectedTexCoordsIfAny(std::string& path) const;
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* DAEMeshLoader_h */
