#include "Definitions.h"

// Vectors for use in verticies
struct Color {	
	float r, g, b, a;	// 8bpp
	Color()
	{}
	Color(double red, double green, double blue, double alpha = 1.0f){
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}
	static Color convertToScalar(Color c) { return { c.r / 255.0f, c.g / 255.0f,c.b / 255.0f,c.a / 255.0f }; }
};

struct Position {
	float x, y, z;
};

struct HomogenousPosition {
	float x, y, z, w;
};

/*
struct ShaderPos {
	float x, y, z, k;
	ShaderPos() 
	{}
	ShaderPos(Position p) { // temp: taking zfar to be 1000
		// x = p.x; y = p.y; z = 1.0f + ((p.z-1) / 200.0f); k = 1.0f;
		x = p.x; y = p.y; z = p.z / 200.0f; k = 1.0f;
	}
};
*/

// Geometry for use in programs
struct Quad {
	Position pos;
	Color col;
	float width, height;
	int layer;
	Quad() : pos({ 0,0,0 }), col({32,32,32,32}), width(1), height(1), layer(1)
	{}
	Quad(Position p, Color c, float w, float h, float a = 0, int l = 1) {
		pos = p;
		col = c;
		width = w;
		height = h;
		layer = l;
	}
};

// Matricies and Vectors
struct Mat4x4 {
	float m[4][4];
	Mat4x4(float val = 0) {
		for (int i = 0; i < 4; i++) { for (int k = 0; k < 4; k++) { m[i][k] = val; } }
	}
};

struct Vec2 {
	float v[2];
	Vec2(float val = 0) {
		for (int i = 0; i < 2; i++) { v[i] = val; }
	}
};

struct Vec4 {
	float v[4];
	Vec4(float val = 0) {
		for (int i = 0; i < 4; i++) { v[i] = val; }
	}
};

static void displayMatrix4x4(Mat4x4 mat) {
	for (int w = 0; w < 4; w++) { PrintWE(mat.m[w][0] << "\t" << mat.m[w][1] << "\t" << mat.m[w][2] << "\t" << mat.m[w][3] << "\t"); }
}