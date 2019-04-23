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

#include <algorithm>

extern const float PI;

typedef unsigned int uint;

struct Color3f
{
	Color3f() :r(1), g(1), b(1) {};
	Color3f(float red, float green, float blue) :r(red), g(green), b(blue) {};
	float r, g, b;
};
class Texture2D
{
public:
	Texture2D(GLenum TextureTarget, const std::string& FileName);

	bool Load();

	void Bind(GLenum TextureUnit);

	glm::ivec2 GetSize();

private:

	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	glm::ivec2 m_size;
};

struct Material
{
	Texture2D* texture;
	glm::vec4 color;
};

struct VertexIndexArrays
{
	std::vector<GLfloat> vertices;
	std::vector<GLint> indices;
};

struct SpriteRenderData
{
	Material material;
	VertexIndexArrays data;
};

extern const Color3f RED;
extern const Color3f GREEN;
extern const Color3f BLUE;
extern const Color3f WHITE;
extern const Color3f BLACK;
extern const Color3f YELLOW;


class Sprite
{
public:
	Sprite();
	Sprite(Texture2D * texture, glm::ivec2 sprite_offset = { 0, 0 }, glm::ivec2 sprite_size = { 0, 0 }, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
	
	void SetTexture(Texture2D* texture);
	void SetColor(glm::vec4 color); 
	void SetSpriteOffset();
	void SetSpriteSize();
	
	Texture2D* GetTexture();
	glm::vec4 GetColor();
	glm::ivec2 GetSpriteOffset();
	glm::ivec2 GetSpriteSize();

private:
	Texture2D * m_texture;
	glm::vec4 m_color;
	glm::ivec2 m_sprite_offset;
	glm::ivec2 m_sprite_size;
};

class Shader
{
public:
	Shader(const char* vertex, const char* fragment);
	Shader();

	GLchar* LoadShader(const char* filename);
	GLint CompileShaderProgram(const char* vSharerSrc, const char* fSharerSrc);

	GLint GetUniformLocation(GLchar* uniform_name);
	GLint GetAttributeLocation(GLchar* attribute_name);

	void UseShader();

	void SetInt(GLchar* uniform_name, int value);
	void SetFloat(GLchar* uniform_name, float value);
	void SetVec3(GLchar* uniform_name, glm::vec3 value);
	void SetMat4(GLchar* uniform_name, glm::mat4 value);

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_shaderprogram;
};

class Renderer2D
{
public:
	Renderer2D(float Zoom, float Ratio);

	void Render();


	void LoadTexture(const char * filename, const char * texturename);
	Texture2D* GetTexture(const char * texturename);

	void RenderSprite(Sprite* sprite, glm::vec3 position, glm::vec2 scale = { 1.0f, 1.0f });

	void RenderStencil(std::vector<GLfloat> vertices);

	glm::mat4 m_model;
	glm::mat4 m_projection;

private:

	void _ClearScreen(Color3f color);

	void _RenderSprite(Texture2D* texture, glm::vec3 position, glm::vec2 scale = { 1.0f, 1.0f }, glm::ivec2 sprite_offset = { 0, 0 }, glm::ivec2 sprite_size = { 0, 0 }, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
	void _RenderSprites();
	void _SortSprites();

	void _RenderStencil();

	Shader m_shader_default;
	Shader m_shader_stencil;


	std::vector<std::tuple<Sprite*, glm::vec3, glm::vec2>> m_sprites;
	std::map<std::string, Texture2D*> m_loaded_textures;
	std::vector<SpriteRenderData> m_sprite_data;

	VertexIndexArrays m_stencil;
};