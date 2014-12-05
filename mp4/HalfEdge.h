#ifndef HALFEDGE_H
#define HALFEDGE_H

#include "Vertex.h"
#include "Face.h"

extern struct vertexDS;
extern struct faceDS;

struct halfedgeDS {
	vertexDS* vertexBegin;
	vertexDS* vertexEnd;
	faceDS* face;
	halfedgeDS* next;
	halfedgeDS* opposite;
};
#endif