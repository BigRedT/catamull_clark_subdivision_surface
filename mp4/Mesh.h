#ifndef MESH_H
#define MESH_H

#include "Face.h"
#include "Vertex.h"
#include "HalfEdge.h"
#include "ReadIndexedFaceSet.h"

template <class elementType>
int findIndex(std::vector<elementType> &Array, elementType &Element) {
	for (int i = 0; i < Array.size(); i++) {
		if (&Array.at(i) == &Element) {
			return i;
		}
	}
	return -1;
}

class meshDS {
public:
	std::vector<faceDS> faceArray;
	std::vector<vertexDS> vertexArray;
	std::vector<halfedgeDS> halfedgeArray;
public:
	meshDS() {
		/* Do nothing */
	}
	void clearMesh(){
		faceArray.clear();
		vertexArray.clear();
		halfedgeArray.clear();
	}
	void createMesh(const std::vector<vertex> &vertices, const std::vector<face> &faces);
	std::vector<vertex> computeFaceCenter();
	std::vector<vertex> computeEdgeCenter(std::vector<vertex>& faceCenters);
};

void meshDS::createMesh(const std::vector<vertex> &vertices, const std::vector<face> &faces){
	// create a container array of faceDS
	for (int i = 0; i < faces.size(); i++) {
		faceDS newface;
		newface.halfedgeRef = NULL;
		faceArray.push_back(newface);
	}

	// create a container array of vertexDS
	for (int i = 0; i < vertices.size(); i++) {
		vertexDS newVertex;
		vertex v = vertices.at(i);
		newVertex.x = v.x;
		newVertex.y = v.y;
		newVertex.z = v.z;
		newVertex.halfedgeRef = NULL;
		vertexArray.push_back(newVertex);
	}
	
	// create a container array of halfedgeDS
	for (int i = 0; i < faces.size(); i++) {
		halfedgeDS* h1 = new halfedgeDS;
		h1->face = &faceArray.at(i);
		h1->vertexBegin = &vertexArray.at(faces.at(i).idx1);
		h1->vertexEnd = &vertexArray.at(faces.at(i).idx2);
		h1->next = NULL;
		h1->opposite = NULL;
		
		halfedgeDS* h2 = new halfedgeDS;
		h2->face = &faceArray.at(i);
		h2->vertexBegin = &vertexArray.at(faces.at(i).idx2);
		h2->vertexEnd = &vertexArray.at(faces.at(i).idx3);
		h2->next = NULL;
		h2->opposite = NULL;

		halfedgeDS* h3 = new halfedgeDS;
		h3->face = &faceArray.at(i);
		h3->vertexBegin = &vertexArray.at(faces.at(i).idx3);
		h3->vertexEnd = &vertexArray.at(faces.at(i).idx4);
		h3->next = NULL;
		h3->opposite = NULL;

		halfedgeDS* h4 = new halfedgeDS;
		h4->face = &faceArray.at(i);
		h4->vertexBegin = &vertexArray.at(faces.at(i).idx4);
		h4->vertexEnd = &vertexArray.at(faces.at(i).idx1);
		h4->next = NULL;
		h4->opposite = NULL;

		halfedgeArray.push_back(*h1);
		halfedgeArray.push_back(*h2);
		halfedgeArray.push_back(*h3);
		halfedgeArray.push_back(*h4);
	
	}
	for (int i = 0; i < faceArray.size(); i++) {
		//assign next
		halfedgeArray.at(4 * i).next = &halfedgeArray.at(4 * i + 1);
		halfedgeArray.at(4 * i + 1).next = &halfedgeArray.at(4 * i + 2);
		halfedgeArray.at(4 * i + 2).next = &halfedgeArray.at(4 * i + 3);
		halfedgeArray.at(4 * i + 3).next = &halfedgeArray.at(4 * i);

		//assign one of these half edges to the face
		faceArray.at(i).halfedgeRef = &halfedgeArray.at(4 * i);
	}

	// fill in the opposite reference in halfedgeArray
	for (int i = 0; i < halfedgeArray.size(); i++) {
		if (halfedgeArray.at(i).opposite != NULL) {
			continue;
		}
		vertexDS* vBegin = halfedgeArray.at(i).vertexBegin;
		vertexDS* vEnd = halfedgeArray.at(i).vertexEnd;
		for (int j = 0; j < halfedgeArray.size(); j++) {
			if (vBegin == halfedgeArray.at(j).vertexEnd && vEnd == halfedgeArray.at(j).vertexBegin) {
				halfedgeArray.at(i).opposite = &halfedgeArray.at(j);
				halfedgeArray.at(j).opposite = &halfedgeArray.at(i);
			}
		}
	}

	//create halfedge reference in vertexArray
	for (int i = 0; i < faceArray.size(); i++) {
		halfedgeDS* firstFaceHalfEdge = faceArray.at(i).halfedgeRef;
		halfedgeDS* faceEdgeIterator = firstFaceHalfEdge;
		while (firstFaceHalfEdge != faceEdgeIterator->next) {
			faceEdgeIterator->vertexBegin->halfedgeRef = faceEdgeIterator;
			faceEdgeIterator = faceEdgeIterator->next;
		}
		faceEdgeIterator->vertexBegin->halfedgeRef = faceEdgeIterator;
	}
}

std::vector<vertex> meshDS::computeFaceCenter() {
	std::vector<vertex> faceCenters;
	for (int i = 0; i < faceArray.size(); i++) {
		vertex faceCenter;
		halfedgeDS* firstFaceHalfEdge = faceArray.at(i).halfedgeRef;
		halfedgeDS* faceEdgeIterator = firstFaceHalfEdge->next;
		faceCenter.x = firstFaceHalfEdge->vertexBegin->x;
		faceCenter.y = firstFaceHalfEdge->vertexBegin->y;
		faceCenter.z = firstFaceHalfEdge->vertexBegin->z;
		while (firstFaceHalfEdge != faceEdgeIterator) {
			faceCenter.x = faceCenter.x + faceEdgeIterator->vertexBegin->x;
			faceCenter.y = faceCenter.y + faceEdgeIterator->vertexBegin->y;
			faceCenter.z = faceCenter.z + faceEdgeIterator->vertexBegin->z;
			faceEdgeIterator = faceEdgeIterator->next;
		}
		faceCenter.x = faceCenter.x / 4;
		faceCenter.y = faceCenter.y / 4;
		faceCenter.z = faceCenter.z / 4;
		faceCenters.push_back(faceCenter);
	}
	return faceCenters;
}

std::vector<vertex> meshDS::computeEdgeCenter(std::vector<vertex>& faceCenters) {
	std::vector<vertex> edgeCenters;
	for (int i = 0; i < halfedgeArray.size(); i++) {
		vertex edgeCenter;
		int faceIdx1 = findIndex<faceDS>(faceArray, *halfedgeArray.at(i).face);
		int faceIdx2 = findIndex<faceDS>(faceArray, *halfedgeArray.at(i).opposite->face);
		vertex faceCenter1 = faceCenters.at(faceIdx1);
		vertex faceCenter2 = faceCenters.at(faceIdx2);
		edgeCenter.x = (halfedgeArray.at(i).vertexBegin->x + halfedgeArray.at(i).vertexEnd->x + faceCenter1.x + faceCenter2.x) / 4;
		edgeCenter.y = (halfedgeArray.at(i).vertexBegin->y + halfedgeArray.at(i).vertexEnd->y + faceCenter1.y + faceCenter2.y) / 4;
		edgeCenter.z = (halfedgeArray.at(i).vertexBegin->z + halfedgeArray.at(i).vertexEnd->z + faceCenter1.z + faceCenter2.z) / 4;
		edgeCenters.push_back(edgeCenter);
	}
	return edgeCenters;
}
#endif 