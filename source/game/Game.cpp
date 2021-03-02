///------------------------------------------------------------------------------------------------
///  Game.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "../engine/ECS.h"
#include "../engine/common/components/TransformComponent.h"
#include "../engine/common/utils/Logging.h"
#include "../engine/common/utils/MathUtils.h"
#include "../engine/debug/components/DebugViewStateSingletonComponent.h"
#include "../engine/debug/systems/ConsoleManagementSystem.h"
#include "../engine/debug/systems/DebugViewManagementSystem.h"
#include "../engine/debug/utils/ConsoleCommandUtils.h"
#include "../engine/input/components/InputStateSingletonComponent.h"
#include "../engine/input/utils/InputUtils.h"
#include "../engine/input/systems/RawInputHandlingSystem.h"
#include "../engine/rendering/components/CameraSingletonComponent.h"
#include "../engine/rendering/components/LightStoreSingletonComponent.h"
#include "../engine/rendering/components/RenderableComponent.h"
#include "../engine/rendering/components/WindowSingletonComponent.h"
#include "../engine/rendering/utils/CameraUtils.h"
#include "../engine/rendering/utils/FontUtils.h"
#include "../engine/rendering/utils/LightUtils.h"
#include "../engine/rendering/utils/MeshUtils.h"
#include "../engine/rendering/systems/RenderingSystem.h"
#include "../engine/resources/MeshResource.h"
#include "../engine/resources/ResourceLoadingService.h"
#include "../engine/scripting/components/ScriptComponent.h"
#include "../engine/scripting/service/LuaScriptingService.h"
#include "../engine/scripting/systems/ScriptingSystem.h"
#include "../engine/sound/SoundService.h"

///------------------------------------------------------------------------------------------------

void Game::VOnSystemsInit()
{
    auto& world = genesis::ecs::World::GetInstance();
    world.AddSystem(std::make_unique<genesis::input::RawInputHandlingSystem>());
    world.AddSystem(std::make_unique<genesis::scripting::ScriptingSystem>());

#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
    world.AddSystem(std::make_unique<genesis::debug::ConsoleManagementSystem>());
    world.AddSystem(std::make_unique<genesis::debug::DebugViewManagementSystem>());
#endif
    
    //world.AddSystem(std::make_unique<physics::PhysicsMovementApplicationSystem>());
    //world.AddSystem(std::make_unique<scene::SceneUpdaterSystem>());
    //world.AddSystem(std::make_unique<physics::PhysicsCollisionDetectionSystem>());
    //world.AddSystem(std::make_unique<physics::PhysicsCollisionResponseSystem>());
    
    world.AddSystem(std::make_unique<genesis::rendering::RenderingSystem>());
}

static bool IsMeshInsideCameraFrustum
(
    const glm::vec3& meshPosition,
    const glm::vec3& meshScale,
    const glm::vec3& meshDimensions,
    const genesis::rendering::CameraFrustum& cameraFrustum
)
{
    const auto scaledMeshDimensions = meshDimensions * meshScale;
    const auto frustumCheckSphereRadius = genesis::math::Max(scaledMeshDimensions.x, genesis::math::Max(scaledMeshDimensions.y, scaledMeshDimensions.z)) * 0.5f;

    for (auto i = 0U; i < 6U; ++i)
    {
        float dist =
            cameraFrustum[i].x * meshPosition.x +
            cameraFrustum[i].y * meshPosition.y +
            cameraFrustum[i].z * meshPosition.z +
            cameraFrustum[i].w - frustumCheckSphereRadius;

        if (dist > 0) return false;
    }

    return true;
}
///------------------------------------------------------------------------------------------------
/*
static void CreateSphereAtRandomPosition(const int i)
{
    const auto sphereEntityId = genesis::rendering::LoadAndCreateModelByName
    (
        "sphere",
        glm::vec3(-1.0f + i * 0.5f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.3f, 0.3f, 0.3f),
        StringId("sphere")
    );
    
    auto& world = genesis::ecs::World::GetInstance();
    
    auto& transformComponent = world.GetComponent<genesis::TransformComponent>(sphereEntityId);
    transformComponent.mRotation.y = genesis::math::RandomFloat(0.0f, genesis::math::PI);
    
    auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(sphereEntityId);
    auto& resource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceId);
    
    renderableComponent.mMaterial.mAmbient   = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    renderableComponent.mMaterial.mDiffuse   = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    renderableComponent.mMaterial.mSpecular  = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    renderableComponent.mMaterial.mShininess = 32.0f;
    renderableComponent.mIsAffectedByLight   = true;
    
    auto physicsComponent = std::make_unique<physics::PhysicsComponent>();
    physicsComponent->mCollidableDimensions = transformComponent.mScale * resource.GetDimensions();
    physicsComponent->mDirection = glm::vec3(genesis::math::RandomFloat(-1.0f, 1.0f), genesis::math::RandomFloat(-1.0f, 1.0f), 0.0f);
    physicsComponent->mDirection = glm::normalize(physicsComponent->mDirection);
    //physicsComponent->mVelocitySpeed = 0.2f;
    //physicsComponent->mRotationalSpeed = 0.3f;
    
    world.AddComponent<physics::PhysicsComponent>(sphereEntityId, std::move(physicsComponent));
}
*/
void Game::VOnGameInit()
{
    genesis::rendering::LoadAndCreateModelByName("map", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map"));
    genesis::rendering::LoadAndCreateModelByName("map_edge", glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_1"));
    genesis::rendering::LoadAndCreateModelByName("map_edge", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_2"));
    genesis::rendering::LoadAndCreateModelByName("map_edge", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_3"));
    genesis::rendering::LoadAndCreateModelByName("map_edge", glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), StringId("map_edge_4"));
    genesis::rendering::AddLightSource(glm::vec3(0.0f, 0.0f, 1.0f), 4.0f);
    //genesis::rendering::AddLightSource(glm::vec3(0.0f, 4.0f, 0.0f), 4.0f);
    genesis::rendering::AddLightSource(glm::vec3(2.0f, 2.0f, 0.0f), 4.0f);
}

///------------------------------------------------------------------------------------------------

static float dtAccum = 0.0f;

void Game::VOnUpdate(const float dt)
{
    auto& world = genesis::ecs::World::GetInstance();
    
    const auto& windowComponent = world.GetSingletonComponent<genesis::rendering::WindowSingletonComponent>();
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    auto& lightStoreComponent = world.GetSingletonComponent<genesis::rendering::LightStoreSingletonComponent>();
    
    dtAccum += dt;
    lightStoreComponent.mLightPositions[0].x = genesis::math::Sinf(dtAccum/2) * 2;
    lightStoreComponent.mLightPositions[0].z = genesis::math::Cosf(dtAccum/2) * 2;
    
    float moveSpeed = 0.2f;
    auto cameraPosition = cameraComponent.mPosition;
    auto maxZ = -0.20f;
    auto minZ = -0.60f;
    
    if (genesis::input::GetKeyState(genesis::input::Key::Q_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition.y += moveSpeed * dt;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::E_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition.y -= moveSpeed * dt;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::A_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition += dt * moveSpeed * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::GetKeyState(genesis::input::Key::D_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition -= dt * moveSpeed * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::GetKeyState(genesis::input::Key::W_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition += dt * moveSpeed * cameraComponent.mFrontVector;
    }
    if (genesis::input::GetKeyState(genesis::input::Key::S_KEY) == genesis::input::InputState::PRESSED)
    {
        cameraComponent.mPosition -= dt * moveSpeed * cameraComponent.mFrontVector;
    }
 
    auto isEdgeVisible = false;
    for (int i = 1; i < 5; ++i) {
        auto entityId = world.FindEntityWithName(StringId("map_edge_" + std::to_string(i)));
        // Calculate render-constant camera view matrix
        auto viewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mPosition + cameraComponent.mFrontVector, cameraComponent.mUpVector);
        
        // Calculate render-constant camera projection matrix
        auto projectionMatrix = glm::perspectiveFovLH
        (
            cameraComponent.mFieldOfView,
            windowComponent.mRenderableWidth,
            windowComponent.mRenderableHeight,
            cameraComponent.mZNear,
            cameraComponent.mZFar
        );
        
        // Calculate the camera frustum for this frame
        auto frustum = genesis::rendering::CalculateCameraFrustum(viewMatrix, projectionMatrix);
        
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        const auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        const auto& currentMesh        = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceId);
        
        if (IsMeshInsideCameraFrustum(transformComponent.mPosition, transformComponent.mScale, currentMesh.GetDimensions(), frustum)) {
            isEdgeVisible = true;
            break;
        }
    }
    
    auto exceededZLimits = cameraComponent.mPosition.z < minZ || cameraComponent.mPosition.z > maxZ;
    if (isEdgeVisible || exceededZLimits) {
        cameraComponent.mPosition = cameraPosition;
    }
    
    cameraComponent.mFrontVector.x = genesis::math::Cosf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.y = genesis::math::Sinf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.z = genesis::math::Sinf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
    
    if (genesis::input::GetButtonState(genesis::input::Button::LEFT_BUTTON) == genesis::input::InputState::PRESSED) {
        Log(LogType::INFO, "Mouse lbutton pressed");
    }
    if (genesis::input::GetButtonState(genesis::input::Button::LEFT_BUTTON) == genesis::input::InputState::RELEASED) {
        Log(LogType::INFO, "Mouse lbutton released");
    }
}

///------------------------------------------------------------------------------------------------

