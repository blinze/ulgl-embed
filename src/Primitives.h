#pragma once

#include "Vertex.h"
#include <vector>
#include <cstdint>
#include <cmath>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

enum class PrimitiveType
{
    Cube,
    Pyramid,
    Cylinder,
    Cone,
    Count
};

inline const char* PrimitiveTypeToString(PrimitiveType type)
{
    switch (type)
    {
        case PrimitiveType::Cube:     return "cube";
        case PrimitiveType::Pyramid:  return "pyramid";
        case PrimitiveType::Cylinder: return "cylinder";
        case PrimitiveType::Cone:     return "cone";
        default:                      return "cube";
    }
}

inline PrimitiveType StringToPrimitiveType(const std::string& str)
{
    if (str == "cube")     return PrimitiveType::Cube;
    if (str == "pyramid")  return PrimitiveType::Pyramid;
    if (str == "cylinder") return PrimitiveType::Cylinder;
    if (str == "cone")     return PrimitiveType::Cone;
    return PrimitiveType::Cube;
}

namespace Primitives
{
    inline Vertex MakeVertex(float px, float py, float pz, float cr, float cg, float cb, float tu, float tv)
    {
        Vertex v;
        v.position[0] = px; v.position[1] = py; v.position[2] = pz;
        v.color[0]    = cr; v.color[1] = cg; v.color[2] = cb;
        v.texCoord[0] = tu; v.texCoord[1] = tv;
        return v;
    }

    inline Mesh CreateCube()
    {
        Mesh mesh;
        mesh.vertices.reserve(24);
        mesh.indices.reserve(36);
        
        // Front face (red) - z = +0.5
        mesh.vertices.push_back(MakeVertex(-0.5f, -0.5f,  0.5f,  1.0f, 0.3f, 0.3f,  0.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f, -0.5f,  0.5f,  1.0f, 0.3f, 0.3f,  1.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.5f,  1.0f, 1.0f));
        mesh.vertices.push_back(MakeVertex(-0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.5f,  0.0f, 1.0f));
        
        // Back face (green) - z = -0.5
        mesh.vertices.push_back(MakeVertex(-0.5f, -0.5f, -0.5f,  0.3f, 1.0f, 0.3f,  1.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex(-0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 0.5f,  1.0f, 1.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 0.5f,  0.0f, 1.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f, -0.5f, -0.5f,  0.3f, 1.0f, 0.3f,  0.0f, 0.0f));
        
        // Top face (blue) - y = +0.5
        mesh.vertices.push_back(MakeVertex(-0.5f,  0.5f, -0.5f,  0.3f, 0.3f, 1.0f,  0.0f, 1.0f));
        mesh.vertices.push_back(MakeVertex(-0.5f,  0.5f,  0.5f,  0.3f, 0.3f, 1.0f,  0.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 1.0f,  1.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 1.0f,  1.0f, 1.0f));
        
        // Bottom face (yellow) - y = -0.5
        mesh.vertices.push_back(MakeVertex(-0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.3f,  0.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.3f,  1.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  1.0f, 1.0f));
        mesh.vertices.push_back(MakeVertex(-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.0f, 1.0f));
        
        // Right face (magenta) - x = +0.5
        mesh.vertices.push_back(MakeVertex( 0.5f, -0.5f, -0.5f,  1.0f, 0.3f, 1.0f,  1.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 1.0f,  1.0f, 1.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 1.0f,  0.0f, 1.0f));
        mesh.vertices.push_back(MakeVertex( 0.5f, -0.5f,  0.5f,  1.0f, 0.3f, 1.0f,  0.0f, 0.0f));
        
        // Left face (cyan) - x = -0.5
        mesh.vertices.push_back(MakeVertex(-0.5f, -0.5f, -0.5f,  0.3f, 1.0f, 1.0f,  0.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex(-0.5f, -0.5f,  0.5f,  0.3f, 1.0f, 1.0f,  1.0f, 0.0f));
        mesh.vertices.push_back(MakeVertex(-0.5f,  0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  1.0f, 1.0f));
        mesh.vertices.push_back(MakeVertex(-0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 1.0f,  0.0f, 1.0f));
        
        // Indices for all 6 faces (2 triangles each)
        mesh.indices.push_back(0);  mesh.indices.push_back(1);  mesh.indices.push_back(2);
        mesh.indices.push_back(2);  mesh.indices.push_back(3);  mesh.indices.push_back(0);   // Front
        mesh.indices.push_back(4);  mesh.indices.push_back(5);  mesh.indices.push_back(6);
        mesh.indices.push_back(6);  mesh.indices.push_back(7);  mesh.indices.push_back(4);   // Back
        mesh.indices.push_back(8);  mesh.indices.push_back(9);  mesh.indices.push_back(10);
        mesh.indices.push_back(10); mesh.indices.push_back(11); mesh.indices.push_back(8);   // Top
        mesh.indices.push_back(12); mesh.indices.push_back(13); mesh.indices.push_back(14);
        mesh.indices.push_back(14); mesh.indices.push_back(15); mesh.indices.push_back(12);  // Bottom
        mesh.indices.push_back(16); mesh.indices.push_back(17); mesh.indices.push_back(18);
        mesh.indices.push_back(18); mesh.indices.push_back(19); mesh.indices.push_back(16);  // Right
        mesh.indices.push_back(20); mesh.indices.push_back(21); mesh.indices.push_back(22);
        mesh.indices.push_back(22); mesh.indices.push_back(23); mesh.indices.push_back(20);  // Left
        
        return mesh;
    }

    inline Mesh CreateQuad()
    {
        Mesh mesh;
        
        mesh.vertices = {
            {{ -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }},
            {{  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }},
            {{  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }},
            {{ -1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }}
        };
        
        mesh.indices = { 0, 1, 2, 2, 3, 0 };
        
        return mesh;
    }

    inline Mesh CreateQuadFlippedUV()
    {
        Mesh mesh;
        
        mesh.vertices = {
            {{ -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }},
            {{  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }},
            {{  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }},
            {{ -1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }}
        };
        
        mesh.indices = { 0, 1, 2, 2, 3, 0 };
        
        return mesh;
    }

    inline Mesh CreatePyramid()
    {
        Mesh mesh;
        
        // Apex (top)
        float apex = 0.6f;
        // Base corners
        float base = -0.4f;
        float size = 0.5f;
        
        // Base vertices (yellow)
        mesh.vertices.push_back(MakeVertex(-size, base, -size,  1.0f, 1.0f, 0.3f,  0.0f, 0.0f)); // 0
        mesh.vertices.push_back(MakeVertex( size, base, -size,  1.0f, 1.0f, 0.3f,  1.0f, 0.0f)); // 1
        mesh.vertices.push_back(MakeVertex( size, base,  size,  1.0f, 1.0f, 0.5f,  1.0f, 1.0f)); // 2
        mesh.vertices.push_back(MakeVertex(-size, base,  size,  1.0f, 1.0f, 0.5f,  0.0f, 1.0f)); // 3
        
        // Front face vertices (red)
        mesh.vertices.push_back(MakeVertex(-size, base,  size,  1.0f, 0.3f, 0.3f,  0.0f, 0.0f)); // 4
        mesh.vertices.push_back(MakeVertex( size, base,  size,  1.0f, 0.3f, 0.3f,  1.0f, 0.0f)); // 5
        mesh.vertices.push_back(MakeVertex( 0.0f, apex,  0.0f,  1.0f, 0.5f, 0.5f,  0.5f, 1.0f)); // 6
        
        // Right face vertices (green)
        mesh.vertices.push_back(MakeVertex( size, base,  size,  0.3f, 1.0f, 0.3f,  0.0f, 0.0f)); // 7
        mesh.vertices.push_back(MakeVertex( size, base, -size,  0.3f, 1.0f, 0.3f,  1.0f, 0.0f)); // 8
        mesh.vertices.push_back(MakeVertex( 0.0f, apex,  0.0f,  0.5f, 1.0f, 0.5f,  0.5f, 1.0f)); // 9
        
        // Back face vertices (blue)
        mesh.vertices.push_back(MakeVertex( size, base, -size,  0.3f, 0.3f, 1.0f,  0.0f, 0.0f)); // 10
        mesh.vertices.push_back(MakeVertex(-size, base, -size,  0.3f, 0.3f, 1.0f,  1.0f, 0.0f)); // 11
        mesh.vertices.push_back(MakeVertex( 0.0f, apex,  0.0f,  0.5f, 0.5f, 1.0f,  0.5f, 1.0f)); // 12
        
        // Left face vertices (magenta)
        mesh.vertices.push_back(MakeVertex(-size, base, -size,  1.0f, 0.3f, 1.0f,  0.0f, 0.0f)); // 13
        mesh.vertices.push_back(MakeVertex(-size, base,  size,  1.0f, 0.3f, 1.0f,  1.0f, 0.0f)); // 14
        mesh.vertices.push_back(MakeVertex( 0.0f, apex,  0.0f,  1.0f, 0.5f, 1.0f,  0.5f, 1.0f)); // 15
        
        // Base (2 triangles)
        mesh.indices.push_back(0); mesh.indices.push_back(2); mesh.indices.push_back(1);
        mesh.indices.push_back(0); mesh.indices.push_back(3); mesh.indices.push_back(2);
        
        // Side faces
        mesh.indices.push_back(4);  mesh.indices.push_back(5);  mesh.indices.push_back(6);  // Front
        mesh.indices.push_back(7);  mesh.indices.push_back(8);  mesh.indices.push_back(9);  // Right
        mesh.indices.push_back(10); mesh.indices.push_back(11); mesh.indices.push_back(12); // Back
        mesh.indices.push_back(13); mesh.indices.push_back(14); mesh.indices.push_back(15); // Left
        
        return mesh;
    }

    inline Mesh CreateCylinder(int segments = 24)
    {
        Mesh mesh;
        float height = 0.8f;
        float radius = 0.4f;
        
        // Build vertices for top cap, bottom cap, and side separately
        uint32_t vertexIndex = 0;
        
        // Top center vertex
        uint32_t topCenter = vertexIndex++;
        mesh.vertices.push_back(MakeVertex(0.0f, height/2, 0.0f, 0.3f, 0.5f, 1.0f, 0.5f, 0.5f));
        
        // Top ring vertices (for cap)
        uint32_t topRingStart = vertexIndex;
        for (int i = 0; i <= segments; i++)
        {
            float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
            float x = radius * std::cos(theta);
            float z = radius * std::sin(theta);
            mesh.vertices.push_back(MakeVertex(x, height/2, z, 0.3f, 0.5f, 1.0f, 0.5f + x, 0.5f + z));
            vertexIndex++;
        }
        
        // Bottom center vertex
        uint32_t bottomCenter = vertexIndex++;
        mesh.vertices.push_back(MakeVertex(0.0f, -height/2, 0.0f, 1.0f, 1.0f, 0.3f, 0.5f, 0.5f));
        
        // Bottom ring vertices (for cap)
        uint32_t bottomRingStart = vertexIndex;
        for (int i = 0; i <= segments; i++)
        {
            float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
            float x = radius * std::cos(theta);
            float z = radius * std::sin(theta);
            mesh.vertices.push_back(MakeVertex(x, -height/2, z, 1.0f, 1.0f, 0.3f, 0.5f + x, 0.5f + z));
            vertexIndex++;
        }
        
        // Side vertices (top and bottom rings for sides with different colors)
        uint32_t sideStart = vertexIndex;
        for (int i = 0; i <= segments; i++)
        {
            float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
            float x = radius * std::cos(theta);
            float z = radius * std::sin(theta);
            float u = static_cast<float>(i) / segments;
            
            float r = (std::cos(theta) + 1.0f) * 0.4f + 0.2f;
            float g = (std::sin(theta) + 1.0f) * 0.4f + 0.2f;
            
            // Top edge of side
            mesh.vertices.push_back(MakeVertex(x, height/2, z, r, g, 0.8f, u, 0.0f));
            // Bottom edge of side  
            mesh.vertices.push_back(MakeVertex(x, -height/2, z, r, g, 0.4f, u, 1.0f));
        }
        
        // Top cap triangles (CCW when viewed from above)
        for (int i = 0; i < segments; i++)
        {
            mesh.indices.push_back(topCenter);
            mesh.indices.push_back(topRingStart + i + 1);
            mesh.indices.push_back(topRingStart + i);
        }
        
        // Bottom cap triangles (CCW when viewed from below)
        for (int i = 0; i < segments; i++)
        {
            mesh.indices.push_back(bottomCenter);
            mesh.indices.push_back(bottomRingStart + i);
            mesh.indices.push_back(bottomRingStart + i + 1);
        }
        
        // Side triangles (CCW when viewed from outside)
        for (int i = 0; i < segments; i++)
        {
            uint32_t topLeft = sideStart + i * 2;
            uint32_t bottomLeft = sideStart + i * 2 + 1;
            uint32_t topRight = sideStart + (i + 1) * 2;
            uint32_t bottomRight = sideStart + (i + 1) * 2 + 1;
            
            // First triangle
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(bottomRight);
            
            // Second triangle
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomRight);
            mesh.indices.push_back(topRight);
        }
        
        return mesh;
    }

    inline Mesh CreateCone(int segments = 24)
    {
        Mesh mesh;
        float height = 0.9f;
        float radius = 0.45f;
        uint32_t vertexIndex = 0;
        
        // Apex vertex (for side faces)
        uint32_t apex = vertexIndex++;
        mesh.vertices.push_back(MakeVertex(0.0f, height/2, 0.0f, 1.0f, 0.5f, 0.3f, 0.5f, 0.0f));
        
        // Side vertices (base ring for sides)
        uint32_t sideRingStart = vertexIndex;
        for (int i = 0; i <= segments; i++)
        {
            float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
            float x = radius * std::cos(theta);
            float z = radius * std::sin(theta);
            float u = static_cast<float>(i) / segments;
            
            float r = (std::cos(theta) + 1.0f) * 0.35f + 0.3f;
            float g = (std::sin(theta) + 1.0f) * 0.35f + 0.3f;
            
            mesh.vertices.push_back(MakeVertex(x, -height/2, z, r, g, 0.8f, u, 1.0f));
            vertexIndex++;
        }
        
        // Base center vertex
        uint32_t baseCenter = vertexIndex++;
        mesh.vertices.push_back(MakeVertex(0.0f, -height/2, 0.0f, 0.3f, 1.0f, 0.5f, 0.5f, 0.5f));
        
        // Base ring vertices (for cap)
        uint32_t baseRingStart = vertexIndex;
        for (int i = 0; i <= segments; i++)
        {
            float theta = 2.0f * static_cast<float>(M_PI) * i / segments;
            float x = radius * std::cos(theta);
            float z = radius * std::sin(theta);
            
            mesh.vertices.push_back(MakeVertex(x, -height/2, z, 0.3f, 1.0f, 0.5f, 0.5f + x, 0.5f + z));
            vertexIndex++;
        }
        
        // Side triangles (CCW when viewed from outside)
        for (int i = 0; i < segments; i++)
        {
            mesh.indices.push_back(apex);
            mesh.indices.push_back(sideRingStart + i + 1);
            mesh.indices.push_back(sideRingStart + i);
        }
        
        // Base cap triangles (CCW when viewed from below)
        for (int i = 0; i < segments; i++)
        {
            mesh.indices.push_back(baseCenter);
            mesh.indices.push_back(baseRingStart + i);
            mesh.indices.push_back(baseRingStart + i + 1);
        }
        
        return mesh;
    }

    inline Mesh CreatePrimitive(PrimitiveType type)
    {
        switch (type)
        {
            case PrimitiveType::Cube:     return CreateCube();
            case PrimitiveType::Pyramid:  return CreatePyramid();
            case PrimitiveType::Cylinder: return CreateCylinder();
            case PrimitiveType::Cone:     return CreateCone();
            default: return CreateCube();
        }
    }
}

