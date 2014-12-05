#ifndef READINDEXEDFACESET_H
#define READINDEXEDFACESET_H

#include<fstream>
#include<vector>

struct vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct face {
	GLint idx1;
	GLint idx2;
	GLint idx3;
	GLint idx4;
};


class readFile{
public:
	std::string filename;
	std::vector<vertex> vertices;
	std::vector<face> faces;
	char tag;
public:
	readFile(std::string objfile) {
		filename = objfile;
	}

	/* reads in the vertices and faces into a std::vector */
	void read() {
		std::fstream f(filename.c_str());
		int i = 0;
		while (!f.eof()) {
			f >> tag;
			if (tag == 'v') {
				vertex v;
				f >> v.x;
				f >> v.y;
				f >> v.z;
				std::cout << ++i << " " << tag << " " << v.x << " " << v.y << " " << v.z << std::endl;
				vertices.push_back(v);
			}
			else if (tag == 'f') {
				face fa;
				f >> fa.idx1;
				f >> fa.idx2;
				f >> fa.idx3;
				f >> fa.idx4;
				fa.idx1--;
				fa.idx2--;
				fa.idx3--;
				fa.idx4--;
				std::cout << ++i << " " << tag << " " << fa.idx1 << " " << fa.idx2 << " " << fa.idx3 << " " << fa.idx4 << std::endl;
				faces.push_back(fa);
			}
			else {
				break;
			}

		}
		f.close();
	}
};

#endif