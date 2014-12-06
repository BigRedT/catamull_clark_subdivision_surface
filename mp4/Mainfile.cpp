#define _CRT_SECURE_NO_DEPRECATE
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
#include "textfile.h"


#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char *file, int line)
{
	//
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	//
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}


void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}


GLuint v, f, p;

void setShaders() {

	char *vs = NULL, *fs = NULL, *fs2 = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("minimal.vert");
	fs = textFileRead("minimal.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs); free(fs);

	glCompileShader(v);
	glCompileShader(f);

	printShaderInfoLog(v);
	printShaderInfoLog(f);

	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);

	glLinkProgram(p);
	printProgramInfoLog(p);

	glUseProgram(p);

	GLint tex0 = glGetUniformLocation(p, "img");
	glUniform1i(tex0, 0);
}

#define PI 3.14
readFile filereader("mesh_I.txt");
meshDS myMesh;
catamullClark ccSubDiv;
int dispMode;
vertex p0, p1, p2, p3;
vertex eye;
GLfloat t = 0;


/* used to read the textures and environment maps in .rgba format */
char * readFile2(char * filename, int * size) {
	FILE * tex;
	char * texture;
	tex = fopen(filename, "r");
	fseek(tex, 0L, SEEK_END);
	*size = ftell(tex);
	texture = (char*)malloc(*size);
	fseek(tex, 0L, SEEK_SET);
	fread(texture, *size, 1, tex);
	fclose(tex);
	return texture;
}

/* initializes texture and environment mapping*/
int img_width, img_height, channels;
GLuint texture_id;
char* img;

void initTexture() {
	int size, dif_size;

	/* textue mapping */
	img = readFile2("wood.rgba", &size);
	dif_size = (int)sqrt(size / 4);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, dif_size, dif_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
}

void init() {
	dispMode = 1;
	glClearColor(0, 0, 0, 1);

	filereader.read();
	
	myMesh.createMesh(filereader.vertices, filereader.faces);
	ccSubDiv.setMesh(myMesh);
	
	p0.x = 0; p0.y = 0; p0.z = 3;
	p1.x = 6; p1.y = -5; p1.z = 0;
	p2.x = 0; p2.y = 0; p2.z = -5;
	p3.x = -5; p3.y = 3; p3.z = -2;
	eye = p0;
		
	glEnable(GL_DEPTH_TEST);

	initTexture();
}

void bezier() {
	eye.x = ((GLfloat)std::pow(1 - t, 3))*p0.x + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p1.x + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p2.x + ((GLfloat)std::pow(t, 3))*p3.x;
	eye.y = ((GLfloat)std::pow(1 - t, 3))*p0.y + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p1.y + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p2.y + ((GLfloat)std::pow(t, 3))*p3.y;
	eye.z = ((GLfloat)std::pow(1 - t, 3))*p0.z + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p1.z + 3 * ((GLfloat)std::pow(1 - t, 2))* t*p2.z + ((GLfloat)std::pow(t, 3))*p3.z;
}

/* used to calculate texture coordinates for a given vertex */
void getTexCoord(vertex v, GLfloat &s, GLfloat &t) {
	GLfloat theta = atan2(v.z, v.x);
	s = (theta + PI) / 2*PI;
	t = (v.y+1) / 2;
	//std::cout << "s: " << s << "t: " << t << std::endl;
}

void getNormal(halfedgeDS &h, vertex &n) {
	vertex v1, v2, v3;
	v1.x = h.vertexBegin->x; v1.y = h.vertexBegin->y; v1.z = h.vertexBegin->z;
	v2.x = h.vertexEnd->x; v2.y = h.vertexEnd->y; v2.z = h.vertexEnd->z;
	v3.x = h.next->vertexEnd->x; v3.y = h.next->vertexEnd->y; v3.z = h.next->vertexEnd->z;
	vertex a,b;
	a.x = v3.x - v2.x; a.y = v3.y - v2.y; a.z = v3.z - v2.z;
	b.x = v1.x - v2.x; b.y = v1.y - v2.y; b.z = v1.z - v2.z;
	n.x = a.y*b.z - a.z*b.y;
	n.y = a.z*b.x - a.x*b.z;
	n.z = a.x*b.y - a.y*b.x;
	GLfloat norm = std::sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
	n.x = n.x / (norm + 0.01);
	n.y = n.y / (norm + 0.01);
	n.z = n.z / (norm + 0.01);
	//std::cout << n.x << " " << n.y << " " << n.z << std::endl;
}

void display() {

	if (dispMode == 1)	//To toggle between the wireframe and filled mesh models 
	{
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	else{
		glPolygonMode(GL_FRONT, GL_LINE);
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

	GLfloat lpos[] = {eye.x,eye.y,eye.z,1};
	GLfloat white[] = { 1, 1, 1, 1 };

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	
	//gluLookAt(2, 0, 2, 0, 0, 0, 0, 1, 0);

	GLfloat s, t;
	for (int i = 0; i < myMesh.faceArray.size(); i++) {
		faceDS face = myMesh.faceArray.at(i);
		halfedgeDS* faceHalfEdge = face.halfedgeRef;
		halfedgeDS* faceEdgeIterator = faceHalfEdge->next;
		vertex v;
		vertex n;
		getNormal(*faceHalfEdge, n);
		glNormal3f(n.x, n.y, n.z);
		glBegin(GL_QUADS);
		while (faceEdgeIterator != faceHalfEdge) {
			vertexDS* vert = faceEdgeIterator->vertexBegin;
			v.x = vert->x; v.y = vert->y; v.z = vert->z;
			getTexCoord(v, s, t);
			glColor3f(1, 0, 0);
			glTexCoord2f(s, t);
			glVertex3f(vert->x, vert->y, vert->z);
			faceEdgeIterator = faceEdgeIterator->next;
		}
		vertexDS* vert = faceEdgeIterator->vertexBegin;
		glColor3f(1, 0, 0);
		getTexCoord(v, s, t);
		glTexCoord2f(s, t);
		glVertex3f(vert->x, vert->y, vert->z);
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
	case GLUT_KEY_DOWN:
		if (dispMode == 0) { dispMode = 1; }
		else { dispMode = 0; }
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

	glewInit();
	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}
	init();

	setShaders();
	
	glutMainLoop();
	
	getchar();
	
	return 0;
}