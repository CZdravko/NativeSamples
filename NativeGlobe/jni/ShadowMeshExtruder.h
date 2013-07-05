/*
 * ShadowMeshExtruder.h
 *
 *  Created on: 5. jul. 2013
 *      Author: ZCule
 */

#ifndef SHADOWMESHEXTRUDER_H_
#define SHADOWMESHEXTRUDER_H_

#include <map>
#include <list>

//
typedef struct SHADOW_VERTEX {
	btVector3 pos;
	btVector3 faceNorm1, faceNorm2;
	int LightIndex;
} SHADOW_VERTEX;


//
typedef struct ORIGINAL_FACE {
	btVector3 verts[3];
	btVector3 faceNorm;
} ORIGINAL_FACE;

typedef struct MESH{
	FullVertex* vertices;
	GLushort* indices;
	int numVerts;
	int numIndices;
};

typedef std::pair<ShadowEdgePositions, ShadowEdgeData> ShadowEdge;
typedef std::map<ShadowEdgePositions, ShadowEdgeData> EdgeMap;


class ShadowEdgePositions {
public:
	btVector3 p0, p1;

	bool operator ==(const ShadowEdgePositions& rhs) const {
		return (rhs.p0 == p1 && rhs.p1 == p0);
	}
	bool operator <(const ShadowEdgePositions& rhs) const {
		if (p0 < rhs.p0)
			return true;
		if (p0 > rhs.p0)
			return false;
		if (p1 < rhs.p1)
			return true;
		return false;
	}
};

class ShadowEdgeData {
public:
	btVector3 n0, n1;
	int i0, i1;
	int f0, f1;
	int f0ei, f1ei;
	int buffer;
};


class ShadowMeshExtruder {
protected:
	GLushort** m_ppShadowIndexBuffer;		//IBO
	int* m_pdwShadowIndexBufferSize;		//IBO size

	FullVertex** m_ppShadowVertexBuffer;	//VBO
	int* m_pdwShadowVertexBufferSize;		//VBO size

	MESH m_mesh;
	MESH riped_mesh;

	EdgeMap extractEdges(void);
	bool reconstructMesh(EdgeMap edges);


	inline bool initMesh(FullVertex** verticesArrayPointer, int numVerts, GLushort** indicesArrayPointer, int numIndices){
		m_mesh = new MESH();
		m_mesh.vertices = *(verticesArrayPointer);
		m_mesh.indices = *(indicesArrayPointer);
		m_mesh.numVerts = numVerts;
		m_mesh.numIndices = numIndices;
		return true;
	}

public:
	ShadowMeshExtruder();
	virtual ~ShadowMeshExtruder();
};

#endif /* SHADOWMESHEXTRUDER_H_ */
