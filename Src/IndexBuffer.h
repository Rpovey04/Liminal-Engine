#pragma once
#include "Vertex.h"

class IndexBuffer
{
private:
	int Quads;
	unsigned int ID;
	unsigned int* Indicies;
	
	void GenIndicies(int num) {
		auto GenQuad = [](int n, unsigned int* list)
		{
			int index = n * 6;
			int point = n * 4;

			list[index] = point + 3;	list[index + 1] = point + 2;	list[index + 2] = point;	// first triangel (clockwise windings)
			list[index + 3] = point + 2;	list[index + 4] = point + 1;	list[index + 5] = point;	// second triangel
		};

		Indicies = new unsigned int[num * 6];
		for (int k = 0; k < num; k++) {
			GenQuad(k, Indicies);
		}
		
		PrintWE("Index buffer initialised successfully!");
	}

public:
	IndexBuffer() {}
	~IndexBuffer() { delete[] Indicies; }
	void DeleteBuffer() {
		GL(glDeleteBuffers(1, &ID));
	}

	void Init(int Q)  {
		Quads = Q;
		GenIndicies(Q);	//data

		GL(glGenBuffers(1, &ID));
		GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
		GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Quads * 6 * sizeof(unsigned int), Indicies, GL_STATIC_DRAW));
	}

	void Bind() {
		GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
	}

	void Unbind() {
		GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	int getNumQuads() {
		return Quads;
	}
};

