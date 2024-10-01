#include "Window.h"

/*
	INIT FUNCTIONS
*/
unsigned int Window::InitGraphics() {
	bool goodInit = glfwInit();
	if (!goodInit) { 
		PrintWE("glfw failed when initiating");
		return 0; 
	}
	else {
		PrintWE("glfw initialised successfully");
		glewExperimental = true;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DECORATED, false);
		return 1;
	}

	CullMode = LIM_NO_CULL;
}

unsigned int Window::InitProjectionMatrix() {
	// All of this will be variable / messed with 
	float AspectRatio = (float)windowWidth / (float)windowHeight;
	float FOV = 90.0f;

	float Near = 0.1f;
	float Far = 1000.0f;
	float Top = tanf(DegreeToRadian(FOV) / 2.0f) * Near;
	float Bottom = -Top;
	float Left = -Top * AspectRatio;
	float Right = Top * AspectRatio;

	// glFrustum(Left, Right, Bottom, Top, Near, Far);

	ProjectionMatrix.m[0][0] = (2 * Near) / (Right - Left); // * (1.0f / (windowWidth / 2.0f));
	ProjectionMatrix.m[2][0] = (Right + Left) / (Right - Left) ;
	
	ProjectionMatrix.m[1][1] = (2 * Near) / (Top - Bottom); //  * (1.0f / (windowHeight / 2.0f));
	ProjectionMatrix.m[2][1] = (Top + Bottom) / (Top - Bottom);

	ProjectionMatrix.m[2][2] = -(Far + Near) / (Far - Near);
	ProjectionMatrix.m[3][2] = (2 * Far * Near) / (Far - Near);

	ProjectionMatrix.m[2][3] = 1;
	PrintWE("Projection matrix initialised:");
	displayMatrix4x4(ProjectionMatrix);

	return 1;
}

unsigned int Window::InitWindow(int width, int height, const char* title, bool fullscreen, bool resizable) {
	windowWidth = width;
	windowHeight = height;
	windowTitle = title;
	isWindowed = !fullscreen;
	isResizable = resizable;

	if (isWindowed) { 
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, resizable && isWindowed);
		windowHandle = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	}
	else { 
		setFullscreenWindowResolution(&windowWidth, &windowHeight);
		GLFWmonitor* Monitor = glfwGetPrimaryMonitor();
		windowHandle = glfwCreateWindow(windowWidth, windowHeight, windowTitle, glfwGetPrimaryMonitor(), NULL); // may test other sources with new setup
		checkWindowResize();
	}

	if (windowHandle == NULL) {
		PrintWE("Window creation unsuccessful");
		return 0;
	}
	else { PrintWE("Window creation successful"); }
	PrintWE("WindowWidth=" << windowWidth << " WindowHeight=" << windowHeight);

	glfwMakeContextCurrent(windowHandle);

	if (glewInit() != GLEW_OK) {
		PrintWE("Error with GLEW init");
		return 0;
	}
	else { PrintWE("GLEW init successful"); }

	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	return 1;
}

unsigned int Window::InitBatch() {
	Ibuffer.Init(10000);	// may be variable for 3D/2D
	Vhandle.Init(Ibuffer.getNumQuads());

	return 1;
}

unsigned int Window::InitShaders() {
	bool successfulLoad = 1;

	// loading all shaders
	successfulLoad &= Stable.loadNewShader("MainShader");

	Stable.swapCurrentActiveShader("MainShader", &currentShader);

	return successfulLoad;
}

unsigned int Window::InitClock() {
	setRenderFramerate(60);		// default is 60
	return 1;
}

void Window::setFullscreenWindowResolution(int* W, int* H) {
	// windows specific (winuser.h + windows.h)
	*W = GetSystemMetrics(SM_CXSCREEN);
	*H = GetSystemMetrics(SM_CYSCREEN);
}

/*
	Construct / Destruct
*/
Window::Window(int width, int height, const char* title, bool fullscreen, bool resizable)
{
	isInitialised = 1;	// check declaration
	
	// initialisation
	isInitialised &= InitGraphics();
	isInitialised &= InitWindow(width, height, title, fullscreen, resizable);
	isInitialised &= InitProjectionMatrix();
	isInitialised &= InitBatch();
	isInitialised &= InitShaders();
	isInitialised &= InitClock();

	// report
	if (isInitialised) { 
		isOpen = 1;
		PrintWE("Initialsed window successfully");
	}
	else { 
		isOpen = 0;
		PrintWE("Bad window initialisation"); 
	}
}

Window::~Window() {
	Stable.DeleteAllShaders();
	Ibuffer.DeleteBuffer();
	Vhandle.DeleteVertexAttributes();
	glfwDestroyWindow(windowHandle);
}

/*
	CORE FUNCTIONS - Rendering
*/

void Window::Clear(Color c) {
	glfwSwapBuffers(windowHandle);
	glClearColor(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	Vhandle.ResetVerticies();
}

void Window::Draw(Quad obj) {
	Draw({
		 VertexHandle::Rect(obj.pos.x, obj.pos.y, obj.pos.z, obj.col),										// bottom left
		 VertexHandle::Rect(obj.pos.x + obj.width, obj.pos.y, obj.pos.z, obj.col),							// bottom right
		 VertexHandle::Rect(obj.pos.x + obj.width, obj.pos.y + obj.height, obj.pos.z, obj.col),				// top right
		 VertexHandle::Rect(obj.pos.x, obj.pos.y + obj.height, obj.pos.z, obj.col),							// top left
		}, obj.layer);
}

void Window::Draw(std::vector<VertexHandle::Rect> points, float layer) {		// all points will always lie on the same plane so can be culled
	if (CullMode == LIM_NO_CULL || CullMode == LIM_GL_CULL) { Vhandle.InsertNewVertexSet(points, layer); }
	else {
		if (!ShouldCull(points)) {
			Vhandle.InsertNewVertexSet(points, layer);
		}
	}
}

void Window::Render() {
	currentShader->Bind();
	currentShader->SetUniformMat4f("projMatrix", ProjectionMatrix);
	currentShader->SetUniformVec2f("resolution", { (float)windowWidth, (float)windowHeight });

	Ibuffer.Bind();

	if (CullMode == LIM_GL_CULL) {
		GL(glEnable(GL_CULL_FACE));
		GL(glFrontFace(GL_CW));
		GL(glCullFace(GL_BACK));
	}
	else { GL(glDisable(GL_CULL_FACE)); }

	Vhandle.Bind();
	Vhandle.submitBufferData();



	Vhandle.drawBufferData();
}

/*
	CORE FUNCTIONS - Events
*/

void Window::Poll() {
	glfwPollEvents();
	if (isResizable) { checkWindowResize(); }
}

void Window::checkWindowResize() {
	int initWidth = windowWidth;
	int initHeight = windowHeight;
	glfwGetWindowSize(windowHandle, &windowWidth, &windowHeight);
	
	if (initWidth != windowWidth || initHeight != windowHeight) {
		PrintWE("Window dimensions changes: W=" << windowWidth << " H=" << windowHeight);
	}
}

/*
	RENDERING FUNCTIONS - Culling
*/

bool Window::ShouldCull(std::vector<VertexHandle::Rect> points) {
	std::vector<float> Dir1, Dir2, Normal;
	float length, crossProduct;

	Dir1 = { points[2].Pos.x - points[3].Pos.x, points[2].Pos.y - points[3].Pos.y, points[2].Pos.z - points[3].Pos.z };
	Dir2 = { points[0].Pos.x - points[3].Pos.x, points[0].Pos.y - points[3].Pos.y, points[0].Pos.z - points[3].Pos.z };
	Normal = { Dir1[1] * Dir2[2] - Dir1[2] * Dir2[1], Dir1[2] * Dir2[0] - Dir1[0] * Dir2[2], Dir1[0] * Dir2[1] - Dir1[1] * Dir2[0] };
	length = sqrtf(Normal[0] * Normal[0] + Normal[1] * Normal[1] + Normal[2] * Normal[2]);
	Normal[0] /= length; Normal[1] /= length; Normal[2] /= length;

	crossProduct = Normal[0] * points[3].Pos.x + Normal[1] * points[3].Pos.y + Normal[2] * points[3].Pos.z;

	return crossProduct >= 0.0f;
}

/*
	GETTERS 
*/
bool Window::willRender() {
	return renderClock.timePassedSinceLastWait();
}

/*
	SETTERS / EXTERNAL ACCESS
*/
void Window::setRenderFramerate(int f) {
	renderClock.setFramerate(f);
	glfwWindowHint(GLFW_REFRESH_RATE, renderClock.getFramerate());
}
void Window::setCullMode(unsigned int m) {
	CullMode = m;
}

