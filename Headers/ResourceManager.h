#pragma once
#include <string>
#include <map>
#include <glad/glad.h>

#include "Texture.h"
#include "Shader.h"

class ResourceManager
{
public:
	static std::map<std::string, Shader>    shaders;
	static std::map<std::string, Texture> textures;
public:
	static Shader LoadShader(const char* vs_file, const char* fs_file, const char* gs_file, std::string name);
	static Shader GetShader(std::string name);
	static Texture& LoadTexture(const char* file, bool gamma, bool flip, std::string name);
	static Texture& GetTexture(std::string name);
	static void Clear();
private:
	ResourceManager() { }
	static Shader LoadShaderFromFile(const char* vs_file, const char* fs_file, const char* gs_file);
	static Texture&& LoadTextureFromFile(const char* file, bool gamma, bool flip);
};
