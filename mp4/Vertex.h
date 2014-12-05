#ifndef VERTEX_H
#define VERTEX_H

#include "HalfEdge.h"

extern struct halfedgeDS;

struct vertexDS
{
	GLfloat x, y, z;
	halfedgeDS* halfedgeRef;
};


#endif