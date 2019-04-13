#pragma once
#include <GL/glew.h>
//#include <GL/freeglut.h>
#include <glm.hpp>
#include <ext.hpp>

//#define GLM_ENABLE_EXPERIMENTAL
//#include <gtx/rotate_vector.hpp>

//#include <stdio.h>
#include <iostream>
//#include <set>
#include <map>
#include <vector>

#include <FreeImage.h>
#include <fstream>

extern const float PI;

typedef unsigned int uint;

struct Color3f
{
	Color3f() :r(1), g(1), b(1) {};
	Color3f(float red, float green, float blue) :r(red), g(green), b(blue) {};
	float r, g, b;
};

extern const Color3f RED;
extern const Color3f GREEN;
extern const Color3f BLUE;
extern const Color3f WHITE;
extern const Color3f BLACK;
extern const Color3f YELLOW;

struct PointLight
{
	glm::vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
	glm::vec3 Position;
};
struct DirectionalLight
{
	glm::vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
	glm::vec3 Direction;
};

class Texture2D
{
public:
	Texture2D(GLenum TextureTarget, const std::string& FileName);

	bool Load();

	void Bind(GLenum TextureUnit);

	glm::vec2 GetSize();

private:

	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	glm::vec2 m_size;
};

class Renderer2D
{
public:
	Renderer2D(float Zoom, float Ratio);

	void Render();

	GLchar* LoadShader(const char* filename);

	GLuint LoadTexture(const char * filename, const char * texturename);;

	GLint CompileShaderProgram(const char* vSharerSrc, const char* fSharerSrc);

	void CompileShaders();

	GLint GetUniformLocation(GLchar* uniform_name);
	GLint GetAttributeLocation(GLchar* attribute_name);

	void SetInt(GLchar* uniform_name, int value);
	void SetFloat(GLchar* uniform_name, float value);
	void SetVec3(GLchar* uniform_name, glm::vec3 value);
	void SetMat4(GLchar* uniform_name, glm::mat4 value);
	void SetPointLight(int num, const PointLight& Light);
	void SetDirectionalLight(int num, const DirectionalLight& Light);


	glm::mat4 m_model;
	glm::mat4 m_projection;

private:

	void _ClearScreen(Color3f color);

	GLuint m_vbo;
	GLuint m_vao;
	GLuint m_shaderprogram;

	std::map<std::string, Texture2D*> m_loaded_textures;
	std::vector<std::tuple<std::string, glm::mat4>> m_sprites;
};