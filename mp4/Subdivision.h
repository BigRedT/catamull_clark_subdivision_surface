#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include "ReadIndexedFaceSet.h"
#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include "Mesh.h"

class catamullClark {
private:
	std::vector<vertex> faceCenters;
	std::vector<vertex> halfEdgeCenters;
	std::vector<int> edgeIndexArray;
	meshDS* mesh;
	enum vertexType {old_vertex, face_center, edge_center};
public:
	catamullClark() {
		/* Do nothing */
	}
	catamullClark(meshDS &myMesh) {
		mesh = &myMesh;
		faceCenters = myMesh.computeFaceCenter();
		halfEdgeCenters = myMesh.computeEdgeCenter(faceCenters);
		for (int i = 0; i < halfEdgeCenters.size(); i++) {
			edgeIndexArray.push_back(-1);
		}
	}
	void setMesh(meshDS &meshToSet) {
		mesh = &meshToSet;
		faceCenters.clear();
		halfEdgeCenters.clear();
		edgeIndexArray.clear();
		faceCenters = meshToSet.computeFaceCenter();
		halfEdgeCenters = meshToSet.computeEdgeCenter(faceCenters);
		for (int i = 0; i < halfEdgeCenters.size(); i++) {
			edgeIndexArray.push_back(-1);
		}
	}
	void createIndexedFaceSet(std::vector<vertex> &newVertices, std::vector<face> &newFaces);
	int getGlobalIdx(int idx, vertexType type);
	void updateOldVertices();
	void subdivision(meshDS &newMesh);

};

void catamullClark::createIndexedFaceSet(std::vector<vertex> &newVertices, std::vector<face> &newFaces) {
	updateOldVertices();
	for (int i = 0; i < mesh->vertexArray.size(); i++) {
		vertex v;
		v.x = mesh->vertexArray.at(i).x;
		v.y = mesh->vertexArray.at(i).y;
		v.z = mesh->vertexArray.at(i).z;
		newVertices.push_back(v);
	}

	for (int i = 0; i < faceCenters.size(); i++) {
		vertex v;
		v.x = faceCenters.at(i).x;
		v.y = faceCenters.at(i).y;
		v.z = faceCenters.at(i).z;
		newVertices.push_back(v);
	}

	int count = 0;
	for (int i = 0; i < halfEdgeCenters.size(); i++) {
		if (edgeIndexArray.at(i) != -1){
			continue;
		}
		else {
			vertex v;
			v.x = halfEdgeCenters.at(i).x;
			v.y = halfEdgeCenters.at(i).y;
			v.z = halfEdgeCenters.at(i).z;
			newVertices.push_back(v);
			edgeIndexArray.at(i) = count;
			int oppositeIdx = findIndex<halfedgeDS>(mesh->halfedgeArray, *mesh->halfedgeArray.at(i).opposite);
			edgeIndexArray.at(oppositeIdx) = count;
			count++;
		}
	}
	
	for (int i = 0; i < mesh->faceArray.size(); i++) {
		faceDS* faceCurrent = &mesh->faceArray.at(i);
		
		halfedgeDS* h1 = faceCurrent->halfedgeRef;
		halfedgeDS* h2 = h1->next;
		halfedgeDS* h3 = h2->next;
		halfedgeDS* h4 = h3->next;
		
		// Get Edge Center Indices
		int edgeCenterIdx1 = findIndex<halfedgeDS>(mesh->halfedgeArray, *h1);
		edgeCenterIdx1 = getGlobalIdx(edgeIndexArray.at(edgeCenterIdx1), edge_center);
		int edgeCenterIdx2 = findIndex<halfedgeDS>(mesh->halfedgeArray, *h2);
		edgeCenterIdx2 = getGlobalIdx(edgeIndexArray.at(edgeCenterIdx2), edge_center);
		int edgeCenterIdx3 = findIndex<halfedgeDS>(mesh->halfedgeArray, *h3);
		edgeCenterIdx3 = getGlobalIdx(edgeIndexArray.at(edgeCenterIdx3), edge_center);
		int edgeCenterIdx4 = findIndex<halfedgeDS>(mesh->halfedgeArray, *h4);
		edgeCenterIdx4 = getGlobalIdx(edgeIndexArray.at(edgeCenterIdx4), edge_center);

		// Get Old Vertex Indices
		int oldVertexIdx1 = findIndex<vertexDS>(mesh->vertexArray, *h1->vertexBegin);
		oldVertexIdx1 = getGlobalIdx(oldVertexIdx1, old_vertex);
		int oldVertexIdx2 = findIndex<vertexDS>(mesh->vertexArray, *h2->vertexBegin);
		oldVertexIdx2 = getGlobalIdx(oldVertexIdx2, old_vertex);
		int oldVertexIdx3 = findIndex<vertexDS>(mesh->vertexArray, *h3->vertexBegin);
		oldVertexIdx3 = getGlobalIdx(oldVertexIdx3, old_vertex);
		int oldVertexIdx4 = findIndex<vertexDS>(mesh->vertexArray, *h4->vertexBegin);
		oldVertexIdx4 = getGlobalIdx(oldVertexIdx4, old_vertex);

		// Get face Center Indices
		int faceCenterIdx = findIndex<faceDS>(mesh->faceArray, *h1->face);
		faceCenterIdx = getGlobalIdx(faceCenterIdx, face_center);
		
		face f1; f1.idx1 = oldVertexIdx1; f1.idx2 = edgeCenterIdx1; f1.idx3 = faceCenterIdx; f1.idx4 = edgeCenterIdx4;
		face f2; f2.idx1 = edgeCenterIdx1; f2.idx2 = oldVertexIdx2; f2.idx3 = edgeCenterIdx2; f2.idx4 = faceCenterIdx;
		face f3; f3.idx1 = faceCenterIdx; f3.idx2 = edgeCenterIdx2; f3.idx3 = oldVertexIdx3; f3.idx4 = edgeCenterIdx3;
		face f4; f4.idx1 = edgeCenterIdx4; f4.idx2 = faceCenterIdx; f4.idx3 = edgeCenterIdx3; f4.idx4 = oldVertexIdx4;
		
		newFaces.push_back(f1);
		newFaces.push_back(f2);
		newFaces.push_back(f3);
		newFaces.push_back(f4);
	}
}

int catamullClark::getGlobalIdx(int idx, vertexType type) {
	int globalIdx;
	int num_old = mesh->vertexArray.size();
	int num_faceCenters = faceCenters.size();
	switch (type){
	case catamullClark::old_vertex:
		globalIdx = idx;
		break;
	case catamullClark::face_center:
		globalIdx = num_old + idx;
		break;
	case catamullClark::edge_center:
		globalIdx = num_old + num_faceCenters + idx;
		break;
	default:
		globalIdx = -1;
		break;
	}
	return globalIdx;
}

void catamullClark::updateOldVertices() {
	for (int i = 0; i < mesh->vertexArray.size(); i++) {
		vertexDS* vertexCurrent = &mesh->vertexArray.at(i);

		halfedgeDS* halfEdgeFirst = vertexCurrent->halfedgeRef;
		halfedgeDS* halfEdgeCurrent = halfEdgeFirst->opposite->next;

		vertex faceAverage;
		vertex edgeAverage;

		int faceIdx = findIndex<faceDS>(mesh->faceArray, *halfEdgeFirst->face);

		faceAverage.x = faceCenters.at(faceIdx).x;
		faceAverage.y = faceCenters.at(faceIdx).y;
		faceAverage.z = faceCenters.at(faceIdx).z;
		
		edgeAverage.x = (halfEdgeFirst->vertexBegin->x + halfEdgeFirst->vertexEnd->x) / 2;
		edgeAverage.y = (halfEdgeFirst->vertexBegin->y + halfEdgeFirst->vertexEnd->y) / 2;
		edgeAverage.z = (halfEdgeFirst->vertexBegin->z + halfEdgeFirst->vertexEnd->z) / 2;

		int valence = 1;
		while (halfEdgeCurrent != halfEdgeFirst) {
			faceIdx = findIndex<faceDS>(mesh->faceArray, *halfEdgeCurrent->face);
			
			faceAverage.x = faceAverage.x + faceCenters.at(faceIdx).x;
			faceAverage.y = faceAverage.y + faceCenters.at(faceIdx).y;
			faceAverage.z = faceAverage.z + faceCenters.at(faceIdx).z;

			edgeAverage.x = edgeAverage.x + (halfEdgeCurrent->vertexBegin->x + halfEdgeCurrent->vertexEnd->x) / 2;
			edgeAverage.y = edgeAverage.y + (halfEdgeCurrent->vertexBegin->y + halfEdgeCurrent->vertexEnd->y) / 2;
			edgeAverage.z = edgeAverage.z + (halfEdgeCurrent->vertexBegin->z + halfEdgeCurrent->vertexEnd->z) / 2;
			
			halfEdgeCurrent = halfEdgeCurrent->opposite->next;
			
			valence++;
		}

		faceAverage.x = faceAverage.x / valence;
		faceAverage.y = faceAverage.y / valence;
		faceAverage.z = faceAverage.z / valence;

		edgeAverage.x = edgeAverage.x / valence;
		edgeAverage.y = edgeAverage.y / valence;
		edgeAverage.z = edgeAverage.z / valence;

		vertexCurrent->x = (faceAverage.x + 2 * edgeAverage.x + (valence - 3)*vertexCurrent->x) / valence;
		vertexCurrent->y = (faceAverage.y + 2 * edgeAverage.y + (valence - 3)*vertexCurrent->y) / valence;
		vertexCurrent->z = (faceAverage.z + 2 * edgeAverage.z + (valence - 3)*vertexCurrent->z) / valence;
		
	}
}

void catamullClark::subdivision(meshDS &newMesh) {
	std::vector<vertex> newVertices;
	std::vector<face> newFaces;
	createIndexedFaceSet(newVertices, newFaces);
	newMesh.clearMesh();
	newMesh.createMesh(newVertices, newFaces);
}

#endif