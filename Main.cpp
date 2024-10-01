#include "Src/Window.h"
#include <chrono>

struct WorldCube {
	Cube c;
	Color col;
};

// Window* w, float width, Position pos, Color col, float rotX, float rotZ, std::vector<float> CameraPos, float yaw, unsigned int numObjects
void AddCube(float width, Position pos, Color col, std::vector<WorldCube>* cubes) {
	// initialising cube
	Cube c;
	std::vector<Color> color;
	c.WorldVerticies[0] = {		// front face
		Position({pos.x, pos.y, pos.z}),		// bottom left
		Position({pos.x + width, pos.y, pos.z}),		// bottom right
		Position({pos.x + width, pos.y + width, pos.z}),		// top right
		Position({pos.x, pos.y + width, pos.z})		// top left
	};
	color.push_back({ 32, 32, 96, 255 });
	c.WorldVerticies[1] = {		// right face
		Position({pos.x + width, pos.y, pos.z}),		// bottom left
		Position({pos.x + width, pos.y, pos.z + width}),		// bottom right
		Position({pos.x + width, pos.y + width, pos.z + width}),		// top right
		Position({pos.x + width, pos.y + width, pos.z})			// top left
	};
	color.push_back({ 64, 32, 32, 255 });
	c.WorldVerticies[2] = {		// back face (issue)
		Position({pos.x + width, pos.y, pos.z + width}),
		Position({pos.x, pos.y, pos.z + width}),
		Position({pos.x, pos.y + width, pos.z + width}),
		Position({pos.x + width, pos.y + width, pos.z + width})
	};
	color.push_back({ 96, 32, 32, 255 });
	c.WorldVerticies[3] = {		// left face
		Position({pos.x, pos.y, pos.z + width}),
		Position({pos.x, pos.y, pos.z}),
		Position({pos.x, pos.y + width, pos.z}),
		Position({pos.x, pos.y + width, pos.z + width}) 
	};
	color.push_back({ 32, 64, 32, 255 });
	c.WorldVerticies[4] = {		// top face
		Position({pos.x, pos.y + width, pos.z}),
		Position({pos.x + width, pos.y + width, pos.z}),
		Position({pos.x + width, pos.y + width, pos.z + width}),
		Position({pos.x, pos.y + width, pos.z + width}),
	};
	color.push_back({ 32, 96, 32, 255 });
	c.WorldVerticies[5] = {		// bottom face
		Position({pos.x, pos.y, pos.z + width}),
		Position({pos.x + width, pos.y, pos.z + width}),
		Position({pos.x + width, pos.y, pos.z}),
		Position({pos.x, pos.y, pos.z}),
	};
	color.push_back({ 255, 255, 255, 255 });

	cubes->push_back({ c, col });
}

void DrawCubes(Window* w, float rotX, float rotZ, std::vector<float> CameraPos, float yaw, float pitch, std::vector<WorldCube>* cubes, unsigned int numObjects) {
	// rotating cube
	std::vector<Cube> Tcube;

	Matrix<float> Camera, Target, Up, Direction, DirectionTranslate;
	Matrix<float>::HomogenousCoord(Direction, 0, 0, 1);		// down the z axis
	Matrix<float>::RotMatrix(DirectionTranslate, 1, yaw, 0);
	Direction *= DirectionTranslate;
	Matrix<float>::HomogenousCoord(Camera, CameraPos[0], CameraPos[1], CameraPos[2]);		// play around with this
	Matrix<float>::AddVector(Target, Camera, Direction);
	Matrix<float>::HomogenousCoord(Up, 0, 1, 0);
	Matrix<float> ViewMatrix;
	Matrix<float>::LookMatrix(ViewMatrix, Camera, Target, Up);

	Position center;
	for (int k = 0; k < numObjects; k++) {
		Tcube.push_back(Cube());
		for (int q = 0; q < 6; q++) {
			for (int v = 0; v < 4; v++) {
				Matrix<float> Coord;
				Matrix<float>::HomogenousCoord(Coord, (*cubes)[k].c.WorldVerticies[q][v].x, (*cubes)[k].c.WorldVerticies[q][v].y, (*cubes)[k].c.WorldVerticies[q][v].z);

				Matrix<float> RotationMatrix;
				Matrix<float>::RotMatrix(RotationMatrix, rotX, 0, rotZ);
				Coord *= RotationMatrix;
				Coord *= ViewMatrix;

				// Coord[0][2] += 220.0f;
				// Tcube.Verticies[q][v] = VertexHandle::Rect(Coord, color[q]);

				Tcube[k].Verticies[q][v] = VertexHandle::Rect(Coord, (*cubes)[k].col, 0, 1.0f);
			}
			center.x = (Tcube[k].Verticies[q][2].Pos.x + Tcube[k].Verticies[q][0].Pos.x) / 2.0f;
			center.y = (Tcube[k].Verticies[q][2].Pos.y + Tcube[k].Verticies[q][0].Pos.y) / 2.0f;
			center.z = (Tcube[k].Verticies[q][2].Pos.z + Tcube[k].Verticies[q][0].Pos.z) / 2.0f;
			Tcube[k].Verticies[q][0].layer = center.x * 0 + center.y * 0 + center.z * 1.0f;
		}
	}

	// drawing cube
	for (int k = 0; k < numObjects; k++) {
		for (int i = 0; i < 6; i++) {
			w->Draw(Tcube[k].Verticies[i], Tcube[k].Verticies[i][0].layer);
		}
	}
}

/*
void MatrixTest() {
	PrintLE("--MatrixTest--");
	Matrix<float> mat1(1, 4, 1, 1);
	// Matrix<float> mat2(2, 3, 1, 2);
	// mat1 *= mat2;
 	PrintLE("\n" << mat1);

	Pause;
}
*/


int main() {
	// MatrixTest();
	Window window(800, 800, "Testing", LIM_WINDOWED, LIM_FIXED_SIZE);
	if (!window.getInitialised()) { return -1; }
	window.setCullMode(LIM_GL_CULL);

	Clock logicClock(120);
	
	std::vector<float> CameraPos = {0, 0, -2};
	std::vector<double> cursorChange = { 0 , 0 };
	float yaw = 0;
	float pitch = 0;
	float turnSpeed = 2.0f;	float moveSpeed = 0.025f;

	std::vector<WorldCube> worldObjects;
	//AddCube(0.2, { 0, 0, 0 }, { 32, 32, 64, 255 }, &worldObjects);
	//AddCube(0.2, { 1, 2, 0 }, { 32, 64, 32, 255 }, &worldObjects);

	Position newCubePos = { 0, 0, 0 };
	std::vector<Color> newCubeColours = { {64, 32, 32, 255}, {32, 64, 32, 255}, {32, 32, 64, 255}, {64, 64, 32, 255}, {32, 64, 64, 255}, {64, 32, 64, 255} };
	unsigned int newCubeCurrentColour = 0;
	bool placeToggle = 1;

	while (window.isRunning()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));		// Include at the start of main loops

		cursorChange = window.CursorChange();
		yaw -= cursorChange[0] * 0.2;
		pitch -= cursorChange[1] * 0.2;

		Matrix<float> Direction, DirectionTranslate;
		Matrix<float>::HomogenousCoord(Direction, 0, 0, 1);		// down the z axis
		Matrix<float>::RotMatrix(DirectionTranslate, 1, yaw, 0);
		Direction *= DirectionTranslate;

		if (window.Wpressed()) { CameraPos[0] += Direction[0][0] * moveSpeed; CameraPos[1] += Direction[0][1] * moveSpeed; CameraPos[2] += Direction[0][2] * moveSpeed; }
		if (window.Spressed()) { CameraPos[0] -= Direction[0][0] * moveSpeed; CameraPos[1] -= Direction[0][1] * moveSpeed; CameraPos[2] -= Direction[0][2] * moveSpeed; }

		Matrix<float>::RotMatrix(DirectionTranslate, 0, 90, 0);
		Direction *= DirectionTranslate;

		if (window.Apressed()) { CameraPos[0] += Direction[0][0] * moveSpeed; CameraPos[1] += Direction[0][1] * moveSpeed; CameraPos[2] += Direction[0][2] * moveSpeed; }
		if (window.Dpressed()) { CameraPos[0] -= Direction[0][0] * moveSpeed; CameraPos[1] -= Direction[0][1] * moveSpeed; CameraPos[2] -= Direction[0][2] * moveSpeed; }


		// ADDING PREVIEW OF CUBE
		newCubePos = { CameraPos[0] - 2 * sin(DegreeToRadian(yaw)), CameraPos[1] + 0, CameraPos[2] + 2 * cos(DegreeToRadian(yaw)) };
		std::cout << "new position: " << newCubePos.x << ", " << newCubePos.z << std::endl;
		std::cout << "yaw: " << yaw << std::endl;
		if (window.Zpressed() && placeToggle) { AddCube(0.2, newCubePos, newCubeColours[newCubeCurrentColour++ % 6], &worldObjects); placeToggle = 0; }
		if (!window.Zpressed()){ placeToggle = 1; }

		AddCube(0.2, newCubePos, { newCubeColours[newCubeCurrentColour%6].r, newCubeColours[newCubeCurrentColour % 6].g, newCubeColours[newCubeCurrentColour % 6].b, 128 }, &worldObjects);	// add preview

		if (window.willRender()) {
			window.Clear({ 128.0f, 128.0f, 128.0f, 255.0f });
			DrawCubes(&window, 0, 0, CameraPos, yaw, pitch, &worldObjects, worldObjects.size());
			window.Render();
		}
		window.Poll();

		window.CursorChange();

		worldObjects.pop_back();		// removing the preview of the cube
	}

	return 1;
}