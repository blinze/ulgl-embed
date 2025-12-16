#pragma once

#include "Vertex.h"
#include <vector>
#include <cstdint>

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

namespace Primitives
{
    inline Vertex MakeVertex(float px, float py, float pz, float cr, float cg, float cb, float tu, float tv)
    {
        Vertex v;
        v.position[0] = px; v.position[1] = py; v.position[2] = pz;
        v.color[0] = cr; v.color[1] = cg; v.color[2] = cb;
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
}

