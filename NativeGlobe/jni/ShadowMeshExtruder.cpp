/*
 * ShadowMeshExtruder.cpp
 *
 *  Created on: 5. jul. 2013
 *      Author: ZCule
 */

#include "ShadowMeshExtruder.h"

#include <map>
#include <list>
#include <vector>
#include "bullet/btVector3.h"

ShadowMeshExtruder::ShadowMeshExtruder() {
	// TODO Auto-generated constructor stub

}

/*
 * We have an array of vertices and an array of indices
 * Loop through indices and build Edge Map
 *
 * number of edges = nummber of faces * 3(each face has 3) /2 (each edge has 2 faces)
 */
EdgeMap ShadowMeshExtruder::extractEdges(void) {
	EdgeMap edges;
	EdgeMap::iterator edgeIter;

	int numfaces = m_mesh.numIndices / 3;

	for (int faceIdx = 0; faceIdx < numfaces; faceIdx++) { // for each face
		btVector3 v[3];
		for (int faceVertIdx = 0; faceVertIdx < 3; ++faceVertIdx) {
			FullVertex fv1 = m_mesh.vertices[3 * faceIdx + faceVertIdx];
			v[faceVertIdx] = new btVector3(fv1.position[0], fv1.position[1],
					fv1.position[2]);
		}
		btVector3 faceNormal = (btVector3) (v[1] - v[0]).cross(v[2] - v[0]);

		// Create and search edges to see which edges have a partner
		ShadowEdgePositions edgePos[3];
		ShadowEdgeData sharedEdgeDat[3];
		bool sharedEdgeFound[3];
		bool insertQuad[3];
		for (int faceEdgeIdx = 0; faceEdgeIdx < 3; ++faceEdgeIdx) {
			edgePos[faceEdgeIdx].p0 = v[faceEdgeIdx];
			edgePos[faceEdgeIdx].p1 = v[(faceEdgeIdx + 1) % 3];

			//If an edge was already inserted than it wil have its
			//indices reversed - I dont think it maters becouse of the == operator defined
			ShadowEdgePositions edgePosFind;
			edgePosFind.p0 = edgePos[faceEdgeIdx].p1;
			edgePosFind.p1 = edgePos[faceEdgeIdx].p0;
			edgeIter = edges.find(edgePosFind);
			// If a matching edge is found, mark it as found and add it to the list of up to 3 edges for this face.
			if (edgeIter != edges.end()) {
				//found it
				//so it has n0 already
				ShadowEdgeData edgeDat = edgeIter->second;
				edgeDat.n1 = faceNormal;
				edgeDat.f1 = faceIdx;
				edgeDat.f1ei = faceEdgeIdx;
			} else {
				//insert new
				ShadowEdgeData edgeDat;
				edgeDat.i0 = index[faceEdgeIdx];
				edgeDat.i1 = index[(faceEdgeIdx + 1) % 3];
				edgeDat.n0 = faceNormal;
				edgeDat.f0 = faceIdx;
				edgeDat.f0ei = faceEdgeIdx;
				edges[edgePos[faceEdgeIdx]] = edgeDat;
			}
		}
	}
	return edges;
}

bool ShadowMeshExtruder::reconstructMesh(EdgeMap edges) {
	EdgeMap::iterator edgeIter;
	std::vector<SHADOW_VERTEX> vertices = new std::vector<SHADOW_VERTEX>;
	std::vector<GLushort> indices = new std::vector<GLushort>;
	for (edgeIter = edges.begin(); edgeIter != edges.end(); ++edgeIter) {
		//step through edges:
		//for each edge E.v1idx <-> E.v2idx represent indices of edgeIface1 Edge of 1st face
		//so they are inverted for the 2nd face
		//add vertices to vertice Vector in the order:
		//(before adding vertices take a size of Vector => startIdx = vertices.size() - 1; )
		//verts[v1idx], verts[v2idx], verts[v1idx], verts[v2idx]
		//faces are then aded to faces Vector :
		// o4---o3
		// |   /|
		// |  / |	2-1-3	&	2-3-4
		// | /  |
		// o2---o1
		//original faces (f1 = faces[e.f1idx] and f2 = faces[e.f2idx]) are corrected:
		// f1.indices[edgeIface1] = startIdx + 1;
		// f1.indices[(edgeIface1 + 1)%3] = startIdx + 2;
		// f2.indices[edgeIface2] = startIdx + 4;
		// f2.indices[(edgeIface2 + 1)%3] = startIdx + 3;
		ShadowEdgeData edgeDat = edgeIter->second;
		SHADOW_VERTEX v1, v2, v3, v4;
		int startIdx = vertices.size() - 1;
		float temp[3] = m_mesh.vertices[edgeDat.i0].position;
		v1.pos = new btVector3(temp[0], temp[1], temp[2]);
		float temp[3] = m_mesh.vertices[edgeDat.i1].position;
		v2.pos = new btVector3(temp[0], temp[1], temp[2]);
		float temp[3] = m_mesh.vertices[edgeDat.i0].position;
		v3.pos = new btVector3(temp[0], temp[1], temp[2]);
		float temp[3] = m_mesh.vertices[edgeDat.i1].position;
		v4.pos = new btVector3(temp[0], temp[1], temp[2]);


		//correct original indices
		m_mesh.indices[3*edgeDat.f0+edgeDat.f0ei] =

		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
		vertices.push_back(v4);

		indices.push_back(startIdx + 2);
		indices.push_back(startIdx + 1);
		indices.push_back(startIdx + 3);

		indices.push_back(startIdx + 2);
		indices.push_back(startIdx + 3);
		indices.push_back(startIdx + 4);

	}
}

ShadowMeshExtruder::~ShadowMeshExtruder() {
	// TODO Auto-generated destructor stub
}

