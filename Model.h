#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "TriMesh2/include/TriMesh.h"
#include<GLFW/glfw3.h>

class Vec3 {
public:
	float x;
	float y; 
	float z;
};

class Vec2 {
public:
	float x;
	float y;
};

class Model {
public: 
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	Model(const char* file, std::vector<float> scale, std::vector<float> translation, std::vector<float> color);

};
#endif