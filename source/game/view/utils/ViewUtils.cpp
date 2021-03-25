///------------------------------------------------------------------------------------------------
///  ViewUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 16/03/2021.
///------------------------------------------------------------------------------------------------

#include "ViewUtils.h"
#include "../components/ClickableComponent.h"
#include "../components/ViewStateComponent.h"
#include "../components/ViewQueueSingletonComponent.h"
#include "../../utils/KeyValueUtils.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/common/utils/MathUtils.h"
#include "../../../engine/common/utils/StringUtils.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/resources/DataFileResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"

#include <rapidxml.hpp>
#include <sstream>

///------------------------------------------------------------------------------------------------

#if !defined(NDEBUG)
float DEBUG_VIEW_SIZE_DX = 0.0f;
float DEBUG_VIEW_SIZE_DY = 0.0f;
float DEBUG_TEXTBOX_SIZE_DX = 0.0f;
float DEBUG_TEXTBOX_SIZE_DY = 0.0f;
float DEBUG_TEXTBOX_DX = 0.0f;
float DEBUG_TEXTBOX_DY = 0.0f;
float DEBUG_MODEL_RX = 0.0f;
float DEBUG_MODEL_RY = 0.0f;
float DEBUG_MODEL_RZ = 0.0f;
#endif

///------------------------------------------------------------------------------------------------

namespace view
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId GUI_MODEL_SHADER_NAME          = StringId("default_gui");
    static const StringId GUI_MODEL_3D_SHADER_NAME       = StringId("default_gui_3d");
    static const StringId DEFAULT_FONT_NAME              = StringId("ancient_greek_font");
    static const StringId DEFAULT_INTERACTION_EVENT_NAME = StringId("close");

    static const std::string GUI_BASE_MODEL_NAME = "gui_base";

    static const char* XML_VIEW_NODE_NAME          = "View";
    static const char* XML_TEXT_NODE_NAME          = "Text";
    static const char* XML_TEXTBOX_NODE_NAME       = "Textbox";
    static const char* XML_CLICKABLETEXT_NODE_NAME = "ClickableText";
    static const char* XML_MODEL_NODE_NAME         = "Model";

    static const char* BACKGROUND_ATTRIBUTE_NAME        = "background_name";
    static const char* BACKGROUND_X_ATTRIBUTE_NAME      = "x";
    static const char* BACKGROUND_Y_ATTRIBUTE_NAME      = "y";
    static const char* BACKGROUND_Z_ATTRIBUTE_NAME      = "z";
    static const char* BACKGROUND_WIDTH_ATTRIBUTE_NAME  = "width";
    static const char* BACKGROUND_HEIGHT_ATTRIBUTE_NAME = "height";

    static const char* TEXT_X_ATTRIBUTE_NAME    = "x";
    static const char* TEXT_Y_ATTRIBUTE_NAME    = "y";
    static const char* TEXT_Z_ATTRIBUTE_NAME    = "z";
    static const char* TEXT_SIZE_ATTRIBUTE_NAME = "size";
    static const char* TEXT_R_ATTRIBUTE_NAME    = "red";
    static const char* TEXT_G_ATTRIBUTE_NAME    = "green";
    static const char* TEXT_B_ATTRIBUTE_NAME    = "blue";

    static const char* TEXTBOX_X_ATTRIBUTE_NAME      = "x";
    static const char* TEXTBOX_Y_ATTRIBUTE_NAME      = "y";
    static const char* TEXTBOX_Z_ATTRIBUTE_NAME      = "z";
    static const char* TEXTBOX_WIDTH_ATTRIBUTE_NAME  = "width";
    static const char* TEXTBOX_HEIGHT_ATTRIBUTE_NAME = "height";

    static const char* CLICKABLETEXT_X_ATTRIBUTE_NAME                 = "x";
    static const char* CLICKABLETEXT_Y_ATTRIBUTE_NAME                 = "y";
    static const char* CLICKABLETEXT_Z_ATTRIBUTE_NAME                 = "z";
    static const char* CLICKABLETEXT_SIZE_ATTRIBUTE_NAME              = "size";
    static const char* CLICKABLETEXT_R_ATTRIBUTE_NAME                 = "red";
    static const char* CLICKABLETEXT_G_ATTRIBUTE_NAME                 = "green";
    static const char* CLICKABLETEXT_B_ATTRIBUTE_NAME                 = "blue";
    static const char* CLICKABLETEXT_INTERACTION_R_ATTRIBUTE_NAME     = "interaction_red";
    static const char* CLICKABLETEXT_INTERACTION_G_ATTRIBUTE_NAME     = "interaction_green";
    static const char* CLICKABLETEXT_INTERACTION_B_ATTRIBUTE_NAME     = "interaction_blue";
    static const char* CLICKABLETEXT_INTERACTION_EVENT_ATTRIBUTE_NAME = "interaction_event";

    static const char* MODEL_NAME_ATTRIBUTE_NAME    = "name";
    static const char* MODEL_X_ATTRIBUTE_NAME       = "x";
    static const char* MODEL_Y_ATTRIBUTE_NAME       = "y";
    static const char* MODEL_Z_ATTRIBUTE_NAME       = "z";
    static const char* MODEL_ROT_X_ATTRIBUTE_NAME   = "rot_x";
    static const char* MODEL_ROT_Y_ATTRIBUTE_NAME   = "rot_y";
    static const char* MODEL_ROT_Z_ATTRIBUTE_NAME   = "rot_z";
    static const char* MODEL_SCALE_X_ATTRIBUTE_NAME = "scale_x";
    static const char* MODEL_SCALE_Y_ATTRIBUTE_NAME = "scale_y";
    static const char* MODEL_SCALE_Z_ATTRIBUTE_NAME = "scale_z";

}

///------------------------------------------------------------------------------------------------

void RecursivelyTraverseViewNodes(const rapidxml::xml_node<>* node, ViewStateComponent& viewStateComponent);

///------------------------------------------------------------------------------------------------

void ProcessViewNode(const rapidxml::xml_node<>* node, ViewStateComponent& viewStateComponent);

///------------------------------------------------------------------------------------------------

std::string ReplaceTextVariables(const std::string& text);

///------------------------------------------------------------------------------------------------

bool HasActiveView()
{
    const auto& world = genesis::ecs::World::GetInstance();
    return world.GetSingletonComponent<ViewQueueSingletonComponent>().mActiveViewExists;
}

///------------------------------------------------------------------------------------------------

void QueueView
(
    const std::string& viewName,
    const StringId entityName /* StringId() */
)
{
    auto& world = genesis::ecs::World::GetInstance();
    world.GetSingletonComponent<ViewQueueSingletonComponent>().mQueuedViews.push(std::make_pair(viewName, entityName));
}

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId LoadAndShowView
(
    const std::string& viewName,
    const StringId entityName /* StringId() */
)
{
    auto& world = genesis::ecs::World::GetInstance();
    auto viewEntity = world.CreateEntity();
    auto viewStateComponent = std::make_unique<ViewStateComponent>();
    
    genesis::resources::ResourceLoadingService::GetInstance().UnloadResource(genesis::resources::ResourceLoadingService::RES_XML_ROOT + viewName + ".xml");
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
    world.GetSingletonComponent<ViewQueueSingletonComponent>().mActiveViewExists = true;
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
    world.GetSingletonComponent<ViewQueueSingletonComponent>().mActiveViewExists = false;
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
            viewPosition.x = std::stof(ReplaceTextVariables(node->first_attribute(BACKGROUND_X_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(BACKGROUND_Y_ATTRIBUTE_NAME) != nullptr)
        {
            viewPosition.y = std::stof(ReplaceTextVariables(node->first_attribute(BACKGROUND_Y_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(BACKGROUND_Z_ATTRIBUTE_NAME) != nullptr)
        {
            viewPosition.z = std::stof(ReplaceTextVariables(node->first_attribute(BACKGROUND_Z_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(BACKGROUND_WIDTH_ATTRIBUTE_NAME) != nullptr)
        {
            viewScale.x = std::stof(ReplaceTextVariables(node->first_attribute(BACKGROUND_WIDTH_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(BACKGROUND_HEIGHT_ATTRIBUTE_NAME) != nullptr)
        {
            viewScale.y = std::stof(ReplaceTextVariables(node->first_attribute(BACKGROUND_HEIGHT_ATTRIBUTE_NAME)->value()));
        }
        
        assert(node->first_attribute(BACKGROUND_ATTRIBUTE_NAME) != nullptr && "No background attribute in view");
        
#if !defined(NDEBUG)
        viewScale.x += DEBUG_VIEW_SIZE_DX;
        viewScale.y += DEBUG_VIEW_SIZE_DY;
#endif
        viewStateComponent.mViewEntities.push_back(genesis::rendering::LoadAndCreateGuiSprite(GUI_BASE_MODEL_NAME, std::string(ReplaceTextVariables(node->first_attribute(BACKGROUND_ATTRIBUTE_NAME)->value())), GUI_MODEL_SHADER_NAME, viewPosition, glm::vec3(0.0f), viewScale));
    }
    else if (std::strcmp(XML_TEXT_NODE_NAME, node->name()) == 0)
    {
        glm::vec4 textColor(1.0f);
        glm::vec3 textPosition(0.0f);
        float textSize(0.0f);
        
        if (node->first_attribute(TEXT_X_ATTRIBUTE_NAME) != nullptr)
        {
            textPosition.x = std::stof(ReplaceTextVariables(node->first_attribute(TEXT_X_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXT_Y_ATTRIBUTE_NAME) != nullptr)
        {
            textPosition.y = std::stof(ReplaceTextVariables(node->first_attribute(TEXT_Y_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXT_Z_ATTRIBUTE_NAME) != nullptr)
        {
            textPosition.z = std::stof(ReplaceTextVariables(node->first_attribute(TEXT_Z_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXT_SIZE_ATTRIBUTE_NAME) != nullptr)
        {
            textSize = std::stof(ReplaceTextVariables(node->first_attribute(TEXT_SIZE_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXT_R_ATTRIBUTE_NAME) != nullptr)
        {
            textColor.r = std::stof(ReplaceTextVariables(node->first_attribute(TEXT_R_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXT_G_ATTRIBUTE_NAME) != nullptr)
        {
            textColor.g = std::stof(ReplaceTextVariables(node->first_attribute(TEXT_G_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXT_B_ATTRIBUTE_NAME) != nullptr)
        {
            textColor.b = std::stof(ReplaceTextVariables(node->first_attribute(TEXT_B_ATTRIBUTE_NAME)->value()));
        }
        
        auto rawText = node->value();
        assert(rawText != nullptr && "No text string value in tags");
        
        auto text = ReplaceTextVariables(std::string(rawText));
        
        viewStateComponent.mViewEntities.push_back(genesis::rendering::RenderText(text, DEFAULT_FONT_NAME, textSize, textPosition, textColor));
    }
    else if (std::strcmp(XML_CLICKABLETEXT_NODE_NAME, node->name()) == 0)
    {
        glm::vec4 clickableTextColor(1.0f);
        glm::vec4 clickableTextInteractionColor(1.0f);
        glm::vec3 clickableTextPosition(0.0f);
        StringId clickableTextEventName = StringId();
        float clickableTextSize(0.0f);
        
        
        if (node->first_attribute(CLICKABLETEXT_X_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextPosition.x = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_X_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_Y_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextPosition.y = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_Y_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_Z_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextPosition.z = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_Z_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_SIZE_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextSize = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_SIZE_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_R_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextColor.r = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_R_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_G_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextColor.g = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_G_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_B_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextColor.b = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_B_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_INTERACTION_R_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextInteractionColor.r = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_INTERACTION_R_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_INTERACTION_G_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextInteractionColor.g = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_INTERACTION_G_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_INTERACTION_B_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextInteractionColor.b = std::stof(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_INTERACTION_B_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(CLICKABLETEXT_INTERACTION_EVENT_ATTRIBUTE_NAME) != nullptr)
        {
            clickableTextEventName = StringId(ReplaceTextVariables(node->first_attribute(CLICKABLETEXT_INTERACTION_EVENT_ATTRIBUTE_NAME)->value()));
        }
        
        auto rawText = node->value();
        assert(rawText != nullptr && "No text string value in tags");
        
        auto text = ReplaceTextVariables(std::string(rawText));
        
        auto entity = genesis::rendering::RenderText(std::string(text), DEFAULT_FONT_NAME, clickableTextSize, clickableTextPosition);
        
        auto clickableComponent = std::make_unique<ClickableComponent>();
        clickableComponent->mTextColor = clickableTextColor;
        clickableComponent->mInteractionColor = clickableTextInteractionColor;
        clickableComponent->mInteractionEvent = clickableTextEventName;
        
        genesis::ecs::World::GetInstance().AddComponent<ClickableComponent>(entity, std::move(clickableComponent));
        
        viewStateComponent.mViewEntities.push_back(entity);
    }
    else if (std::strcmp(XML_TEXTBOX_NODE_NAME, node->name()) == 0)
    {
        glm::vec3 textboxPosition(0.0f);
        glm::vec2 textboxDimensions(0.0f);
        if (node->first_attribute(TEXTBOX_X_ATTRIBUTE_NAME) != nullptr)
        {
            textboxPosition.x = std::stof(ReplaceTextVariables(node->first_attribute(TEXTBOX_X_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXTBOX_Y_ATTRIBUTE_NAME) != nullptr)
        {
            textboxPosition.y = std::stof(ReplaceTextVariables(node->first_attribute(TEXTBOX_Y_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXTBOX_Z_ATTRIBUTE_NAME) != nullptr)
        {
            textboxPosition.z = std::stof(ReplaceTextVariables(node->first_attribute(TEXTBOX_Z_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXTBOX_WIDTH_ATTRIBUTE_NAME) != nullptr)
        {
            textboxDimensions.x = std::stof(ReplaceTextVariables(node->first_attribute(TEXTBOX_WIDTH_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(TEXTBOX_HEIGHT_ATTRIBUTE_NAME) != nullptr)
        {
            textboxDimensions.y = std::stof(ReplaceTextVariables(node->first_attribute(TEXTBOX_HEIGHT_ATTRIBUTE_NAME)->value()));
        }
        
#if !defined(NDEBUG)
        textboxPosition.x += DEBUG_TEXTBOX_DX;
        textboxPosition.y += DEBUG_TEXTBOX_DY;
        textboxDimensions.x += DEBUG_TEXTBOX_SIZE_DX;
        textboxDimensions.y += DEBUG_TEXTBOX_SIZE_DY;
#endif
        
        auto rawText = node->value();
        assert(rawText != nullptr && "No text string value in tags");
        
        auto text = ReplaceTextVariables(std::string(rawText));
        
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
    else if (std::strcmp(XML_MODEL_NODE_NAME, node->name()) == 0)
    {
        glm::vec3 modelPosition(0.0f);
        glm::vec3 modelRotation(0.0f);
        glm::vec3 modelScale(1.0f);
        if (node->first_attribute(MODEL_X_ATTRIBUTE_NAME) != nullptr)
        {
            modelPosition.x = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_X_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(MODEL_Y_ATTRIBUTE_NAME) != nullptr)
        {
            modelPosition.y = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_Y_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(MODEL_Z_ATTRIBUTE_NAME) != nullptr)
        {
            modelPosition.z = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_Z_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(MODEL_ROT_X_ATTRIBUTE_NAME) != nullptr)
        {
            modelRotation.x = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_ROT_X_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(MODEL_ROT_Y_ATTRIBUTE_NAME) != nullptr)
        {
            modelRotation.y = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_ROT_Y_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(MODEL_ROT_Z_ATTRIBUTE_NAME) != nullptr)
        {
            modelRotation.z = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_ROT_Z_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(MODEL_SCALE_X_ATTRIBUTE_NAME) != nullptr)
        {
            modelScale.x = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_SCALE_X_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(MODEL_SCALE_Y_ATTRIBUTE_NAME) != nullptr)
        {
            modelScale.y = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_SCALE_Y_ATTRIBUTE_NAME)->value()));
        }
        if (node->first_attribute(MODEL_SCALE_Z_ATTRIBUTE_NAME) != nullptr)
        {
            modelScale.z = std::stof(ReplaceTextVariables(node->first_attribute(MODEL_SCALE_Z_ATTRIBUTE_NAME)->value()));
        }
        
                
#if !defined(NDEBUG)
        modelRotation.x += DEBUG_MODEL_RX;
        modelRotation.y += DEBUG_MODEL_RY;
        modelRotation.z += DEBUG_MODEL_RZ;
#endif

        assert(node->first_attribute(MODEL_NAME_ATTRIBUTE_NAME) != nullptr && "No model name present");
        
        auto modelName = std::string(ReplaceTextVariables(node->first_attribute(MODEL_NAME_ATTRIBUTE_NAME)->value()));
        
        viewStateComponent.mViewEntities.push_back(genesis::rendering::LoadAndCreateGuiSprite(modelName, modelName, GUI_MODEL_3D_SHADER_NAME, modelPosition, modelRotation, modelScale, true));
    }
}

///-----------------------------------------------------------------------------------------------


///-----------------------------------------------------------------------------------------------

std::string ReplaceTextVariables(const std::string& text)
{
    std::stringstream textBuilder;
    std::stringstream variableNameBuilder;
    
    auto recordingVariableName = false;
    
    for (auto c: text)
    {
        if (c == '{')
        {
            recordingVariableName = true;
        }
        else if (c == '}')
        {
            textBuilder << ReadValue(StringId(variableNameBuilder.str()));
            variableNameBuilder.str("");
            recordingVariableName = false;
        }
        else
        {
            if (recordingVariableName)
            {
                variableNameBuilder << c;
            }
            else
            {
                textBuilder << c;
            }
        }
    }
    
    assert(!recordingVariableName && "Missing variable closing brace");
    return textBuilder.str();
}

///-----------------------------------------------------------------------------------------------

}

