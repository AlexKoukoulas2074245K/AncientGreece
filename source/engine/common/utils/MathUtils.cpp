///------------------------------------------------------------------------------------------------
///  MathUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 28/02/2021.
///-----------------------------------------------------------------------------------------------

#include "MathUtils.h"

#include <SDL_mouse.h>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace math
{

///-----------------------------------------------------------------------------------------------

glm::vec3 ComputeMouseRayDirection(const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const float windowWidth, const float windowHeight)
{
    auto mouse_x = 0;
    auto mouse_y = 0;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    
    const auto& mouseX = mouse_x / (windowWidth  * 0.5f) - 1.0f;
    const auto& mouseY = mouse_y / (windowHeight * 0.5f) - 1.0f;

    const auto& invVP = glm::inverse(projMatrix * viewMatrix);
    const auto& screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
    const auto& worldPos = invVP * screenPos;
    
    return glm::normalize(glm::vec3(worldPos));
}

///------------------------------------------------------------------------------------------------

bool RayToSphereIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& sphereCenter, const float sphereRadius, float& t)
{
    const auto& radius2 = sphereRadius * sphereRadius;
    const auto& L = sphereCenter - rayOrigin;
    const auto& tca = glm::dot(L, rayDirection);
    const auto& d2 = glm::dot(L, L) - tca * tca;
    
    if (d2 > radius2) return false;
    const auto& thc = Sqrt(radius2 - d2);
    auto t0 = tca - thc;
    auto t1 = tca + thc;

    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0) return false; // both t0 and t1 are negative
    }

    t = t0;
    return true;
}

///------------------------------------------------------------------------------------------------

bool RayToPlaneIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& planeCenter, const glm::vec3& planeNormal, float& t)
{
    float denom = glm::dot(planeNormal, rayDirection);
    if (abs(denom) > 0.0001f) // your favorite epsilon
    {
        t = glm::dot(planeNormal, (planeCenter - rayOrigin)) / denom;
        if (t >= 0)
        {
            return true;
        }
    }
    return false;
}

///------------------------------------------------------------------------------------------------

bool IsMeshInsideFrustum(const glm::vec3& meshPosition, const glm::vec3& meshScale, const glm::vec3& meshDimensions, const Frustum& frustum)
{
    const auto scaledMeshDimensions = meshDimensions * meshScale;
    const auto frustumCheckSphereRadius = math::Max(scaledMeshDimensions.x, math::Max(scaledMeshDimensions.y, scaledMeshDimensions.z)) * 0.5f;

    for (auto i = 0U; i < 6U; ++i)
    {
        float dist =
            frustum[i].x * meshPosition.x +
            frustum[i].y * meshPosition.y +
            frustum[i].z * meshPosition.z +
            frustum[i].w - frustumCheckSphereRadius;

        if (dist > 0) return false;
    }

    return true;
}

///------------------------------------------------------------------------------------------------

}

}
