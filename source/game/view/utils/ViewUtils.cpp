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
#include "../../../engine/common/utils/StringUtils.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/resources/DataFileResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"

#include <rapidxml.hpp>

///------------------------------------------------------------------------------------------------

#if !defined(NDEBUG)
float DEBUG_VIEW_SIZE_DX = 0.0f;
float DEBUG_VIEW_SIZE_DY = 0.0f;
float DEBUG_TEXTBOX_SIZE_DX = 0.0f;
float DEBUG_TEXTBOX_SIZE_DY = 0.0f;
float DEBUG_TEXTBOX_DX = 0.0f;
float DEBUG_TEXTBOX_DY = 0.0f;
#endif

///------------------------------------------------------------------------------------------------

namespace view
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId GUI_MODEL_SHADER_NAME  = StringId("default_gui");
    static const StringId DEFAULT_FONT_NAME      = StringId("ancient_greek_font");
    static const std::string GUI_BASE_MODEL_NAME = "gui_base";

    static const char* XML_VIEW_NODE_NAME    = "View";
    static const char* XML_TEXTBOX_NODE_NAME = "Textbox";

    static const char* BACKGROUND_ATTRIBUTE_NAME = "background_name";

    static const char* BACKGROUND_X_ATTRIBUTE_NAME      = "x";
    static const char* BACKGROUND_Y_ATTRIBUTE_NAME      = "y";
    static const char* BACKGROUND_Z_ATTRIBUTE_NAME      = "z";
    static const char* BACKGROUND_WIDTH_ATTRIBUTE_NAME  = "width";
    static const char* BACKGROUND_HEIGHT_ATTRIBUTE_NAME = "height";

    static const char* TEXTBOX_X_ATTRIBUTE_NAME      = "x";
    static const char* TEXTBOX_Y_ATTRIBUTE_NAME      = "y";
    static const char* TEXTBOX_Z_ATTRIBUTE_NAME      = "z";
    static const char* TEXTBOX_WIDTH_ATTRIBUTE_NAME  = "width";
    static const char* TEXTBOX_HEIGHT_ATTRIBUTE_NAME = "height";
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
    const auto& xmlResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::DataFileResource>(xmlResourceId);
    
    auto xmlCopy = xmlResource.GetContents();
    rapidxml::xml_document<> doc;
    doc.parse<0>(&xmlCopy[0]);
    
    auto node = doc.first_node();
    
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
        glm::vec3 viewScale(1.0f);
        if (node->first_attribute(BACKGROUND_X_ATTRIBUTE_NAME) != nullptr)
        {
            viewPosition.x = std::atof(node->first_attribute(BACKGROUND_X_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(BACKGROUND_Y_ATTRIBUTE_NAME) != nullptr)
        {
            viewPosition.y = std::atof(node->first_attribute(BACKGROUND_Y_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(BACKGROUND_Z_ATTRIBUTE_NAME) != nullptr)
        {
            viewPosition.z = std::atof(node->first_attribute(BACKGROUND_Z_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(BACKGROUND_WIDTH_ATTRIBUTE_NAME) != nullptr)
        {
            viewScale.x = std::atof(node->first_attribute(BACKGROUND_WIDTH_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(BACKGROUND_HEIGHT_ATTRIBUTE_NAME) != nullptr)
        {
            viewScale.y = std::atof(node->first_attribute(BACKGROUND_HEIGHT_ATTRIBUTE_NAME)->value());
        }
        
        assert(node->first_attribute(BACKGROUND_ATTRIBUTE_NAME) != nullptr && "No background attribute in view");
        
#if !defined(NDEBUG)
        viewScale.x += DEBUG_VIEW_SIZE_DX;
        viewScale.y += DEBUG_VIEW_SIZE_DY;
#endif
        viewStateComponent.mViewEntities.push_back(genesis::rendering::LoadAndCreateGuiSprite(GUI_BASE_MODEL_NAME, std::string(node->first_attribute(BACKGROUND_ATTRIBUTE_NAME)->value()), GUI_MODEL_SHADER_NAME, viewPosition, glm::vec3(0.0f), viewScale));
    }
    else if (std::strcmp(XML_TEXTBOX_NODE_NAME, node->name()) == 0)
    {
        glm::vec3 textboxPosition(0.0f);
        glm::vec2 textboxDimensions(0.0f);
        if (node->first_attribute(TEXTBOX_X_ATTRIBUTE_NAME) != nullptr)
        {
            textboxPosition.x = std::atof(node->first_attribute(TEXTBOX_X_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(TEXTBOX_Y_ATTRIBUTE_NAME) != nullptr)
        {
            textboxPosition.y = std::atof(node->first_attribute(TEXTBOX_Y_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(TEXTBOX_Z_ATTRIBUTE_NAME) != nullptr)
        {
            textboxPosition.z = std::atof(node->first_attribute(TEXTBOX_Z_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(TEXTBOX_WIDTH_ATTRIBUTE_NAME) != nullptr)
        {
            textboxDimensions.x = std::atof(node->first_attribute(TEXTBOX_WIDTH_ATTRIBUTE_NAME)->value());
        }
        if (node->first_attribute(TEXTBOX_HEIGHT_ATTRIBUTE_NAME) != nullptr)
        {
            textboxDimensions.y = std::atof(node->first_attribute(TEXTBOX_HEIGHT_ATTRIBUTE_NAME)->value());
        }
        
#if !defined(NDEBUG)
        textboxPosition.x += DEBUG_TEXTBOX_DX;
        textboxPosition.y += DEBUG_TEXTBOX_DY;
        textboxDimensions.x += DEBUG_TEXTBOX_SIZE_DX;
        textboxDimensions.y += DEBUG_TEXTBOX_SIZE_DY;
#endif
        
        auto text = node->value();
        assert(text != nullptr && "No textbox string in tags");
        
        const auto textString = std::string(text);
        const auto textCharCount = textString.size();
        const auto textSplitBySpace = StringSplit(textString, ' ');
        
        const auto textboxArea = textboxDimensions.x * textboxDimensions.y;
        const auto derivedCharSize = genesis::math::Sqrt(textboxArea/textCharCount);
        
        glm::vec3 textPositionCounter(textboxPosition.x - textboxDimensions.x/2.0f, textboxPosition.y + textboxDimensions.y/2.0f, textboxPosition.z);
        std::string textAccumulator;
        
        for (const auto& textComponent: textSplitBySpace)
        {
            auto textIncludingSpace = textComponent + ' ';
            if (textPositionCounter.x + textIncludingSpace.size() * derivedCharSize > textboxPosition.x + textboxDimensions.x / 2.0f)
            {
                textPositionCounter.x = textboxPosition.x - textboxDimensions.x/2.0f;
                viewStateComponent.mViewEntities.push_back(genesis::rendering::RenderText(textAccumulator, DEFAULT_FONT_NAME, derivedCharSize, textPositionCounter));
                textAccumulator = "";
                textPositionCounter.y -= derivedCharSize;
            }
            
            textAccumulator += textIncludingSpace;
            
            if (textComponent == textSplitBySpace[textSplitBySpace.size() - 1])
            {
                textPositionCounter.x = textboxPosition.x - textboxDimensions.x/2.0f;
                viewStateComponent.mViewEntities.push_back(genesis::rendering::RenderText(textAccumulator, DEFAULT_FONT_NAME, derivedCharSize, textPositionCounter));
                textAccumulator = "";
            }
            
            textPositionCounter.x += textIncludingSpace.size() * derivedCharSize;
        }
    }
}

///-----------------------------------------------------------------------------------------------

}

