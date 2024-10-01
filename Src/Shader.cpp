#include "Shader.h"


std::string ReadFile(std::string Address) {
	std::ifstream file(Address);
	std::string currentLine, result;

	while (std::getline(file, currentLine)) {
		result += (currentLine + "\n");
	}

	file.close();
	return result;
}


/*
	Deletion
*/
Shader::~Shader() {
	Unload();
}

void Shader::Unload() {
	int stat;
	GL(glGetProgramiv(RendererID, GL_DELETE_STATUS, &stat));
	
	if (stat == GL_TRUE) {
		PrintWE("Shader with ID " << RendererID << " already flagged for deletion");
	}
	else {
		PrintWE("Deleting shader with ID  " << RendererID);
		GL(glDeleteProgram(RendererID));
	}
}

/*
	Compilation
*/
bool Shader::Init(const std::string FragFileName, const std::string VertFileName) {
	// retrieving source code
	VertSource = ReadFile(VertFileName);
	FragSource = ReadFile(FragFileName);
	RendererID = CreateShader();

	return RendererID != 0;
}

unsigned int Shader::CompileShader(const std::string source, unsigned int type)
{
	// creates the initial ID of the shader
	unsigned int ID = glCreateShader(type);

	// allows the string to be used as a char array
	const char* src = source.c_str();
	GL(glShaderSource(ID, 1, &src, nullptr));
	GL(glCompileShader(ID));

	int result;
	GL(glGetShaderiv(ID, GL_COMPILE_STATUS, &result));
	// error handling
	if (!result)
	{
		PrintWE("Error compiling shader: " << source);
		int length;
		GL(glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &length));
		char* message = new char[length];
		GL(glGetShaderInfoLog(ID, length, &length, message));
		PrintWE(message);
		delete message;

		return 0;
	}


	return ID;
}

unsigned int Shader::CreateShader() 
{
	unsigned int ShaderId;
	ShaderId = glCreateProgram();
	// compiling the vertex and fragment shaders
	unsigned int Vshader = CompileShader(VertSource, GL_VERTEX_SHADER);
	unsigned int Fshader = CompileShader(FragSource, GL_FRAGMENT_SHADER);
	// attaching the two shaders together
	GL(glAttachShader(ShaderId, Vshader));
	GL(glAttachShader(ShaderId, Fshader));
	// finalising the program
	GL(glLinkProgram(ShaderId));
	GL(glValidateProgram(ShaderId));
	// deleting the shaders after linking
	GL(glDeleteShader(Vshader));
	GL(glDeleteShader(Fshader));
	
	return ShaderId;
}

/*
	Binding
*/
void Shader::Bind() {
	int currentActive;
	GL(glGetIntegerv(GL_CURRENT_PROGRAM, &currentActive));
	if (currentActive == RendererID) {
		PrintWFTF("Shader with ID " << RendererID << " already bound");
	}
	else {
		PrintWFTF("Binding shader with ID " << RendererID);
		GL(glUseProgram(RendererID));
	}
}

void Shader::Unbind() {
	GL(glUseProgram(0));
}

/*
	Uniforms
*/
unsigned int Shader::GetUniformLoc(const std::string name) {
	// checking if the uniform has already been retrieved
	if (UniformLocs.find(name) != UniformLocs.end()) { return UniformLocs[name]; }

	// getting the location if it has not already been done
	PrintWE("Locating uniform: (" << name << ")");
	int location = glGetUniformLocation(RendererID, name.c_str());
	if (location == -1) { PrintWE("Uniform not in shader"); }
	PrintWE("Found with location: " << location);
	UniformLocs[name] = location;

	return location;
}

void Shader::SetUniformMat4f(const std::string name, Mat4x4 Matrix) {
	glUniformMatrix4fv(GetUniformLoc(name), 1, GL_FALSE, &(Matrix.m[0][0]));
}

void Shader::SetUniformVec2f(const std::string name, Vec2 Vector) {
	glUniform2f(GetUniformLoc(name), Vector.v[0], Vector.v[1]);
}
void Shader::SetUniformVec2f(const std::string name, std::vector<float> Vector) {
	glUniform2f(GetUniformLoc(name), Vector[0], Vector[1]);
}