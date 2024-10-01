#include "Structs.h"


class VertexHandle
{
private:
	unsigned int VarrayID;
	unsigned int VbufferID;
	bool VbufferBound;

	int maxQuads;
	int currentQuads;

public:
	static struct Rect {
		HomogenousPosition Pos;
		Color Col;
		bool shouldDraw;
		float layer;
		Rect() : Pos({ 0, 0, 0}), Col({ 0, 0, 0, 0 }), shouldDraw(1), layer(0)
		{}
		Rect(Matrix<float>& p, Color c, float l = 0, float adjust = 200.0f) : shouldDraw(1) {
			Pos = {p[0][0], p[0][1], p[0][2] / adjust, p[0][3]};
			Col = c;
			layer = l;
		}
		Rect(float p1, float p2, float p3, Color c, float l = 0, float adjust = 200.0f) : shouldDraw(1) {
			Pos = { p1, p2, p3 / adjust, 1 };
			Col = c;
			layer = l;
		}
	};

	Rect* VertexBucket;		// It's still an array but is being used as a kind of bucket which is picked from by the shaders
	int* VertexLayers;
	VertexHandle() : VbufferBound(0)
	{}
	~VertexHandle() {
		delete VertexBucket;
	}
	void DeleteVertexAttributes() {
		GL(glDeleteVertexArrays(1, &VarrayID));
		GL(glDeleteBuffers(1, &VbufferID));
	}

	bool Init(int n) {
		maxQuads = n;
		VertexBucket = new Rect[maxQuads*4];
		VertexLayers = new int[maxQuads*4];

		GL(glCreateVertexArrays(1, &VarrayID));
		BindArray();

		GL(glGenBuffers(1, &VbufferID));
		BindBuffer();
		GL(glBufferData(GL_ARRAY_BUFFER, maxQuads * sizeof(Rect) * 4, nullptr, GL_DYNAMIC_DRAW));

		// attributes
		GL(glEnableVertexArrayAttrib(VarrayID, 0));	// points attrib
		GL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Rect), (const void*)offsetof(Rect, Pos)));

		GL(glEnableVertexArrayAttrib(VarrayID, 1));	// color attrib
		GL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Rect), (const void*)offsetof(Rect, Col)));

		return 1;
	}

	void BindArray() {
		int VertexArrayBound;
		GL(glGetVertexAttribiv(VarrayID, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &VertexArrayBound));

		if (VertexArrayBound != 0) {
			PrintWFTF("Vertex Array with ID " << VarrayID << " already bound");
		}
		else {
			PrintWFTF("Binding Vertex Array with ID " << VarrayID);
			GL(glBindVertexArray(VarrayID));
		}
	}
	void UnbindArray() {
		PrintWFTF("Unbinding Vertex Array with ID " << VarrayID);
		GL(glBindVertexArray(0));
	}
	void BindBuffer() {
		if (!VbufferBound) {
			PrintWFTF("Binding Vertex Buffer with ID " << VbufferID);
			GL(glBindBuffer(GL_ARRAY_BUFFER, VbufferID));
			VbufferBound = 1;
		}
		else {
			PrintWFTF("Vertex buffer with ID " << VbufferID << " already bound");
		}
	}
	void UnbindBuffer() {
		PrintWFTF("Unbinding Vertex Buffer with ID " << VbufferID);
		GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		VbufferBound = 0;
	}

	void Bind() {
		BindArray();
		BindBuffer();
	};
	void Unbind() {
		UnbindArray();
		UnbindBuffer();
	};
	
	void InsertNewVertexSet(std::vector<Rect> Vset, float order) {
		if (currentQuads == 0) {
			for (int i = 0; i < 4; i++) {
				Vset[i].Col = Color::convertToScalar(Vset[i].Col);
				VertexBucket[currentQuads * 4 + i] = Vset[i];
			}
		}
		else {
			bool found = 0;
			for (int j = 0; j < currentQuads && !found; j++) {
				if (VertexBucket[j * 4].layer > order) {
					found = 1;
					for (int k = currentQuads; k >= 1 && k >= j + 1; k--) {
						VertexBucket[k * 4] = VertexBucket[(k - 1) * 4];
						VertexBucket[k * 4 + 1] = VertexBucket[(k - 1) * 4 + 1];
						VertexBucket[k * 4 + 2] = VertexBucket[(k - 1) * 4 + 2];
						VertexBucket[k * 4 + 3] = VertexBucket[(k - 1) * 4 + 3];
					}
					for (int i = 0; i < 4; i++) {
						Vset[i].Col = Color::convertToScalar(Vset[i].Col);
						VertexBucket[j * 4 + i] = Vset[i];
					}
				}
			}
			if (!found) {
				for (int i = 0; i < 4; i++) {
					Vset[i].Col = Color::convertToScalar(Vset[i].Col);
					VertexBucket[currentQuads * 4 + i] = Vset[i];
				}
			}
		}
		currentQuads += 1;
	}

	void ResetVerticies() {
		currentQuads = 0;
	}

	void submitBufferData() {
		GL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Rect) * (currentQuads * 4), VertexBucket));
	}
	void drawBufferData() {
		GL(glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(currentQuads * 6), GL_UNSIGNED_INT, nullptr));
	}
};


struct Cube {
	std::vector<std::vector<Position>> WorldVerticies;
	std::vector<std::vector<VertexHandle::Rect>> Verticies;
	Cube() {
		Verticies = std::vector<std::vector<VertexHandle::Rect>>(6);
		for (int i = 0; i < 6; i++) {
			Verticies[i] = std::vector<VertexHandle::Rect>(4);
		}
		WorldVerticies = std::vector<std::vector<Position>>(6);
		for (int i = 0; i < 6; i++) {
			WorldVerticies[i] = std::vector<Position>(4);
		}
	}
};
