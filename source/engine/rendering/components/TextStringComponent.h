///------------------------------------------------------------------------------------------------
///  TextStringComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef TextStringComponent_h
#define TextStringComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

class CharacterEntry final
{
public:
    CharacterEntry(const ecs::EntityId entityId, const char character)
        : mEntityId(entityId)
        , mCharacter(character)
    {
    }

    const ecs::EntityId mEntityId;
    const char mCharacter;
};

///-----------------------------------------------------------------------------------------------

class TextStringComponent final: public ecs::IComponent
{
public:
    std::vector<CharacterEntry> mTextCharacterEntities;
    float mCharacterSize;
};

///-----------------------------------------------------------------------------------------------

}

}

#endif /* TextStringComponent_h */
