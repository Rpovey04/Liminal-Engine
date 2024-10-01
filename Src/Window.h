#include "Shader.h"
#include "Clock.h"

/*	TO DO
	- Texture handling
	- OpenAL (last, can just be altered a bit)
	- Final Render
	- Uniforms and tables (choose what to submit maybe)
	- Write shaders
*/

class Window {
private:
	/*
		Window Varibles
	*/
		GLFWwindow* windowHandle;
		int windowWidth, windowHeight;
		const char* windowTitle;
		bool isOpen, isWindowed, isResizable, isInitialised;
		Clock renderClock;
	
	/*
		Rendering Variables (Buffers, vertex and indicies)
	*/
		IndexBuffer Ibuffer;
		VertexHandle Vhandle;
		unsigned int CullMode;
	/*
		Shader Variables
	*/
		ShaderTable Stable;
		Shader* currentShader;
		Mat4x4 ProjectionMatrix;

	/*
		INIT FUNCTIONS
	*/
		unsigned int InitGraphics();
		unsigned int InitProjectionMatrix();
		unsigned int InitWindow(int width, int height, const char* title, bool fullscreen, bool resizable);
		unsigned int InitBatch();
		unsigned int InitShaders();
		unsigned int InitClock();

		void setFullscreenWindowResolution(int* w, int* h);	// platform specific : rewrite when porting


	/*
		EVENT PROCESSING
	*/
		void checkWindowResize();
		double lastCursorX, lastCursorY;

public:
	/*
		Construct / Destruct
	*/
		Window(int width, int height, const char* title, bool fullscreen, bool resizable);
		~Window();

	/*
		Core Functions
	*/
		void Clear(Color c);
		void Render();
		void Draw(Quad obj);	// made for 2D sprites (will likely going to be unsupported when camera exists)
		void Draw(std::vector<VertexHandle::Rect> points, float layer = 0);
		void Poll();
	

	/*
		Rendering Processes
	*/
		bool ShouldCull(std::vector<VertexHandle::Rect> points);

	/*
		User Input
	*/
		bool Apressed() { return glfwGetKey(windowHandle, GLFW_KEY_A) == GLFW_PRESS; }
		bool Spressed() { return glfwGetKey(windowHandle, GLFW_KEY_S) == GLFW_PRESS; }
		bool Dpressed() { return glfwGetKey(windowHandle, GLFW_KEY_D) == GLFW_PRESS; }
		bool Wpressed() { return glfwGetKey(windowHandle, GLFW_KEY_W) == GLFW_PRESS; }
		bool Zpressed() { return glfwGetKey(windowHandle, GLFW_KEY_Z) == GLFW_PRESS; }
		bool Xpressed() { return glfwGetKey(windowHandle, GLFW_KEY_X) == GLFW_PRESS; }
		bool Leftpressed() { return glfwGetKey(windowHandle, GLFW_KEY_LEFT) == GLFW_PRESS; }
		bool Rightpressed() { return glfwGetKey(windowHandle, GLFW_KEY_RIGHT) == GLFW_PRESS; }
		std::vector<double> CursorChange() {
			double prevX = lastCursorX;
			double prevY = lastCursorY;
			glfwGetCursorPos(windowHandle, &lastCursorX, &lastCursorY);
			glfwSetCursorPos(windowHandle, windowWidth / 2, lastCursorY);
			if (prevX >= 0 && prevX <= windowWidth && prevY >= 0 && prevY <= windowHeight) {
				return { lastCursorX - prevX, lastCursorY - prevY };
			}
			else {
				return { 0, 0 };
			}
		}

	/*
		Getters
	*/
		bool getInitialised() { return isInitialised; }
		bool isRunning() { return isOpen && !glfwWindowShouldClose(windowHandle); }
		bool willRender();
	/*
		Setters
	*/
		void setRenderFramerate(int f);
		void setCullMode(unsigned int m);
};

