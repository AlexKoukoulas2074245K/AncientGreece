///------------------------------------------------------------------------------------------------
///  OverworldUnitAiUpdaterSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 25/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldUnitAiUpdaterSystem_h
#define OverworldUnitAiUpdaterSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------

class OverworldUnitAiComponent;

///-----------------------------------------------------------------------------------------------
class OverworldUnitAiUpdaterSystem final : public genesis::ecs::BaseSystem<OverworldUnitAiComponent>
{
public:
    OverworldUnitAiUpdaterSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldUnitAIUpdaterSystem_h */
