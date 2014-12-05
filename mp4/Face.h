#ifndef FACE_H
#define FACE_H

#include "HalfEdge.h"

extern struct halfedgeDS;

struct faceDS
{
	halfedgeDS* halfedgeRef;
};


#endif