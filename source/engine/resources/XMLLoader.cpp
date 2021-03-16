///------------------------------------------------------------------------------------------------
///  XMLLoader.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#include "XMLLoader.h"
#include "XMLResource.h"
#include "../common/utils/OSMessageBox.h"

#include <fstream>   // ifstream
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace resources
{

///-----------------------------------------------------------------------------------------------

void XMLLoader::VInitialize()
{ 
}

///-----------------------------------------------------------------------------------------------

std::unique_ptr<IResource> XMLLoader::VCreateAndLoadResource(const std::string& resourcePath) const
{
    auto doc = std::make_unique<rapidxml::xml_document<>>();
    
    std::ifstream file(resourcePath);
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    buffer.push_back('\0');

    doc->parse<0>(&buffer[0]);
    return std::unique_ptr<IResource>(new XMLResource(std::move(doc)));
}

///-----------------------------------------------------------------------------------------------

}

}

