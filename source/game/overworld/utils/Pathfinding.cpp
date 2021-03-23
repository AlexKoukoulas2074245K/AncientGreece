///------------------------------------------------------------------------------------------------
///  Pathfinding.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 22/03/2021.
///------------------------------------------------------------------------------------------------

#include "Pathfinding.h"
#include "NavmapUtils.h"
#include "../../../engine/resources/TextureResource.h"

#include <algorithm>
#include <queue>
#include <tsl/robin_map.h>
#include <unordered_set>
#include <vector>

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

struct Tile
{
    int mX;
    int mY;
    int mFScore;
    int mGScore;
    AreaTypeMask mAreaTypeMask;
};

bool operator == (const Tile& lhs, const Tile& rhs)
{
    return lhs.mX == rhs.mX && lhs.mY == rhs.mY;
}

bool operator < (const Tile& lhs, const Tile& rhs)
{
    // Reverse less for min heap
    return lhs.mFScore > rhs.mFScore;
}

struct PriorityQueueHasher
{
    bool operator()(const Tile* a, const Tile* b)
    {
        return a->mFScore > b->mFScore;
    }
};

struct TileHasher
{
    TileHasher(const int cols)
        : mCols(cols){}
    
    std::size_t operator() (const Tile& tile) const
    {
        return tile.mY * mCols + tile.mX;
    }
    
    std::hash<int> hashFunction;
    int mCols;
};

///------------------------------------------------------------------------------------------------

class PriorityTileQueue
{
public:
    inline void push(Tile* tile)
    {
        mTiles.push_back(tile);
        std::push_heap(mTiles.begin(), mTiles.end(), PriorityQueueHasher());
    }
    
    inline Tile* front()
    {
        return mTiles.front();
    }
    
    inline bool empty() const
    {
        return mTiles.empty();
    }
    
    inline void pop()
    {
        std::pop_heap(mTiles.begin(), mTiles.end(), PriorityQueueHasher());
        mTiles.pop_back();
    }
    
    inline bool contains(const Tile* tile)
    {
        return std::find(mTiles.cbegin(), mTiles.cend(), tile) != mTiles.cend();
    }
    
private:
    std::vector<Tile*> mTiles;
};


///------------------------------------------------------------------------------------------------

int CalculateHeuristic(const Tile* a, const Tile* b)
{
    return genesis::math::Abs(a->mX - b->mX) + genesis::math::Abs(a->mY - b->mY);
}

///------------------------------------------------------------------------------------------------

void FindNeighbours(const Tile* tile,  const AreaTypeMask& unitNavigableAreaMask, const int rows, const int cols, Tile** tileMap, std::list<Tile*>& neighbours)
{
    const auto hasLeftNeighbour  = tile->mX - 1 >= 0;
    const auto hasRightNeighbour = tile->mX + 1 <= cols - 1;
    const auto hasTopNeighbour   = tile->mY - 1 >= 0;
    const auto hasBotNeighbour   = tile->mY + 1 <= rows - 1;
    
    if (hasLeftNeighbour && (unitNavigableAreaMask & tileMap[tile->mY][tile->mX - 1].mAreaTypeMask) != 0)
    {
        neighbours.push_back(&tileMap[tile->mY][tile->mX - 1]);
    }
    if (hasRightNeighbour && (unitNavigableAreaMask & tileMap[tile->mY][tile->mX + 1].mAreaTypeMask) != 0)
    {
        neighbours.push_back(&tileMap[tile->mY][tile->mX + 1]);
    }
    if (hasTopNeighbour && (unitNavigableAreaMask & tileMap[tile->mY - 1][tile->mX].mAreaTypeMask) != 0)
    {
        neighbours.push_back(&tileMap[tile->mY - 1][tile->mX]);
    }
    if (hasBotNeighbour && (unitNavigableAreaMask & tileMap[tile->mY + 1][tile->mX].mAreaTypeMask) != 0)
    {
        neighbours.push_back(&tileMap[tile->mY + 1][tile->mX]);
    }
    if (hasLeftNeighbour && hasTopNeighbour && (unitNavigableAreaMask & tileMap[tile->mY - 1][tile->mX - 1].mAreaTypeMask) != 0)
    {
        neighbours.push_back(&tileMap[tile->mY - 1][tile->mX - 1]);
    }
    if (hasLeftNeighbour && hasBotNeighbour && (unitNavigableAreaMask & tileMap[tile->mY + 1][tile->mX - 1].mAreaTypeMask) != 0)
    {
        neighbours.push_back(&tileMap[tile->mY + 1][tile->mX - 1]);
    }
    if (hasRightNeighbour && hasTopNeighbour && (unitNavigableAreaMask & tileMap[tile->mY - 1][tile->mX + 1].mAreaTypeMask) != 0)
    {
        neighbours.push_back(&tileMap[tile->mY - 1][tile->mX + 1]);
    }
    if (hasRightNeighbour && hasBotNeighbour && (unitNavigableAreaMask & tileMap[tile->mY + 1][tile->mX + 1].mAreaTypeMask) != 0)
    {
        neighbours.push_back(&tileMap[tile->mY + 1][tile->mX + 1]);
    }
}

///------------------------------------------------------------------------------------------------

void Cleanup(const int rows, Tile** tileMap)
{
    for (int y = 0; y < rows; ++y)
    {
        delete[] tileMap[y];
    }
    
    delete[] tileMap;
}

///------------------------------------------------------------------------------------------------

std::list<glm::vec3> FindPath(const glm::vec3& startPos, const glm::vec3& endPos, const AreaTypeMask& unitNavigableAreaMask, const glm::vec3& mapDimensions, const genesis::resources::TextureResource& navmapTexture)
{
    std::list<glm::vec3> resultPath;
    
    const auto cols = navmapTexture.GetDimensions().x;
    const auto rows = navmapTexture.GetDimensions().y;

    const auto startPixel = MapPositionToNavmapPixel(startPos, mapDimensions, navmapTexture.GetDimensions());
    const auto endPixel = MapPositionToNavmapPixel(endPos, mapDimensions, navmapTexture.GetDimensions());
    
    if (startPixel.x == endPixel.x && startPixel.y == endPixel.y)
        return resultPath;
    
    const auto Inf = rows * cols;
    Tile** tileMap = new Tile*[rows];
    for (int y = 0; y < rows; ++y)
    {
        tileMap[y] = new Tile[cols];
        for (int x = 0; x < cols; ++x)
        {
            tileMap[y][x].mX = x;
            tileMap[y][x].mY = y;
            tileMap[y][x].mGScore = Inf;
            tileMap[y][x].mFScore = Inf;
            auto rgbTriplet = navmapTexture.GetRGBatPixel(x, y);
            tileMap[y][x].mAreaTypeMask = RGB_TO_AREA_TYPE_MASK.count(rgbTriplet) > 0 ? RGB_TO_AREA_TYPE_MASK.at(rgbTriplet) : areaTypeMasks::NEUTRAL;
        }
    }
    
    Tile& startTile = tileMap[startPixel.y][startPixel.x];
    Tile& endTile = tileMap[endPixel.y][endPixel.x];
    
    startTile.mGScore = 0;
    startTile.mFScore = CalculateHeuristic(&startTile, &endTile);
    
    tsl::robin_map<Tile*, Tile*> path;
    std::unordered_set<Tile*> closedSet;
    PriorityTileQueue openSet;
    openSet.push(&startTile);
    
    while (!openSet.empty())
    {
        auto* current = openSet.front();
        
        if (current == &endTile)
        {
            for (auto* tile = &endTile; path.count(tile); tile = path.at(tile))
            {
                resultPath.push_front(NavmapPixelToMapPosition(glm::ivec2(tile->mX, tile->mY), mapDimensions, navmapTexture.GetDimensions()));
            }
            resultPath.push_back(endPos);
            
            Cleanup(rows, tileMap);
            return resultPath;
        }
        
        openSet.pop();
        closedSet.insert(current);
        
        std::list<Tile*> neighbours;
        FindNeighbours(current, unitNavigableAreaMask, rows, cols, tileMap, neighbours);
        while (!neighbours.empty())
        {
            auto* neighbour = neighbours.front();
            if (closedSet.count(neighbour) > 0)
            {
                neighbours.pop_front();
                continue;
            }
            
            if (!openSet.contains(neighbour))
            {
                openSet.push(neighbour);
            }
            
            auto tentativeGScore = current->mGScore + 1;
            if (tentativeGScore >= neighbour->mGScore)
            {
                neighbours.pop_front();
                continue;
            }
            
            path[neighbour] = current;
            neighbour->mGScore = tentativeGScore;
            neighbour->mFScore = tentativeGScore + CalculateHeuristic(neighbour, &endTile);
            neighbours.pop_front();
        }
    }
    
    Cleanup(rows, tileMap);
    return resultPath;
}


///-----------------------------------------------------------------------------------------------

}

