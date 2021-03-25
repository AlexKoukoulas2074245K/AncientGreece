///------------------------------------------------------------------------------------------------
///  FontUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///------------------------------------------------------------------------------------------------

#ifndef FontUtils_h
#define FontUtils_h

///------------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------
/// Loads the font with the given name. 
///
/// This function assumes that a font texture atlas (res/textures/atlases) and a font data file
/// (under res/data/font_maps) exist with the same name as the one passed in the function.
/// @param[in] fontName the name of the font to load.
/// @param[in] fontAtlasCols the number of columns in the font atlas texture.
/// @param[in] fontAtlasRows the number of rows in the font atlas texture.
void LoadFont
(
    const StringId& fontName, 
    const int fontAtlasCols, 
    const int fontAtlasRows
);

///------------------------------------------------------------------------------------------------
/// Renders a text string with the given font. 
///
/// @param[in] text the text to render.
/// @param[in] fontName the name of the font to use in the text rendering.
/// @param[in] size the size of the rendered text's individual glyphs.
/// @param[in] position the position to render the string at.
/// @param[in] color (optional) specifies the custom color of the rendered string.
/// @param[in] is3d (optional) specifies whether the rendered text is a gui element or part of the 3d world.
/// @param[in] entityName (optional) specifies the name of the entity holding the text component.
/// @returns the id of an entity holding the root TextStringComponent which contains all the character entities of the input string.
ecs::EntityId RenderText
(
    const std::string& text,
    const StringId& fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
    const bool is3d = false,
    const StringId entityName = StringId()
);

///------------------------------------------------------------------------------------------------
/// Renders a text string with the given font if the text is different from the previous entity's text. 
/// 
/// In case of inequality the old entity will be destroyed.
/// @param[in] text the text to render.
/// @param[in] previousString the entity hodling a TextStringComponent that the first argument will be compared against.
/// @param[in] fontName the name of the font to use in the text rendering.
/// @param[in] size the size of the rendered text's individual glyphs.
/// @param[in] position the position to render the string at.
/// @param[in] color (optional) specifies the custom color of the rendered string.
/// @returns the id of an entity holding the root TextStringComponent which contains all 
/// the character entities of the input string (will be the second argument in case of equality).
ecs::EntityId RenderTextIfDifferentToPreviousString
(
    const std::string& text,
    const ecs::EntityId previousString,
    const StringId& fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
);


///------------------------------------------------------------------------------------------------
/// Checks whether the TextStringComponent of the entity passed in represents the same string as the second argument. 
///
/// @param[in] textStringEntityId the id of the entity holding the root TextStringComponent of the text to be checked.
/// @param[in] textToTest the string to test against.
/// @returns whether or not the individual characters of the TextStringComponent held by the first argument represents 
/// the same string as the second argument.
bool IsTextStringTheSameAsText
(
    const ecs::EntityId textStringEntityId,
    const std::string& textToTest
);

///------------------------------------------------------------------------------------------------
/// Calculates the bounding AABB of the given text entity
///
/// @param[in] textStringEntityId the name of the entity holding the text string component.
/// @returns whether the bounding rect of the represented rendered string
genesis::math::Rectangle CalculateTextBoundingRect
(
    const ecs::EntityId textStringEntityId
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* FontUtils_h */
