#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <memory>

#include <PixPhetamine/Mesh.h>


namespace DendyEngine
{

class CTerrain: public PixPhetamine::IMesh
{
protected:
    float m_Scale{4.0f};
    static constexpr size_t c_TerrainSize{2048};
    static constexpr float c_TerrainMaxHeight{40};
    //static constexpr size_t c_TerrainSize{32};
    std::array<float,c_TerrainSize*c_TerrainSize> m_HeightsArray;
    
    // Rendering
    std::vector<GLfloat> m_NormalsVec;
    std::vector<GLfloat> m_TextureCoordinatesVec;
    std::vector<GLuint> m_FaceIndicesVec;

protected:
    inline float const _GetHeight(size_t const& index) const {return m_HeightsArray.at(index);}
    inline float const _GetHeight(size_t const& x, size_t const& y) const {return m_HeightsArray.at(y*c_TerrainSize+x);}
    inline void _SetHeight(size_t const& x, size_t const& y, float value) { m_HeightsArray.at(y*c_TerrainSize+x) = value; }

    void _InitialiseMesh();

public:
    CTerrain();
    ~CTerrain();

    inline constexpr float GetMaxHeight() const {return c_TerrainMaxHeight;}
    float GetHeightAtPosition(glm::vec2 const& position) const;
    float GetHeightAtPosition(glm::vec3 const& position) const;
    glm::vec3 GetNormalAtPosition(glm::vec2 const& position) const;
    glm::vec3 GetNormalAtPosition(glm::vec3 const& position) const;
    float ComputeDistance(glm::vec3 positionStart, glm::vec3 positionEnd) const {return 0.0f;}

    // Rendering
    void LoadToGPU() override;
    void UnloadFromGPU() override;
    GLuint GetTriangleCount() const override {return m_FaceIndicesVec.size();}

};

}