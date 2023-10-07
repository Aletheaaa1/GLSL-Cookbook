#include "ResourceManager.h"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::map<std::string, Shader>  ResourceManager::shaders;
std::map<std::string, Texture> ResourceManager::textures;

Shader ResourceManager::LoadShader(const char* vs_file, const char* fs_file, const char* gs_file, std::string name)
{
	shaders[name] = LoadShaderFromFile(vs_file, fs_file, gs_file);
	return shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return shaders[name];
}

Texture& ResourceManager::LoadTexture(const char* file, bool gamma, bool flip, std::string name)
{
	textures[name] = LoadTextureFromFile(file, gamma, flip);
	return textures[name];
}

Texture& ResourceManager::GetTexture(std::string name)
{
	return textures[name];
}

void ResourceManager::Clear()
{
	for (auto& [name, shader] : shaders)
	{
		glDeleteShader(shader.GetProgram());
	}
	for (auto& [name, texture] : textures)
	{
		const unsigned int id = texture.GetTextureID();
		glDeleteTextures(1, &id);
	}
}

Shader ResourceManager::LoadShaderFromFile(const char* vs_file, const char* fs_file, const char* gs_file)
{
	return Shader{ vs_file, fs_file, gs_file };
}

Texture&& ResourceManager::LoadTextureFromFile(const char* file, bool gamma, bool flip)
{
	return Texture{ file, gamma, flip };
}