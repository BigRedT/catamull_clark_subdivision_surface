#include <iostream>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <math.h>

#include "ReadIndexedFaceSet.h"
#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include "Mesh.h"
#include "Subdivision.h"

readFile filereader("mesh_I.txt");
meshDS myMesh;
catamullClark ccSubDiv;
int dispMode;
vertex p0, p1, p2, p3;
vertex eye;
GLfloat t = 0;
void init() {
	dispMode = 0;
	glClearColor(0, 0, 0, 1);
	filereader.read();
	myMesh.createMesh(filereader.vertices, filereader.faces);
	ccSubDiv.setMesh(myMesh);
	p0.x = 0; p0.y = 0; p0.z = 3;
	p1.x = 6; p1.y = -5; p1.z = 0;
	p2.x = 0; p2.y = 0; p2.z = -5;
	p3.x = -5; p3.y = 3; p3.z = -2;
	eye = p0;
}

void bezier() {
	eye.x = ((GLfloat)std::pow(1 - t, 3))*p0.x + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p1.x + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p2.x + ((GLfloat)std::pow(t, 3))*p3.x;
	eye.y = ((GLfloat)std::pow(1 - t, 3))*p0.y + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p1.y + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p2.y + ((GLfloat)std::pow(t, 3))*p3.y;
	eye.z = ((GLfloat)std::pow(1 - t, 3))*p0.z + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p1.z + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p2.z + ((GLfloat)std::pow(t, 3))*p3.z;
}

void display() {

	if (dispMode == 1)	//To toggle between the wireframe and filled mesh models 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* applying perspective projection */

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 1, 10);

	/* applying model view transformations */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, 0, 0, 0, 0, 1, 0);
	//gluLookAt(2, 0, 2, 0, 0, 0, 0, 1, 0);

	/* rendering the I */
#if 0
	for (int i = 0; i < filereader.faces.size(); i++) {
		face f = filereader.faces.at(i);
		glColor3f(1, 0, 0);
		glBegin(GL_QUADS);
		glVertex3f(filereader.vertices.at(f.idx1).x, filereader.vertices.at(f.idx1).y, filereader.vertices.at(f.idx1).z);
		glVertex3f(filereader.vertices.at(f.idx2).x, filereader.vertices.at(f.idx2).y, filereader.vertices.at(f.idx2).z);
		glVertex3f(filereader.vertices.at(f.idx3).x, filereader.vertices.at(f.idx3).y, filereader.vertices.at(f.idx3).z);
		glVertex3f(filereader.vertices.at(f.idx4).x, filereader.vertices.at(f.idx4).y, filereader.vertices.at(f.idx4).z);
		glEnd();
	}
#endif // 0
	for (int i = 0; i < myMesh.faceArray.size(); i++) {
		faceDS face = myMesh.faceArray.at(i);
		halfedgeDS* faceHalfEdge = face.halfedgeRef;
		halfedgeDS* faceEdgeIterator = faceHalfEdge->next;
		glBegin(GL_QUADS);
		while (faceEdgeIterator != faceHalfEdge) {
			vertexDS* vertex = faceEdgeIterator->vertexBegin;
			glColor3f(1, 0, 0);
			glVertex3f(vertex->x, vertex->y, vertex->z);
			faceEdgeIterator = faceEdgeIterator->next;
		}
		vertexDS* vertex = faceEdgeIterator->vertexBegin;
		glColor3f(1, 0, 0);
		glVertex3f(vertex->x, vertex->y, vertex->z);
		glEnd();
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	//glOrtho(-1,1,-1,1,-1,1);

}
void arrowKeys(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		ccSubDiv.subdivision(myMesh);
		ccSubDiv.setMesh(myMesh);
		break;
	case GLUT_KEY_RIGHT:
		if (t > 1) { break; }
		t = t + 0.01;
		bezier();
		break;
	case GLUT_KEY_LEFT:
		if (t < 0) { break; }
		t = t - 0.01;
		bezier();
		break;
	case GLUT_KEY_F1:
		exit(0);
		break;
	}
}

int main(int argc, char **argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hey Oh!!");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(arrowKeys);

	init();
	glutMainLoop();
	
	getchar();
	
	return 0;
}