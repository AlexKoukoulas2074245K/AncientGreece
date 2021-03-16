///------------------------------------------------------------------------------------------------
///  XMLResource.h
///  Genesis
///
///  Created by Alex Koukoulas on 16/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef XMLResource_h
#define XMLResource_h

///------------------------------------------------------------------------------------------------

#include "IResource.h"

#include <memory>
#include <rapidxml.hpp>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

class XMLResource final: public IResource
{
    friend class XMLLoader;

public:
    ~XMLResource();
    
    const rapidxml::xml_node<>* GetRootNode() const;
 
private:
    XMLResource(std::unique_ptr<rapidxml::xml_document<>> doc);
    
private:
    std::unique_ptr<rapidxml::xml_document<>> mDocument;

};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* XMLResource_h */
