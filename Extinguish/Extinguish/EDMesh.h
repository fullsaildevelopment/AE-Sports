#pragma once

#include "../ShaderStructures/ShaderStructures.h"
#include "vec3.h"
#include <vector>

struct ED2Vertex
{
	vec3f pos;
	vec3f norm;
	float u;
	float v;

	bool operator==(const ED2Vertex& vert)const
	{
		return (vert.pos.x == pos.x && vert.pos.y == pos.y && vert.pos.z == pos.z &&
			vert.norm.x == norm.x && vert.norm.y == norm.y && vert.norm.z == norm.z &&
			vert.u == u && vert.v == v);
	}
};

struct ED2Edge
{
	int indices[2];
};


struct ED2Triangle
{
	unsigned int indices[3];
};

// EDMesh
//
// Contains all the data for a collision mesh.
struct ED2Mesh
{
	std::vector<ED2Triangle> m_Triangles;
	std::vector<ED2Vertex> m_Vertices;
	std::vector<vec3f> m_TriNorms;
	std::vector<ED2Edge> m_Edges;

	static void DoEDMesh(std::vector<VS_BasicInput> verts, std::vector<unsigned int> indexs, ED2Mesh* mesh)
	{
		int sizeV = (int)verts.size();
		int sizeI = (int)indexs.size();
		mesh->m_Vertices.resize(sizeV);
		mesh->m_Triangles.resize(sizeI / 3);
		mesh->m_TriNorms.resize(sizeI / 3);
		for (int i = 0; i < sizeV; ++i)
		{
			mesh->m_Vertices[i].pos = float3(verts[i].position.x, verts[i].position.y, verts[i].position.z);
			mesh->m_Vertices[i].norm = float3(verts[i].normal.x, verts[i].normal.y, verts[i].normal.z);
			mesh->m_Vertices[i].u = verts[i].uv.x;
			mesh->m_Vertices[i].v = verts[i].uv.y;
		}
		for (int i = 0; i < sizeI / 3; ++i)
		{
			mesh->m_Triangles[i].indices[0] = indexs[i * 3];
			mesh->m_Triangles[i].indices[1] = indexs[i * 3 + 1];
			mesh->m_Triangles[i].indices[2] = indexs[i * 3 + 2];
			mesh->m_TriNorms[i] = mesh->m_Vertices[indexs[i * 3]].norm;
			ED2Edge edge1;
			edge1.indices[0] = indexs[i * 3];
			edge1.indices[1] = indexs[i * 3 + 1];
			ED2Edge edge2;
			edge2.indices[0] = indexs[i * 3 + 1];
			edge2.indices[1] = indexs[i * 3 + 2];
			ED2Edge edge3;
			edge3.indices[0] = indexs[i * 3 + 2];
			edge3.indices[1] = indexs[i * 3];
			mesh->m_Edges.push_back(edge1);
			mesh->m_Edges.push_back(edge2);
			mesh->m_Edges.push_back(edge3);
		}
	}
};
