#pragma once
#include "Matrix.h"

/*
	FOR CONSOLE SETUP
*/
#ifdef _DEBUG
// #define PrintWFTF(txt) std::cout << "Debug: " << txt << std::endl		// Window Frame-To-Frame
#define PrintWFTF(txt) nullptr 
#define PrintWE(txt) std::cout << "Debug: " << txt << std::endl			// Window Essential
// #define PrintWE(txt) nullptr 
#define PrintLFTF(txt) std::cout << "Debug: " << txt << std::endl		// Logic Frame-To-Frame
// #define PrintLFTF(txt) nullptr 
#define PrintLE(txt) std::cout << "Debug: " << txt << std::endl			// Logic Essential
// #define PrintLE(txt) nullptr 
#define StartUp std::cout << "LIMINAL ENGINE 3.0.0 DEBUG MODE" << std::endl
#define Pause std::cout << ">>"; int n = 0; std::cin >> n
#pragma comment(linker, "/SUBSYSTEM:console /ENTRY:mainCRTStartup")
#endif // Debug
#ifndef _DEBUG
#define PrintWFTF(txt) nullptr 
#define PrintWE(txt) nullptr 
#define PrintLFTF(txt) nullptr 
#define PrintLE(txt) nullptr 
#define StartUp nullptr
#define Pause nullptr
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // Release 

/*
	ERROR CHECKING / POLLING (OpenGL)
*/
#define GL(fn) \
	fn;\
	CheckError(__LINE__, __FILE__)

static void CheckError(int line, const char* file) {
	while (GLenum err = glGetError()) {
		PrintWE("|GL| Error" << err << " on line " << line << " in file " << file);
	}
}

/*
	WINDOW SETUP DEFINTIIONS
*/
#define LIM_FULLSCREEN 1
#define LIM_WINDOWED 0
#define LIM_RESIZABLE 1
#define LIM_FIXED_SIZE 0

/*
	CULLING OPTIONS
*/
#define LIM_NO_CULL 1
#define LIM_CPU_CULL 2
#define LIM_GL_CULL 3