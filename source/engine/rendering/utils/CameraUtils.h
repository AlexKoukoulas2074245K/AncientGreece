///------------------------------------------------------------------------------------------------
///  CameraUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef CameraUtils_h
#define CameraUtils_h

///-----------------------------------------------------------------------------------------------

#include "../../common/utils/MathUtils.h"
#include "../components/CameraSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------
/// Calculates the camera frustum from the given view and projection matrices
///
/// @param[in] viewMatrix the view matrix of the camera.
/// @param[in] projMatrix the projection matrix of the camera.
/// @returns the calculated camera frustum.
CameraFrustum CalculateCameraFrustum(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* CameraUtis_h */
