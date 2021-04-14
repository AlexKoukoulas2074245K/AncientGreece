///------------------------------------------------------------------------------------------------
///  DiamondSquareHeightMapGenerator.h
///  Genesis
///
///  Created by Alex Koukoulas on 14/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef DiamondSquareHeightMapGenerator_h
#define DiamondSquareHeightMapGenerator_h

///-----------------------------------------------------------------------------------------------

#include <SDL_surface.h>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------
class DiamondSquareHeightMapGenerator
{
public:
    static SDL_Surface* GenerateRandomHeightMap(const float roughness);
    
private:
    DiamondSquareHeightMapGenerator() = default;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* DiamondSquareHeightMapGenerator_h */
