///------------------------------------------------------------------------------------------------
///  DiamondSquareHeightMapGenerator.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 14/04/2021.
///-----------------------------------------------------------------------------------------------

#include "DiamondSquareHeightMapGenerator.h"
#include "../../common/utils/MathUtils.h"

#include <tsl/robin_map.h>
#include <SDL_surface.h>
#include <utility>
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static constexpr int DIAMOND_SQUARE_HEIGHTMAP_MAX = 257;

    static const int CORNER_INDICES[4][2] = { {0, 0}, {DIAMOND_SQUARE_HEIGHTMAP_MAX - 1, 0}, {DIAMOND_SQUARE_HEIGHTMAP_MAX - 1, DIAMOND_SQUARE_HEIGHTMAP_MAX - 1}, {0, DIAMOND_SQUARE_HEIGHTMAP_MAX - 1} };
    
    static const float INIT_CORNER_VALUES[4]  = {1.0f, 4.0f, 4.0f, 1.0f};
    static const float HEIGHT_MAP_OFFSET                 = 0.0f;
    static const float HEIGHT_MAP_RANGE                  = 10.0f;
    static const float MIN_VALUE                         = 1.0f;
    static const float HEIGHT_MAP_ACCEPTABLE_COLOR_RATIO = 0.8f;
    static const float EPSILON_VALUE                     = 0.00001f;

    static constexpr int RGB_SAND_RANGE       = 5;
    static constexpr int RGB_SAND_GRASS_RANGE = 12;
    static constexpr int RGB_GRASS_RANGE      = 25;
    static constexpr int RGB_GRASS_ROCK_RANGE = 63;
    static constexpr int RGB_ROCK_RANGE       = 89;
    static constexpr int RGB_ROCK_SNOW_RANGE  = 140;
    static constexpr int RGB_SNOW_RANGE       = 191;

    static const tsl::robin_map<int, int> DIAMOND_SQUARE_VALUE_TO_RANGE =
    {
        { 1, RGB_SAND_RANGE },
        { 2, RGB_SAND_GRASS_RANGE },
        { 3, RGB_GRASS_RANGE },
        { 4, RGB_GRASS_ROCK_RANGE },
        { 5, RGB_ROCK_RANGE },
        { 6, RGB_ROCK_SNOW_RANGE },
        { 7, RGB_SNOW_RANGE }
    };
}

///-----------------------------------------------------------------------------------------------

struct DiamondSquareHeightMap
{
    float** mData;
};

///-----------------------------------------------------------------------------------------------

struct DenoisedValueChange
{
    int col;
    int row;
    float val;
};

///-----------------------------------------------------------------------------------------------

float Average4(float a, float b, float c, float d)
{
    return (a + b + c + d)/4.0f;
}

///-----------------------------------------------------------------------------------------------

float GetValue(const int col, const int row, float** data)
{
    if (col < 0 || col > DIAMOND_SQUARE_HEIGHTMAP_MAX - 1 || row < 0 || row > DIAMOND_SQUARE_HEIGHTMAP_MAX -1) return Average4(INIT_CORNER_VALUES[0], INIT_CORNER_VALUES[1], INIT_CORNER_VALUES[2], INIT_CORNER_VALUES[3]);
    return data[row][col];
}

///-----------------------------------------------------------------------------------------------

void SetValue(const int col, const int row, const float val, float** data)
{
    data[row][col] = val;
}

///-----------------------------------------------------------------------------------------------

bool NotCorner(const int col, const int row)
{
    return col != 0 && row != 0 && col != DIAMOND_SQUARE_HEIGHTMAP_MAX - 1 && row != DIAMOND_SQUARE_HEIGHTMAP_MAX - 1;
}

///-----------------------------------------------------------------------------------------------

float FitValueInRange(const float val, const float m1, const float m2)
{
    float min = 0.0f;
    float max = 0.0f;
    if (m1 <= m2)
    {
        min = m1;
        max = m2;
    }
    else
    {
        min = m2;
        max = m1;
    }
    
    return val > max ? max : (val < min ? min : val);
}

///-----------------------------------------------------------------------------------------------

float RandomizeValue(const float val, const float p)
{
    return val * (p + (1.0f - p) * genesis::math::RandomFloat());
}

///-----------------------------------------------------------------------------------------------

float GetSign(const float value)
{
    return genesis::math::Abs(value) < EPSILON_VALUE ? GetSign(genesis::math::RandomFloat() - 0.5f) : genesis::math::Abs(value) / value;
}

///-----------------------------------------------------------------------------------------------

float GenerateRandomValue(const float average, const int size, const float roughness)
{
    const auto gap = ((HEIGHT_MAP_RANGE - MIN_VALUE) / 2.0f - (average - MIN_VALUE)) / ((HEIGHT_MAP_RANGE - MIN_VALUE) / 2.0f);
    const auto distance = size/DIAMOND_SQUARE_HEIGHTMAP_MAX;
    const auto offset = (RandomizeValue(HEIGHT_MAP_OFFSET, 0.8) * 0.8 + RandomizeValue(GetSign(gap) * roughness, 0.9) * 0.2) * RandomizeValue(distance, 0.5) * 4;
    const auto roughnessVal = (genesis::math::RandomFloat() - 0.5f) * roughness * size * 0.02f;
    return average * (offset + roughnessVal);
}

///-----------------------------------------------------------------------------------------------

float GetNewValue(const float average, const int size, const float roughness)
{
    return FitValueInRange(std::round(average + GenerateRandomValue(average, size, roughness)), MIN_VALUE, HEIGHT_MAP_RANGE);
}

///-----------------------------------------------------------------------------------------------

void InitData(float** data)
{
    for (int row = 0; row < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++row)
    {
        for (int col = 0; col < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++col)
        {
            SetValue(col, row, 0.0f, data);
        }
    }
    
    SetValue(CORNER_INDICES[0][1], CORNER_INDICES[0][0], INIT_CORNER_VALUES[0], data);
    SetValue(CORNER_INDICES[1][1], CORNER_INDICES[1][0], INIT_CORNER_VALUES[1], data);
    SetValue(CORNER_INDICES[2][1], CORNER_INDICES[2][0], INIT_CORNER_VALUES[2], data);
    SetValue(CORNER_INDICES[3][1], CORNER_INDICES[3][0], INIT_CORNER_VALUES[3], data);
}

///-----------------------------------------------------------------------------------------------

void Square(const int col, const int row, const int halfSize, const float roughness, float** data)
{
    const auto avg = Average4
    (
        GetValue(col - halfSize, row - halfSize, data),
        GetValue(col + halfSize, row - halfSize, data),
        GetValue(col + halfSize, row + halfSize, data),
        GetValue(col - halfSize, row + halfSize, data)
    );
    SetValue(col, row, GetNewValue(avg, halfSize, roughness), data);
}

///-----------------------------------------------------------------------------------------------

void Diamond(const int col, const int row, const int halfSize, const float roughness, float** data)
{
    const auto avg = Average4
    (
        GetValue(col           , row - halfSize, data),
        GetValue(col + halfSize, row           , data),
        GetValue(col           , row + halfSize, data),
        GetValue(col - halfSize, row           , data)
    );
    SetValue(col, row, GetNewValue(avg, halfSize, roughness), data);
}

///-----------------------------------------------------------------------------------------------

bool AllNeighboursSame(const int col, const int row, float** data)
{
    return static_cast<int>(GetValue(col, row, data)) == static_cast<int>(GetValue(col, row, data)) == static_cast<int>(GetValue(col, row, data)) == static_cast<int>(GetValue(col, row, data));
}

///-----------------------------------------------------------------------------------------------

void DiamondSquare(const int size, const float roughness, float** data)
{
    const auto halfSize = size/2;
    if (halfSize < 1) return;
    
    for (int row = halfSize; row < DIAMOND_SQUARE_HEIGHTMAP_MAX; row += size)
    {
        for (int col = halfSize; col < DIAMOND_SQUARE_HEIGHTMAP_MAX; col += size)
        {
            if (NotCorner(col, row))
            {
                Square(col, row, halfSize, roughness, data);
            }
        }
    }
    
    for (int row = 0; row < DIAMOND_SQUARE_HEIGHTMAP_MAX; row += halfSize)
    {
        for (int col = (row + halfSize) % size; col < DIAMOND_SQUARE_HEIGHTMAP_MAX; col += size)
        {
            if (NotCorner(col, row))
            {
                Diamond(col, row, halfSize, roughness, data);
            }
        }
    }
    
    DiamondSquare(size / 2, roughness, data);
}

///-----------------------------------------------------------------------------------------------

void DenoiseHeightMap(float** data)
{
    std::vector<DenoisedValueChange> denoisedValueChanges;
    for (int y = 0; y < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++y)
    {
        for (int x = 0; x < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++x)
        {
            if (NotCorner(x, y))
            {
                if (AllNeighboursSame(x, y, data) && static_cast<int>(data[y][x]) != static_cast<int>(data[y][x - 1]))
                {
                    denoisedValueChanges.push_back({ x, y, data[y][x - 1] });
                }
            }
        }
    }
    
    for (const auto& denoisedValueChange: denoisedValueChanges)
    {
        data[denoisedValueChange.row][denoisedValueChange.col] = denoisedValueChange.val;
    }
}

///-----------------------------------------------------------------------------------------------

bool CheckHeightMapColorDistribution(float** data)
{
    tsl::robin_map<int, int> colorDistribution;
    const auto numPixels = DIAMOND_SQUARE_HEIGHTMAP_MAX * DIAMOND_SQUARE_HEIGHTMAP_MAX;
    for (int y = 0; y < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++y)
    {
        for (int x = 0; x < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++x)
        {
            const auto val = static_cast<int>(data[y][x]);
            if (!colorDistribution.count(val))
            {
                colorDistribution[val] = 1;
            }
            else
            {
                colorDistribution[val]++;
                if (colorDistribution[val] > static_cast<int>(numPixels * HEIGHT_MAP_ACCEPTABLE_COLOR_RATIO))
                {
                    return false;
                }
            }
        }
    }
    
    return true;
}

///-----------------------------------------------------------------------------------------------

SDL_Surface* GenerateHeightMapSurface(float** data)
{
    char* pixels = new char [ 4 * DIAMOND_SQUARE_HEIGHTMAP_MAX * DIAMOND_SQUARE_HEIGHTMAP_MAX];
    int i = 0;
    for (int y = 0; y < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++y)
    {
        for (int x = 0; x < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++x)
        {
            auto intValue = static_cast<int>(data[y][x]);
            if (intValue == 0) intValue = 1;
            const auto respectiveRange = DIAMOND_SQUARE_VALUE_TO_RANGE.count(intValue) ? DIAMOND_SQUARE_VALUE_TO_RANGE.at(intValue) : RGB_SNOW_RANGE;
           
            pixels[i++] = respectiveRange;
            pixels[i++] = respectiveRange;
            pixels[i++] = respectiveRange;
            pixels[i++] = 255;
        }
    }
   
    SDL_Surface* generatedSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, DIAMOND_SQUARE_HEIGHTMAP_MAX, DIAMOND_SQUARE_HEIGHTMAP_MAX, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
   
    for (int i = 0 ; i < DIAMOND_SQUARE_HEIGHTMAP_MAX ; i++)
    {
       std::memcpy( ((char *) generatedSurface->pixels) + generatedSurface->pitch * i, pixels + 4 * DIAMOND_SQUARE_HEIGHTMAP_MAX * (DIAMOND_SQUARE_HEIGHTMAP_MAX - i - 1), DIAMOND_SQUARE_HEIGHTMAP_MAX * 4);
    }
       
    SDL_SaveBMP(generatedSurface, "last_generated_heightmap.bmp");
    
    delete [] pixels;
    
    return generatedSurface;
}

SDL_Surface* DiamondSquareHeightMapGenerator::GenerateRandomHeightMap(const float roughness)
{
    DiamondSquareHeightMap result;
    result.mData = new float*[DIAMOND_SQUARE_HEIGHTMAP_MAX];
    for (int i = 0; i < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++i)
    {
        result.mData[i] = new float[DIAMOND_SQUARE_HEIGHTMAP_MAX];
    }
    
    while (true)
    {
        InitData(result.mData);
        DiamondSquare(DIAMOND_SQUARE_HEIGHTMAP_MAX - 1, roughness, result.mData);
        DenoiseHeightMap(result.mData);
        
        if (CheckHeightMapColorDistribution(result.mData))
        {
            break;
        }
    }
    
    auto generatedSurface = GenerateHeightMapSurface(result.mData);
    
    for (int y = 0; y < DIAMOND_SQUARE_HEIGHTMAP_MAX; ++y)
    {
        delete[] result.mData[y];
    }
    delete[] result.mData;
    
    return generatedSurface;
}

///-----------------------------------------------------------------------------------------------

}

}
