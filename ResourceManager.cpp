/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#include "ResourceManager.h"
#include <SOIL/SOIL.h>
#include "image.h"

// Instantiate static variables
std::map<std::string, Texture*>    ResourceManager::Textures;
std::map<std::string, Shader*>       ResourceManager::Shaders;


void ResourceManager::LoadShader(const std::string vShaderFile, const std::string fShaderFile, const std::string gShaderFile, std::string name)
{
    Shaders[name] =new Shader(vShaderFile,fShaderFile);
    std::cout << "LoadShader" << std::endl;
    // return Shaders[name];
}

Shader* ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture* ResourceManager::LoadTexture(const uint8 *src, bool alpha, std::string name)
{
    Textures[name] =new Texture( 320,240,alpha,(uint8*)src);
    return Textures[name];
}

Texture* ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    // (Properly) delete all shaders
    // for (auto iter : Shaders)
        // glDeleteProgram(iter.second.GetProgramID());
    // (Properly) delete all textures
    // for (auto iter : Textures)
        // glDeleteTextures(1, &(iter.second.GetTextureID()));
}