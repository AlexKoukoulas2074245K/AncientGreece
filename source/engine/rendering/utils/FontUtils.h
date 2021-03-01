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
/// Renders a single character with the given font. 
///
/// @param[in] character the character to render.
/// @param[in] fontName the name of the font to use in rendering.
/// @param[in] size the size of character.
/// @param[in] position the position to render the character at.
/// @param[in] color (optional) specifies the custom color of the rendered character.
/// @returns the id of an entity holding the renderable and transform components of the character.
ecs::EntityId RenderCharacter
(
    const char character,
    const StringId& fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
);

///------------------------------------------------------------------------------------------------
/// Renders a text string with the given font. 
///
/// @param[in] text the text to render.
/// @param[in] fontName the name of the font to use in the text rendering.
/// @param[in] size the size of the rendered text's individual glyphs.
/// @param[in] position the position to render the string at.
/// @param[in] color (optional) specifies the custom color of the rendered string.
/// @returns the id of an entity holding the root TextStringComponent which contains all the character entities of the input string.
ecs::EntityId RenderText
(
    const std::string& text,
    const StringId& fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
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
/// Moves a text string by a certain displacement.
///
/// @param[in] textStringEntityId the id of the entity holding the root TextStringComponent of the text to be moved.
/// @param[in] dx the horizontal displacement
/// @param[in] dy the vertical displacement
void MoveText
(
    const ecs::EntityId textStringEntityId,
    const float dx = 0.0f,
    const float dy = 0.0f
);

///------------------------------------------------------------------------------------------------
/// Sets the position of a text string.
///
/// @param[in] textStringEntityId the id of the entity holding the root TextStringComponent of the text to be moved.
/// @param[in] position the target position of the text
void SetTextPosition
(
    const ecs::EntityId textStringEntityId,
    const glm::vec3& position
);

///------------------------------------------------------------------------------------------------
/// Clears a text string. 
///
/// @param[in] textStringEntityId the id of the entity holding the root TextStringComponent of the text to be cleared.
void DestroyRenderedText
(
    const ecs::EntityId textStringEntityId
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

}

}

///------------------------------------------------------------------------------------------------

#endif /* FontUtils_h */
