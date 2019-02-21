#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm.hpp>
#include <ext.hpp>

#define GLM_ENABLE_EXPERIMENTAL
//#include <gtx/matrix_decompose.hpp>
#include <gtx/rotate_vector.hpp>

#include <stdio.h>
#include <iostream>

#include "Geometry.h"
#include "Vector.h"

#include "ModelManager.h"

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

struct Material {
    glm::vec3 ambient;//TODO: Not used (Uses diffuse texture instead)
    glm::vec3 diffuse;//Not used (Uses diffuse texture instead)
    glm::vec3 specular;//Not used (Uses specular texture instead)
    float shininess = 16.0f;
};

void RenderVector3(Vec3 vector, Color3f color = RED);
void RenderVector3At(Vec3 position, Vec3 vector, Color3f = RED);


//TODO: Maybe change push_backs to emplace
class BatchRenderer
{
public:
	BatchRenderer(float Zoom, float Ratio);
	
	void Render();
	
	void RenderSkybox();

	void RenderTriangle(float* vertices);
    

	void RenderTriangles(float* vertices, int numoftriangles);
 

	void RenderTriangle(std::vector<Point2> triangle, Color3f color); //TODO
	

	void RenderRegularTriangle(Point2 pos, float radius, float angle, Color3f color);
    

	void RenderRegularTetrahedron(glm::vec3 pos, float radius, Color3f color);
    
	void RenderLine(Vector3f pos1, Vector3f pos2, Color3f color);
	
	void RenderLines(float* lines, int number_of_lines);

	void RenderLineStrip(std::vector<float> linestrip);
	
	void RenderLineStrip(std::vector<Vector3f> linestrip, Color3f color);
	
	void RenderLine(std::vector<Point2> line, Color3f color);
	
	void RenderCuboid(const Cuboid& cuboid, Color3f color);
	
	void RenderLine(float x0, float y0, float x1, float y1, Color3f color);

	void RenderTriangleFan(std::vector<Point2> trianglefan, Color3f color);

	void RenderLineStrip(std::vector<Point2> linestrip, Color3f color);

	void RenderCircleHollow(Point2 pos, float radius, Color3f color, int numvertices = 21);

	void RenderCircleFilled(Point2 pos, float radius, Color3f color, int numvertices = 20);

	void RenderExtrudedShape(std::vector<glm::vec3> points, float height, Color3f color);

	void RenderMesh(std::string mesh, glm::mat4 model_mat, Material material);

	void RenderPortal(std::string mesh, glm::mat4 model_mat, glm::mat4 other_model_mat);

    //Works
	GLchar* LoadShader(const char* filename);

    //TODO: May not work
	GLuint LoadTexture(const char * filename);

    //TODO: May not work
	GLint compileShader(const char* filename, GLenum type);

	GLint compileShaderProgramDefault(const char* vSharerSrc, const char* fSharerSrc);

	void CompileShaders();

	GLint GetUniformLocation(GLchar* uniform_name);
	GLint GetAttributeLocation(GLchar* attribute_name);

	void SetInt(int location, int value);
	void SetFloat(int location, float value);
	void SetVec3(int location, glm::vec3 value);
	void SetMat4(int location, glm::mat4 value);
	void SetPointLight(int num, const PointLight& Light);
	void SetDirectionalLight(int num, const DirectionalLight& Light);

	void SetMaterial(const Material& Material);

	void SetBlinn(bool blinn);
    
	void SetLightAmountP(int numm);
	void SetLightAmountD(int numm);

	glm::mat4 m_model;
	glm::mat4 m_view;
	glm::mat4 m_projection;
	glm::mat4 m_mvp;
    glm::mat4 m_mvp_new;

    struct CameraStruct
    {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::mat4 MatRot;
        glm::vec3 Forward;
        glm::vec3 Right;
        glm::vec3 Up;

    }Camera;

    DirectionalLight m_directional_light;
    std::vector<PointLight> m_point_light;

    bool Blinn = true;

private:

	void _ClearScreen();

	void _RenderTriangle(float* triangle, int numvertices);

	void _RenderTriangles();

	void _RenderLines();

	void _RenderTriangleFans();

	void _RenderLineStrips();

	void _RenderMeshes();

	void _RenderPortals();

    //Not textured
	GLuint m_vbo;
	GLuint m_vao;
	GLuint m_shaderprogram;

    //Textured
	GLuint m_vbo_textured;
	GLuint m_vao_textured;
	GLuint m_shaderprogram_textured;
    GLuint m_shaderprogram_mvp_textured_unlit;
	//GLuint m_texture;

    //Better not textured
    GLuint m_vbo_new;
    GLuint m_vao_new;
    GLuint m_shaderprogram_mvp_textured;
    //Texture* m_texture_new;
    //Mesh mesh;


	//Skybox
	GLuint m_vbo_skybox;
	GLuint m_vao_skybox;
	GLuint m_shaderprogram_skybox;
	Texture* m_skybox_texture;


	std::vector<float> m_triangles;
	std::vector<float> m_lines;
	std::vector<std::vector<float>> m_trianglefans;
	std::vector<std::vector<float>> m_linestrips;

    std::map<std::string, Mesh*> m_loaded_meshes;
    std::vector<std::tuple<std::string, glm::mat4, Material>> m_meshes;
	std::vector<std::tuple<std::string, glm::mat4, glm::mat4>> m_portals;

	Material m_default_material;
};