///------------------------------------------------------------------------------------------------
///  FontUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///------------------------------------------------------------------------------------------------

#include "FontUtils.h"
#include "MeshUtils.h"
#include "../components/FontsStoreSingletonComponent.h"
#include "../components/RenderableComponent.h"
#include "../components/TextStringComponent.h"
#include "../../common/components/NameComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../resources/ResourceLoadingService.h"
#include "../../resources/DataFileResource.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId TEXT_SHADER_NAME                     = StringId("default_gui");
    static const StringId TEXT_3D_SHADER_NAME                  = StringId("text_3d");
    static const StringId GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME = StringId("custom_color");

    static const std::string FONT_MAP_FILE_EXTENSION           = ".dat";
    static const std::string FONT_ATLAS_TEXTURE_FILE_EXTENSION = ".png";

    static const float DEFAULT_FONT_PADDING_PROPORTION_TO_SIZE = 0.333333f;
    static const float FONT_3D_PADDING_PROPORTION_TO_SIZE      = 0.8f;
}

///------------------------------------------------------------------------------------------------

void LoadFont
(
    const StringId& fontName,
    const int fontAtlasCols,
    const int fontAtlasRows
)
{
    auto& world = ecs::World::GetInstance();

    // Get/Create the font store component
    if (!world.HasSingletonComponent<FontsStoreSingletonComponent>())
    {
        world.SetSingletonComponent<FontsStoreSingletonComponent>(std::make_unique<FontsStoreSingletonComponent>());
    }

    // Make sure the font is not already loaded
    auto& fontStoreComponent = world.GetSingletonComponent<FontsStoreSingletonComponent>();
    if (fontStoreComponent.mLoadedFonts.count(fontName) != 0)
    {
        return;
    }

    const auto fontMapFileResourceId = resources::ResourceLoadingService::GetInstance().LoadResource(resources::ResourceLoadingService::RES_FONT_MAP_DATA_ROOT + fontName.GetString() + FONT_MAP_FILE_EXTENSION);
    const auto& fontMapFileResource  = resources::ResourceLoadingService::GetInstance().GetResource<resources::DataFileResource>(fontMapFileResourceId);

    const auto fontMapSplitByNewline = StringSplit(fontMapFileResource.GetContents(), '\n');
    
    for (auto row = 0U; row < fontMapSplitByNewline.size(); ++row)
    {
        const auto fontMapLineSplitBySpace = StringSplit(fontMapSplitByNewline[row], ' ');
        for (auto col = 0U; col < fontMapLineSplitBySpace.size(); ++col)
        {
            const auto currentFontCharacter = fontMapLineSplitBySpace[col][0];
            fontStoreComponent.mLoadedFonts[fontName][currentFontCharacter] = LoadAndCreateMeshFromAtlasTexCoords
            (
                col,
                row,
                fontAtlasCols,
                fontAtlasRows,
                false                
            );
        }
    }

    // Add space character
    fontStoreComponent.mLoadedFonts[fontName][' '] = LoadAndCreateMeshFromAtlasTexCoords
    (
        fontAtlasCols - 1,
        fontAtlasRows - 1,
        fontAtlasCols,
        fontAtlasRows,
        false
    );

    resources::ResourceLoadingService::GetInstance().UnloadResource(fontMapFileResourceId);
}

///-----------------------------------------------------------------------------------------------

ecs::EntityId RenderText
(
    const std::string& text,
    const StringId& fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color, /* glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) */
    const bool is3d, /* false */
    const StringId entityName
)
{
    auto& world = ecs::World::GetInstance();
    auto& fontStoreComponent = world.GetSingletonComponent<FontsStoreSingletonComponent>();
    
    auto transformComponent  = std::make_unique<TransformComponent>();
    transformComponent->mPosition = position;
    transformComponent->mScale = glm::vec3(size);
    
    auto renderableComponent = std::make_unique<RenderableComponent>();
    renderableComponent->mTextureResourceId = resources::ResourceLoadingService::GetInstance().LoadResource(resources::ResourceLoadingService::RES_ATLASES_ROOT + fontName.GetString() + FONT_ATLAS_TEXTURE_FILE_EXTENSION);
    renderableComponent->mShaderNameId = is3d ? TEXT_3D_SHADER_NAME : TEXT_SHADER_NAME;
    renderableComponent->mRenderableType = is3d ? genesis::rendering::RenderableType::TEXT_3D_MODEL : genesis::rendering::RenderableType::GUI_SPRITE;
    renderableComponent->mShaderUniforms.mShaderFloatVec4Uniforms[GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME] = color;
    
    for (const auto& character : text)
    {
        if (fontStoreComponent.mLoadedFonts.at(fontName).count(character) == 0)
        {
            continue;
        }
        
        renderableComponent->mMeshResourceIds.push_back( fontStoreComponent.mLoadedFonts.at(fontName).at(character));
    }
    
    auto textStringComponent = std::make_unique<TextStringComponent>();
    textStringComponent->mText = text;
    textStringComponent->mCharacterSize = size;
    textStringComponent->mPaddingProportionalToSize = is3d ? FONT_3D_PADDING_PROPORTION_TO_SIZE : DEFAULT_FONT_PADDING_PROPORTION_TO_SIZE;
    
    auto entity = world.CreateEntity();
    world.AddComponent<TransformComponent>(entity, std::move(transformComponent));
    world.AddComponent<TextStringComponent>(entity, std::move(textStringComponent));
    world.AddComponent<RenderableComponent>(entity, std::move(renderableComponent));

    if (entityName != StringId())
    {
        world.AddComponent<NameComponent>(entity, std::make_unique<NameComponent>(entityName));
    }
    
    return entity;
}

///-----------------------------------------------------------------------------------------------

ecs::EntityId RenderTextIfDifferentToPreviousString
(
    const std::string& text,
    const ecs::EntityId previousString,
    const StringId& fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color /* glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) */
)
{
    auto& world = ecs::World::GetInstance();
    
    if (previousString != ecs::NULL_ENTITY_ID && IsTextStringTheSameAsText(previousString, text))
    {
        return previousString;
    }
    
    if (previousString != ecs::NULL_ENTITY_ID)
    {
        world.DestroyEntity(previousString);
    }

    return RenderText(text, fontName, size, position, color);
}

///-----------------------------------------------------------------------------------------------

bool IsTextStringTheSameAsText
(
    const ecs::EntityId textStringEntityId,
    const std::string& textToTest
)
{
    auto& world = ecs::World::GetInstance();
    auto& textStringComponent = world.GetComponent<TextStringComponent>(textStringEntityId);
    
    return textStringComponent.mText == textToTest;
}

///-----------------------------------------------------------------------------------------------

genesis::math::Rectangle CalculateTextBoundingRect
(
    const ecs::EntityId textStringEntityId
)
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& transformComponent = world.GetComponent<TransformComponent>(textStringEntityId);
    const auto& textStringComponent = world.GetComponent<TextStringComponent>(textStringEntityId);
    
    genesis::math::Rectangle result;
    const auto renderedTextWidth = ((textStringComponent.mPaddingProportionalToSize * textStringComponent.mCharacterSize) * textStringComponent.mText.size());
    const auto renderedTextHeight = textStringComponent.mCharacterSize;
    
    result.bottomLeft = glm::vec2(transformComponent.mPosition.x - textStringComponent.mCharacterSize/8.0f, transformComponent.mPosition.y - renderedTextHeight/4.0f);
    result.topRight = glm::vec2(transformComponent.mPosition.x + renderedTextWidth - textStringComponent.mCharacterSize/8.0f, transformComponent.mPosition.y + renderedTextHeight/2.0f);
    
    return result;
}

///-----------------------------------------------------------------------------------------------

}

}

