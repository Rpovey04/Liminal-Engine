#pragma once
#define _USE_MATH_DEFINES

// platform specific
#include <windows.h>	// windows specific
#include "winuser.h"	// windows specific

// standards
#include <iostream>
#include <tuple>
#include <string>
#include <cmath>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <thread>
#include <typeinfo>

// GLEW
#include "GL/glew.h"

// GLFW
#include "GLFW/glfw3.h"

// GITHUB code for PNG image
#include "stb_image.h"

// Extra math functions
static float RadianToDegree(float Rad) {
	return Rad * 180.0f / M_PI;
}

static float DegreeToRadian(float Deg) {
	return Deg * M_PI / 180.0f;
}