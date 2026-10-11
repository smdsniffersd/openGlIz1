#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>
using namespace std;

struct Shape
{
	unsigned int VAO, VBO;
	int vertexCount;
	GLenum mode;
	float r, g, b;
};

Shape createShape(const float* data, int count, GLenum mode, float r, float g, float b) {
	Shape s;
	s.vertexCount = count;
	s.mode = mode;
	s.r = r; s.g = g; s.b = b;

	glGenVertexArrays(1, &s.VAO);
	glGenBuffers(1, &s.VBO);
	glBindVertexArray(s.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	return s;
}

Shape createCircle(float cx, float cy, float radius, int segments, float r, float g, float b) {
	const float PI = 3.14159265358979323846f;
	std::vector<float> verts;
	verts.reserve((segments + 2) * 3);

	verts.push_back(cx);
	verts.push_back(cy);
	verts.push_back(0.0f);

	for (int i = 0; i <= segments; i++) {
		float angle = 2.0f * PI * i / segments;
		verts.push_back(cx + radius * cosf(angle));
		verts.push_back(cy + radius * sinf(angle));
		verts.push_back(0.0f);
	}

	return createShape(verts.data(), (int)verts.size() / 3, GL_TRIANGLE_FAN, r, g, b);
}
Shape createShapeColored(const float* data, int count, GLenum mode) {
	Shape s;
	s.vertexCount = count;
	s.mode = mode;
	s.r = s.g = s.b = 1.0f;

	glGenVertexArrays(1, &s.VAO);
	glGenBuffers(1, &s.VBO);
	glBindVertexArray(s.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);

	glBufferData(GL_ARRAY_BUFFER, count * 6 * sizeof(float), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	return s;
}
Shape createCircleGradient(float cx, float cy, float radius, int segments, float rCenter, float gCenter, float bCenter, float rEdge, float gEdge, float bEdge){
	const float PI = 3.14159265358979323846f;
	vector<float> verts;

	// центр круга — ЦВЕТ ЦЕНТРА
	verts.push_back(cx);
	verts.push_back(cy);
	verts.push_back(0.0f);
	verts.push_back(rCenter);
	verts.push_back(gCenter);
	verts.push_back(bCenter);

	// точки на окружности — ЦВЕТ КРАЯ
	for (int i = 0; i <= segments; ++i) {
		float angle = 2.0f * PI * i / segments;

		verts.push_back(cx + radius * cosf(angle));   // x
		verts.push_back(cy + radius * sinf(angle));   // y
		verts.push_back(0.0f);                        // z

		verts.push_back(rEdge);   // r
		verts.push_back(gEdge);   // g
		verts.push_back(bEdge);   // b
	}

	return createShapeColored(verts.data(),
		(int)verts.size() / 6,   // ← делим на 6!
		GL_TRIANGLE_FAN);
}