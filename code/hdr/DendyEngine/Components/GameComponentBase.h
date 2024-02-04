#pragma once

#include <glm/glm.hpp>

namespace DendyEngine::Components
{

enum class EGameComponentType;

class IGameComponent
{
public:
    virtual ~IGameComponent() = default;
};

template<typename TGameComponent, EGameComponentType TGameComponentType>
class CGameComponent : public IGameComponent
{
public:
    static constexpr EGameComponentType Type = TGameComponentType;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Define Enum
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


enum class EGameComponentType
{
    ScenePose,
    Transform,
    TerrainChunk,
    WalkingCharacter,
    Vision,
    Visibility,
    RenderablePawn,
    Kossack,
    StaticMesh,
    Camera,
    Collider
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Define Data
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct SScenePose : CGameComponent<SScenePose,EGameComponentType::ScenePose>
{
    glm::vec2 Position{0.0f};
    glm::vec2 Orientation{0.0f, 1.0f};

    SScenePose(glm::vec2 const& position, glm::vec2 const& orientation): Position(position), Orientation(orientation) {}
};


}