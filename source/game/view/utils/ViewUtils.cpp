///------------------------------------------------------------------------------------------------
///  ViewUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 16/03/2021.
///------------------------------------------------------------------------------------------------

#include "ViewUtils.h"
#include "../components/ViewStateComponent.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/common/utils/MathUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/resources/XMLResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"

///------------------------------------------------------------------------------------------------

namespace view
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId GUI_MODEL_SHADER_NAME  = StringId("default_gui");
    static const std::string GUI_BASE_MODEL_NAME = "gui_base";

    static const char* XML_VIEW_NODE_NAME    = "View";
    static const char* XML_TEXTBOX_NODE_NAME = "Textbox";

    static const char* BACKGROUND_ATTRIBUTE_NAME = "background_name";
    static const char* POSITION_X_ATTRIBUTE_NAME = "x";
    static const char* POSITION_Y_ATTRIBUTE_NAME = "y";
    static const char* POSITION_Z_ATTRIBUTE_NAME = "z";
}

///------------------------------------------------------------------------------------------------

void RecursivelyTraverseViewNodes(const rapidxml::xml_node<>* node, ViewStateComponent& viewStateComponent);

///------------------------------------------------------------------------------------------------

void ProcessViewNode(const rapidxml::xml_node<>* node, ViewStateComponent& viewStateComponent);

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId LoadView
(
    const std::string& viewName,
    const StringId entityName /* StringId() */
)
{
    auto& world = genesis::ecs::World::GetInstance();
    auto viewEntity = world.CreateEntity();
    auto viewStateComponent = std::make_unique<ViewStateComponent>();
    
    auto xmlResourceId = genesis::resources::ResourceLoadingService::GetInstance().LoadResource(genesis::resources::ResourceLoadingService::RES_XML_ROOT + viewName + ".xml");
    const auto& xmlResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::XMLResource>(xmlResourceId);

    auto node = xmlResource.GetRootNode();
    
    RecursivelyTraverseViewNodes(node, *viewStateComponent);
    
    if (entityName != StringId())
    {
        world.AddComponent<genesis::NameComponent>(viewEntity, std::make_unique<genesis::NameComponent>(entityName));
    }
    
    world.AddComponent<ViewStateComponent>(viewEntity, std::move(viewStateComponent));
    
    return viewEntity;
}

///-----------------------------------------------------------------------------------------------

void DestroyView
(
    const genesis::ecs::EntityId viewEntityId
)
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& viewStateComponent = world.GetComponent<ViewStateComponent>(viewEntityId);
    
    for (auto entityId: viewStateComponent.mViewEntities)
    {
        world.DestroyEntity(entityId);
    }
    
    world.DestroyEntity(viewEntityId);
}

///-----------------------------------------------------------------------------------------------

void RecursivelyTraverseViewNodes(const rapidxml::xml_node<>* node, ViewStateComponent& viewStateComponent)
{
    if (node == nullptr || std::strlen(node->name()) == 0) return;
    
    ProcessViewNode(node, viewStateComponent);
    
    for (auto childNode = node->first_node(); childNode; childNode = childNode->next_sibling())
    {
        RecursivelyTraverseViewNodes(childNode, viewStateComponent);
    }
}

///-----------------------------------------------------------------------------------------------

void ProcessViewNode(const rapidxml::xml_node<>* node, ViewStateComponent& viewStateComponent)
{
    if (std::strcmp(XML_VIEW_NODE_NAME, node->name()) == 0)
    {
        glm::vec3 viewPosition(0.0f);
        if (node->first_attribute(POSITION_X_ATTRIBUTE_NAME) != nullptr)
        {
            viewPosition.x = std::atof(node->first_attribute(POSITION_X_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(POSITION_Y_ATTRIBUTE_NAME) != nullptr)
        {
            viewPosition.y = std::atof(node->first_attribute(POSITION_Y_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(POSITION_Z_ATTRIBUTE_NAME) != nullptr)
        {
            viewPosition.z = std::atof(node->first_attribute(POSITION_Z_ATTRIBUTE_NAME)->value());
        }
        
        assert(node->first_attribute(BACKGROUND_ATTRIBUTE_NAME) != nullptr && "No background attribute in view");
        
        viewStateComponent.mViewEntities.push_back(genesis::rendering::LoadAndCreateGuiSprite(GUI_BASE_MODEL_NAME, std::string(node->first_attribute(BACKGROUND_ATTRIBUTE_NAME)->value()), GUI_MODEL_SHADER_NAME, viewPosition));
    }
    else if (std::strcmp(XML_TEXTBOX_NODE_NAME, node->name()) == 0)
    {
        
    }
}

///-----------------------------------------------------------------------------------------------

}

