#pragma once
#include "IndexBuffer.h"

class Shader
{
private:
	unsigned int RendererID;
	
	std::unordered_map<std::string, int> UniformLocs;

	std::string FragSource;
	std::string VertSource;

	unsigned int GetUniformLoc(const std::string name);
	unsigned int CompileShader(const std::string source, unsigned int type);
	unsigned int CreateShader();

	

public:
	Shader() {}
	bool Init(const std::string FragFileName, const std::string VertFileName);
	
	~Shader();
	void Unload();

	void Bind();
	void Unbind();

	// uniforms
	void SetUniformMat4f(const std::string name, Mat4x4 Matrix);
	void SetUniformVec2f(const std::string name, Vec2 vector);
	void SetUniformVec2f(const std::string name, std::vector<float> vector);
};

class ShaderTable
{
private:
	std::unordered_map<std::string, Shader> ShaderMap;
	std::string currentBoundShader;

public:
	ShaderTable() : currentBoundShader("-")
	{}
	~ShaderTable() {
		DeleteAllShaders();
	}
	void DeleteAllShaders() {
		ShaderMap.clear();
	}

	bool loadNewShader(const std::string shaderName) {
		Shader* newShader = &(ShaderMap[shaderName]);
		return newShader->Init("Shaders/" + shaderName + ".frag", "Shaders/" + shaderName + ".vert");
	}
	void swapCurrentActiveShader(std::string newActiveName, Shader** shaderPointer) {
		if (currentBoundShader == newActiveName) {
			PrintWFTF("Shader " << newActiveName << "Already bound");
			return;
		}
		else if (currentBoundShader != "-") {
			ShaderMap[currentBoundShader].Unbind();
		}
		currentBoundShader = newActiveName;
		*shaderPointer = &(ShaderMap[currentBoundShader]);
		(*shaderPointer)->Bind();
	}
};

