#pragma once
#include <include/glew/include/GL/glew.h>
#include <include/freeglut/include/GL/freeglut.h>
#include <include/glm/glm.hpp>
#include <include/glm/ext.hpp>

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
	BatchRenderer(float Zoom, float Ratio)
	{
		m_triangles = std::vector<float>();
		m_lines = std::vector<float>();
		m_trianglefans = std::vector<std::vector<float>>();
		m_linestrips = std::vector<std::vector<float>>();

		//m_texture = LoadTexture("test3.bmp");

		const char* vertex_shader_example =
			"#version 430\n"
			"layout(location = 0) in vec3 vertex_position;"
			"layout(location = 1) in vec3 vertex_colour;"
			"layout(location = 2) uniform mat4 MVP;"
			"out vec3 colour;"
			"void main() {"
			"  colour = vertex_colour;"
			"  gl_Position = MVP * vec4(vertex_position, 1.0);"
			"}";
		const char* fragment_shader_example =
			"#version 430\n"
			"in vec3 colour;"
			"out vec4 frag_colour;"
			"void main() {"
			"  frag_colour = vec4(colour, 1.0);"
			"}";

		const char* vertex_shader_textured =
			"#version 430\n"
			"layout(location = 0) in vec2 vertex_position;"
			"layout(location = 1) in vec3 vertex_colour;"
			"layout(location = 2) uniform mat4 MVP;"
			"layout(location = 3) in vec2 texture_coordinate;"
			"out vec3 colour;"
			"out vec2 tex_coord;"
			"void main() {"
			"  colour = vertex_colour;"
			"  tex_coord = texture_coordinate;"
			"  gl_Position = MVP * vec4(vertex_position, 1.0, 1.0);"
			"}";
		const char* fragment_shader_textured =
			"#version 430\n"
			"in vec3 colour;"
			"in vec2 tex_coord;"
			"uniform sampler2D texture;"
			"out vec4 frag_colour;"
			"void main() {"
			"  frag_colour = vec4(colour,1.0)*texture2D(texture, tex_coord);"
			"}";

		//TODO
        const char* vertex_shader_mvp =
            "#version 430\n"
            "layout(location = 0) in vec3 vertex_position;"
            "layout(location = 1) in vec2 texture_coordinate;"
            "layout(location = 2) in vec3 vertex_normal;"
            "layout(location = 3) uniform mat4 MVP;"
            "layout(location = 11) uniform mat4 M;"
            "layout(location = 12) uniform mat4 V;"
            "out vec3 Normal0;"
            "out vec2 tex_coord;"
            "out vec3 WorldPos0;"
            "out vec3 FragPos;"
            "void main() {"
            "  gl_Position = MVP * vec4(vertex_position, 1.0);"
            "  FragPos = vec3(M * vec4(vertex_position, 1.0));"
            "  tex_coord = texture_coordinate;"
            //"  Normal0 = (M* vec4(vertex_normal,1.0)).xyz;"
            "  Normal0 = (M * vec4(vertex_normal, 0.0)).xyz;"
            "  WorldPos0 = (M * vec4(vertex_position, 1.0)).xyz;"
            "}";

		//TODO
            const char* fragment_shader_mvp =
                "#version 430\n"
                "struct PointLight"
                "{"
                "   vec3 Color;"
                "   float AmbientIntensity;"
                "   float DiffuseIntensity;"
                "   vec3 Position;"//Currently only using Position from PointLight, everything else from DirectionalLight
                "};"
                "struct DirectionalLight"
                "{"
                "   vec3 Color;"
                "   float AmbientIntensity;"
                "   float DiffuseIntensity;"
                "   vec3 Direction;"
                "};"
                "struct Material"
                "{"
                "   vec3 ambient;"//TODO: Not used
                "   vec3 diffuse;"//Not used
                "   vec3 specular;"//Not used
                "   float shininess;"
                "};"
                "layout(location = 4) uniform vec3 gEyeWorldPos;"
                "layout(location = 9) uniform float gMatSpecularIntensity;"
                "layout(location = 10) uniform float gSpecularPower;"//Not used, comes from material(shininess)
                "layout(location = 13) uniform Material material;"
                "layout(location = 17) uniform sampler2D texture;"
                "layout(location = 18) uniform sampler2D specular_texture;"
                "layout(location = 23) uniform bool blinn;"
                "const int MAX_LIGHTS = 10;"
                "layout(location = 24) uniform int num_lights_p;"
                "layout(location = 25) uniform int num_lights_d;"
                "layout(location = 100) uniform DirectionalLight gDirectionalLight[MAX_LIGHTS];"
                "layout(location = 200) uniform PointLight gPointLight[MAX_LIGHTS];"
                "in vec3 WorldPos0;"
                "in vec3 Normal0;"
                "in vec2 tex_coord;"
                "in vec3 FragPos;"
                "out vec4 frag_colour;"
                "vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)"//TODO Attenuation
                "{"
                "   vec3 lightDir = normalize(light.Position - FragPos); "

				"   float constant = 1.0f;"//
				"   float linear = 0.0014f;"//
				"   float quadratic = 0.000007f;"//
				"   float distance = length(light.Position - FragPos);"//
				"   float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));"//

                "   float DiffuseFactor = max(dot(normal, lightDir), 0.0);"
                "   float SpecularFactor = 0.0;"
                "   if (blinn)"
                "   {"
                "     vec3 halfwayDir = normalize(lightDir + viewDir);"
                "     SpecularFactor = pow(max(dot(normal, halfwayDir), 0.0), material.shininess); "
                "   }"
                "   else"
                "   {"
                "     vec3 reflectDir = reflect(-lightDir, normal);"
                "     SpecularFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);"
                "   }"
                "   vec3 AmbientColor = (light.Color * light.AmbientIntensity) * vec3(texture2D(texture, tex_coord));"
                "   vec3 DiffuseColor = (light.Color * light.DiffuseIntensity)  * DiffuseFactor * vec3(texture2D(texture, tex_coord));"
                "   vec3  SpecularColor = (light.Color * gMatSpecularIntensity) * SpecularFactor * vec3(texture2D(specular_texture, tex_coord));"

				"   AmbientColor *= attenuation;"
				"   DiffuseColor *= attenuation;"
				"   SpecularColor *= attenuation;"

                "   return (AmbientColor + DiffuseColor + SpecularColor);"
                "}"
                "vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)"
                "{"
                "   vec3 lightDir = normalize(-light.Direction);"
                "   float DiffuseFactor = max(dot(normal, lightDir), 0.0);"
                "   float SpecularFactor = 0.0;"
                "   if(blinn)"
                "   {"
                "     vec3 halfwayDir = normalize(lightDir + viewDir);"
                "     SpecularFactor = pow(max(dot(normal, halfwayDir), 0.0), material.shininess); "
                "   }"
                "   else"
                "   {"
                "     vec3 reflectDir = reflect(-lightDir, normal);"
                "     SpecularFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);"
                "   }"
                "   vec3 AmbientColor = (light.Color * light.AmbientIntensity) * vec3(texture2D(texture, tex_coord));"
                "   vec3 DiffuseColor = (light.Color * light.DiffuseIntensity)  * DiffuseFactor * vec3(texture2D(texture, tex_coord));"
                "   vec3  SpecularColor = (light.Color * gMatSpecularIntensity) * SpecularFactor * vec3(texture2D(specular_texture, tex_coord));"
                "   return (AmbientColor + DiffuseColor + SpecularColor);"
                "}"
                "void main() {"
                //"  vec3 LightDirection = -gDirectionalLight.Direction; "
                //"  vec3 LightDirection = normalize(gPointLight.Position - FragPos); "
                "  vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);"
                //"  vec3 halfwayDir = normalize(LightDirection + VertexToEye);"
                "  vec3 Normal = normalize(Normal0);"
                "  vec3 result = vec3(0.0,0.0,0.0);"
                "  for (int i = 0; i < num_lights_p; i++)"
                "    result += CalcPointLight(gPointLight[i], Normal, VertexToEye);"
                "  for (int i = 0; i < num_lights_d; i++)"
                "    result += CalcDirectionalLight(gDirectionalLight[i], Normal, VertexToEye);"
                "  frag_colour = vec4(result, 1.0);"
                /*"  float DiffuseFactor = dot(Normal, LightDirection);"

                "  vec3 AmbientColor = (gDirectionalLight.Color * gDirectionalLight.AmbientIntensity) * vec3(texture2D(texture, tex_coord));"
                "  vec3 DiffuseColor = vec3(0, 0, 0);"
                "  vec3 SpecularColor = vec3(0, 0, 0);"
                "  if (DiffuseFactor > 0) {"
                "    DiffuseColor = (gDirectionalLight.Color * gDirectionalLight.DiffuseIntensity)  * DiffuseFactor * vec3(texture2D(texture, tex_coord));"
                "    if (blinn)"
                "    {"
                "      float SpecularFactor = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess); "
                "      SpecularColor = (gDirectionalLight.Color * gMatSpecularIntensity) * SpecularFactor * vec3(texture2D(specular_texture, tex_coord));"
                "    }"
                "    else"
                "    {"
                "      vec3 LightReflect = normalize(reflect(-LightDirection, Normal));"
                "      float SpecularFactor = dot(VertexToEye, LightReflect);"
                "      if (SpecularFactor > 0) {"
                "        SpecularFactor = pow(SpecularFactor, material.shininess);"
                "        SpecularColor = (gDirectionalLight.Color * gMatSpecularIntensity) * SpecularFactor * vec3(texture2D(specular_texture, tex_coord));"
                "      }"
                "    }"
                "  }"
                "  else {"
                "    DiffuseColor = vec3(0, 0, 0);"
                "  }"
                "  frag_colour = vec4(AmbientColor + DiffuseColor + SpecularColor, 1.0);"*/
                "}";

        //TODO
        const char* vertex_shader_mvp_unlit =
            "#version 430\n"
            "layout(location = 0) in vec3 vertex_position;"
            "layout(location = 1) in vec2 texture_coordinate;"
            "layout(location = 2) in vec3 vertex_normal;"
            "layout(location = 3) uniform mat4 MVP;"
            "layout(location = 11) uniform mat4 M;"
            "layout(location = 12) uniform mat4 V;"
            "out vec3 Normal0;"
            "out vec2 tex_coord;"
            "out vec3 WorldPos0;"
            "void main() {"
            "  tex_coord = texture_coordinate;"
            "  gl_Position = MVP * vec4(vertex_position, 1.0);"
            "}";

        //TODO
        const char* fragment_shader_mvp_unlit =
            "#version 430\n"
            "layout(location = 4) uniform vec3 gEyeWorldPos;"
            "struct DirectionalLight"
            "{"
            "   vec3 Color;"
            "   float AmbientIntensity;"
            "   float DiffuseIntensity;"
            "   vec3 Direction;"
            "};"
            "layout(location = 5) uniform DirectionalLight gDirectionalLight;"
            "layout(location = 9) uniform float gMatSpecularIntensity;"
            "layout(location = 10) uniform float gSpecularPower;"
            "in vec3 WorldPos0;"
            "in vec3 Normal0;"
            "in vec2 tex_coord;"
            "uniform sampler2D texture;"
            "out vec4 frag_colour;"
            "void main() {"
            "  frag_colour = texture2D(texture, tex_coord);"
            "}";

		//Not textured
		m_shaderprogram = compileShaderProgramDefault(vertex_shader_example,fragment_shader_example);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);//Vertex: 3 floats
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));//Color: 3 floats, offset is 3

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//Textured
		m_shaderprogram_textured = compileShaderProgramDefault(vertex_shader_textured, fragment_shader_textured);

		glGenBuffers(1, &m_vbo_textured);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_textured);

		glGenVertexArrays(1, &m_vao_textured);
		glBindVertexArray(m_vao_textured);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_textured);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), NULL);//X,Y
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(sizeof(float) * 2));//R,B,G
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(sizeof(float) * 5));//U,V

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

        //Better textured
        m_shaderprogram_mvp_textured = compileShaderProgramDefault(vertex_shader_mvp, fragment_shader_mvp);
        
        //Unlit textured
        m_shaderprogram_mvp_textured_unlit = compileShaderProgramDefault(vertex_shader_mvp_unlit, fragment_shader_mvp_unlit);

		m_projection = glm::ortho(-Zoom * Ratio, Zoom*Ratio, -Zoom, Zoom, -10.0f, 10.0f);
		m_view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
		m_model = glm::mat4(1.0f);
		m_mvp = m_projection * m_view;

        m_mvp_new = m_projection * m_view*m_model;

        //Model loading
        m_loaded_meshes.insert({ "dragon",new Mesh("Content/Models/dragon.obj") });
        m_loaded_meshes.insert({ "bunny",new Mesh("Content/Models/bunny.obj") });
        m_loaded_meshes.insert({ "airboat",new Mesh("Content/Models/airboat.obj") });

        m_loaded_meshes.insert({ "cruiser",new Mesh("Content/Models/cruiser/cruiser.obj") });
        m_loaded_meshes.insert({ "f16",new Mesh("Content/Models/f16/f16.obj") });

        m_loaded_meshes.insert({ "flatplane",new Mesh("Content/Models/flatplane/flatplane.obj") });


        //CUBE
        {
            
            const std::vector<Vertex> cube_vertices = {
                // positions          // texture coords          // normals 
                Vertex({ -0.5f, -0.5f, -0.5f},  { 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f }),
                Vertex({0.5f, -0.5f, -0.5f}, {1.0f, 0.0f }, {0.0f,  0.0f, -1.0f}) ,
                Vertex({ 0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f }, { 0.0f,  0.0f, -1.0f}),
                Vertex({ 0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f }, { 0.0f,  0.0f, -1.0f}),
                Vertex({ -0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f }, {  0.0f,  0.0f, -1.0f}),
                Vertex({ -0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f }, {  0.0f,  0.0f, -1.0f}),

                Vertex({ -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f}, {  0.0f,  0.0f, 1.0f }),
                Vertex({ 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f }, { 0.0f,  0.0f, 1.0f}),
                Vertex({ 0.5f,  0.5f,  0.5f },{ 1.0f, 1.0f } ,{ 0.0f,  0.0f, 1.0f}),
                Vertex({ 0.5f,  0.5f,  0.5f },{ 1.0f, 1.0f },{ 0.0f,  0.0f, 1.0f}),
                Vertex({ -0.5f,  0.5f,  0.5f },{ 0.0f, 1.0f }, {  0.0f,  0.0f, 1.0f}),
                Vertex({ -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f }, {  0.0f,  0.0f, 1.0f}),

                Vertex({ -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f}),
                Vertex({ -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f}),
                Vertex({ -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f}),
                Vertex({ -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f}),
                Vertex({ -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f}),
                Vertex({ -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f}),

                Vertex({ 0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f}),
                Vertex({ 0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f },{ 1.0f, 0.0f, 0.0f}),
                Vertex({ 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f}),
                Vertex({ 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f}),
                Vertex({ 0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f}),
                Vertex({ 0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f}),

                Vertex({ -0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f},{ 0.0f, -1.0f,  0.0f }),
                Vertex({ 0.5f, -0.5f, -0.5f },{ 1.0f, 1.0f }, { 0.0f, -1.0f,  0.0f}),
                Vertex({ 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f }, { 0.0f, -1.0f,  0.0f}),
                Vertex({ 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f }, { 0.0f, -1.0f,  0.0f}),
                Vertex({ -0.5f, -0.5f,  0.5f } ,{  0.0f, 0.0f},{ 0.0f, -1.0f,  0.0f }),
                Vertex({ -0.5f, -0.5f, -0.5f } ,{  0.0f, 1.0f},{ 0.0f, -1.0f,  0.0f }),

                Vertex({ -0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f},{ 0.0f,  1.0f,  0.0f}),
                Vertex({ 0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f },{ 0.0f, 1.0f, 0.0f}),
                Vertex({ 0.5f,  0.5f,  0.5f },{  1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f}),
                Vertex({ 0.5f,  0.5f,  0.5f },{  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f}),
                Vertex({ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }, {  0.0f, 1.0f, 0.0f}),
                Vertex({ -0.5f,  0.5f, -0.5f },{  0.0f, 1.0f }, {  0.0f, 1.0f, 0.0f})
            };
            const std::vector<GLuint> cube_indices
            {
                0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36
            };
            Mesh* Cube = new Mesh();
            Cube->m_Entries[0].Init(cube_vertices, cube_indices);
            Cube->m_Textures[0] = new Texture(GL_TEXTURE_2D, "Content/Images/box_d.png"); 
			bool success = Cube->m_Textures[0]->Load();
            assert(success);
            Cube->m_Textures_Specular[0] = new Texture(GL_TEXTURE_2D, "Content/Images/box_s.png");
			success = Cube->m_Textures_Specular[0]->Load();
            assert(success);

            m_loaded_meshes.insert({ "Cube",Cube });
        }

        //SKYBOX
        {
            const std::vector<Vertex> skybox_vertices = std::vector<Vertex>
            {
                Vertex({ -0.5f,-0.5f,-0.5f },{ 0.25f,0.0f },{ -0.5f,-0.5f,-0.5f }),//0
                Vertex({ 0.5f,-0.5f,-0.5f },{ 0.5f,0.0f },{ 0.5f,-0.5f,-0.5f }),//1
                Vertex({ 0.5f,-0.5f,0.5f },{ 0.5f,1.0f/3 },{ 0.5f,-0.5f,0.5f }),//2
                Vertex({ -0.5f,-0.5f,0.5f },{ 0.25f,1.0f/3 },{ -0.5f,-0.5f,0.5f }),//3

                Vertex({ -0.5f,0.5f,-0.5f },{ 0.25f,1.0f },{ -0.5f,0.5f,-0.5f }),//4
                Vertex({ 0.5f,0.5f,-0.5f },{ 0.5f,1.0f },{ 0.5f,0.5f,-0.5f }),//5
                Vertex({ 0.5f,0.5f,0.5f },{ 0.5f,2.0f/3 },{ 0.5f,0.5f,0.5f }),//6
                Vertex({ -0.5f,0.5f,0.5f },{ 0.25f,2.0f/3 },{ -0.5f,0.5f,0.5f }),//7

                Vertex({ -0.5f,-0.5f,-0.5f },{ 0.0f,1.0f/3 },{ -0.5f,-0.5f,-0.5f }),//8
                Vertex({ 0.5f,-0.5f,-0.5f },{ 0.75f,1.0f/3 },{ 0.5f,-0.5f,-0.5f }),//9

                Vertex({ -0.5f,0.5f,-0.5f },{ 0.0f,2.0f/3 },{ -0.5f,0.5f,-0.5f }),//10
                Vertex({ 0.5f,0.5f,-0.5f },{ 0.75f,2.0f/3 },{ 0.5f,0.5f,-0.5f }),//11

                Vertex({ -0.5f,-0.5f,-0.5f },{ 1.0f,1.0f/3 },{ -0.5f,-0.5f,-0.5f }),//12
                Vertex({ -0.5f,0.5f,-0.5f },{ 1.0f,2.0f/3 },{ -0.5f,0.5f,-0.5f })//13
            };
            const std::vector<GLuint> skybox_indices =
            {
                //BOT
                0,1,2,
                0,2,3,
                //TOP
                4,5,6,
                4,6,7,

                //FRONT
                12,9,11,
                12,13,11,
                //BACK
                2,3,7,
                2,6,7,

                //LEFT
                8,3,7,
                8,10,7,
                //RIGHT
                9,2,6,
                9,11,6
            };
            Mesh* Skybox = new Mesh();
            Skybox->m_Entries[0].Init(skybox_vertices, skybox_indices);
            Skybox->m_Textures[0] = new Texture(GL_TEXTURE_2D, "Content/Images/skybox.jpg"); 
			bool success = Skybox->m_Textures[0]->Load();
            assert(success);
            m_loaded_meshes.insert({ "Skybox",Skybox });
        }
        m_point_light = std::vector<PointLight>();

        m_directional_light.AmbientIntensity = 0.05f;
        m_directional_light.Color = { 1,1,1 };
        m_directional_light.DiffuseIntensity = 0.25f;
        m_directional_light.Direction = { 1,-1,0 };

        PointLight light;
        light.AmbientIntensity = 0.05f;
        light.Color = { 0,0,1 };
        light.DiffuseIntensity = 0.25f;
        light.Position = { 200,100,200 };
        m_point_light.push_back(light);

        //m_texture_new = new Texture(GL_TEXTURE_2D, "test.png");
        //m_texture_new->Load();
	}
	void Render()
	{
        m_mvp = m_projection * m_view;
        m_mvp_new = m_projection * m_view*m_model;

		_RenderTriangles();
		_RenderTriangleFans();
		_RenderLines();
        _RenderLineStrips();

        Material material;
        material.ambient = { 1.0f,1.0f,1.0f };
        material.diffuse = { 0.5f,0.5f,0.5f };
        material.specular = { 1.0f,1.0f,1.0f };
        material.shininess = 32.0f;

        //material.shininess = rand() % 100 / 100.0f;

        glUseProgram(m_shaderprogram_mvp_textured);
        //glBindVertexArray(m_vao_new);
        glUniformMatrix4fv(12, 1, GL_FALSE, &m_view[0][0]);
        SetDirectionalLight(0,m_directional_light);
        for(uint i=0;i<m_point_light.size();i++)
        {
            SetPointLight(i, m_point_light[i]);
        }
        SetLightAmountP((int)m_point_light.size());
        SetLightAmountD(1);
        SetMaterial(material);
        glUniform3fv(4, 1, &Camera.Position[0]);
        SetFloat(9, 1.0f);//Specular Intensity
        SetFloat(10, 32.0f);//Specular Power

        SetInt(18, 1);//Set specular texture to GL_TEXTURE1
        SetBlinn(Blinn);
        const std::vector<Vertex> cube_vertices = std::vector<Vertex>
        {
            Vertex({ -0.5f,-0.5f,-0.5f },{ 0.0f,0.0f },{ -0.5f,-0.5f,-0.5f }),
            Vertex({ 0.5f,-0.5f,-0.5f },{ 0.0f,0.0f },{ 0.5f,-0.5f,-0.5f }),
            Vertex({ 0.5f,-0.5f,0.5f },{ 0.0f,0.0f },{ 0.5f,-0.5f,0.5f }),
            Vertex({ -0.5f,-0.5f,0.5f },{ 0.0f,0.0f },{ -0.5f,-0.5f,0.5f }),

            Vertex({ -0.5f,0.5f,-0.5f },{ 0.0f,0.0f },{ -0.5f,0.5f,-0.5f }),
            Vertex({ 0.5f,0.5f,-0.5f },{ 0.0f,0.0f },{ 0.5f,0.5f,-0.5f }),
            Vertex({ 0.5f,0.5f,0.5f },{ 0.0f,0.0f },{ 0.5f,0.5f,0.5f }),
            Vertex({ -0.5f,0.5f,0.5f },{ 0.0f,0.0f },{ -0.5f,0.5f,0.5f })
        };
        const std::vector<GLuint> cube_indices =
        {
            0,1,2,
            0,2,3,
            4,5,6,
            4,6,7,

            0,1,5,
            0,4,5,
            2,3,7,
            2,6,7,

            0,3,7,
            0,4,7,
            1,2,6,
            1,5,6
        }; 
        GLuint err = glGetError();
        if (GLEW_OK != err)
        {
            printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
        }

        for each (auto mo in m_meshes)
        {
            m_mvp_new = m_projection * m_view *std::get<1>(mo);
            glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]); 
            GLuint err = glGetError();
            if (GLEW_OK != err)
            {
                printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
            }
            glUniformMatrix4fv(11, 1, GL_FALSE, &std::get<1>(mo)[0][0]);
            
            if (m_loaded_meshes[std::get<0>(mo)])
            {
                if (std::get<0>(mo) == "Skybox")
                {
                    glUseProgram(m_shaderprogram_mvp_textured_unlit);
                    glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
                }
                else
                {
                    glUseProgram(m_shaderprogram_mvp_textured);
                    glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
                    SetMaterial(std::get<2>(mo));
                }

                m_loaded_meshes[std::get<0>(mo)]->Render();
            }
        }
         
        err = glGetError();
        if (GLEW_OK != err)
        {
            printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
        }
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        m_meshes.clear();

		m_triangles.clear();
		m_lines.clear();
		m_trianglefans.clear();
		m_linestrips.clear();

		//Test
		int numvertices = 3;
		float vertices[3 * 7]
		{
			0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,
			20.0f,0.0f,0.0f,1.0f,0.0f,1.0f,0.0f,
			20.0f,20.0f,0.0f,0.0f,1.0f,1.0f,1.0f
		};

        //Render a test triangle
		/*glUseProgram(m_shaderprogram_textured);
		glBindVertexArray(m_vao_textured);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_textured);
		glBufferData(GL_ARRAY_BUFFER, 7 * numvertices * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glUniformMatrix4fv(2, 1, GL_FALSE, &m_mvp[0][0]);
        m_texture_new->Bind(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, numvertices);
		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	}

    void RenderTriangle(float* vertices)
    {
        m_triangles.insert(m_triangles.end(), vertices, vertices + 18);
    }

    void RenderTriangles(float* vertices, int numoftriangles)
    {
        m_triangles.insert(m_triangles.end(), vertices, vertices + numoftriangles * 18);
    }

	void RenderTriangle(std::vector<Point2> triangle, Color3f color) //TODO
	{
        return;//TODO Z
		if (triangle.size() < 3)
		{
			printf("BatchRenderer triangle not enough vertices\n");
			return;
		}		
		m_triangles.push_back(triangle[0].x);
        m_triangles.push_back(0);
		m_triangles.push_back(triangle[0].y);
		m_triangles.push_back(color.r);
		m_triangles.push_back(color.g);
		m_triangles.push_back(color.b);
		m_triangles.push_back(triangle[1].x);
        m_triangles.push_back(0);
		m_triangles.push_back(triangle[1].y);
		m_triangles.push_back(color.r);
		m_triangles.push_back(color.g);
		m_triangles.push_back(color.b);
		m_triangles.push_back(triangle[2].x);
        m_triangles.push_back(0);
		m_triangles.push_back(triangle[2].y);
		m_triangles.push_back(color.r);
		m_triangles.push_back(color.g);
		m_triangles.push_back(color.b);
	}

    void RenderRegularTriangle(Point2 pos, float radius, float angle, Color3f color)
    {
        //(X,Y,Z+R,G,B) * 3
        float vertices[3 * (3 + 3)] =
        {
            pos.x + radius * sinf(-angle)			  ,0,pos.y + radius * cosf(-angle),
            color.r,color.g,color.b,
            pos.x + radius * sinf(-angle + 2 * PI / 3),0,pos.y + radius * cosf(-angle + 2 * PI / 3),
            color.r,color.g,color.b,
            pos.x + radius * sinf(-angle + 4 * PI / 3),0,pos.y + radius * cosf(-angle + 4 * PI / 3),
            color.r,color.g,color.b
        };
        RenderTriangle(vertices);
    }

    void RenderRegularTetrahedron(glm::vec3 pos, float radius, Color3f color)
    {
        // 3(vertices) * 4(sides) * ( 3(X,Y,Z) + 3(R,G,B) )
        float vertices[3 * 4 * (3 + 3)] =
        {
            pos.x + radius * sinf(0)			 ,pos.y,pos.z + radius * cosf(0),
            color.r,color.g,color.b,
            pos.x + radius * sinf(0 + 2 * PI / 3),pos.y,pos.z + radius * cosf(0 + 2 * PI / 3),
            color.r,color.g,color.b,
            pos.x + radius * sinf(0 + 4 * PI / 3),pos.y,pos.z + radius * cosf(0 + 4 * PI / 3),
            color.r,color.g,color.b,

            pos.x + radius * sinf(0)			 ,pos.y      ,pos.z + radius * cosf(0),
            color.r,color.g,color.b,
            pos.x + radius * sinf(0 + 2 * PI / 3),pos.y      ,pos.z + radius * cosf(0 + 2 * PI / 3),
            color.r,color.g,color.b,    
            pos.x                                ,pos.y + radius ,pos.z,
            color.r,color.g,color.b,

            pos.x                                ,pos.y + radius ,pos.z,
            color.r,color.g,color.b,
            pos.x + radius * sinf(0 + 2 * PI / 3),pos.y ,pos.z + radius * cosf(0 + 2 * PI / 3),
            color.r,color.g,color.b,
            pos.x + radius * sinf(0 + 4 * PI / 3),pos.y ,pos.z + radius * cosf(0 + 4 * PI / 3),
            color.r,color.g,color.b,

            pos.x + radius * sinf(0)			 ,pos.y ,pos.z + radius * cosf(0),
            color.r,color.g,color.b,
            pos.x                                ,pos.y + radius ,pos.z,
            color.r,color.g,color.b,
            pos.x + radius * sinf(0 + 4 * PI / 3),pos.y ,pos.z + radius * cosf(0 + 4 * PI / 3),
            color.r,color.g,color.b
        };
        RenderTriangles(vertices , 4);
    }

	void RenderLine(std::vector<Point2> line, Color3f color)
	{
		if (line.size() < 2)
		{
			printf("BatchRenderer line not enough vertices\n");
			return;
		}
		m_lines.push_back(line[0].x);
        m_lines.push_back(0.0f);
		m_lines.push_back(line[0].y);
		m_lines.push_back(color.r);
		m_lines.push_back(color.g);
		m_lines.push_back(color.b);
		m_lines.push_back(line[1].x);
        m_lines.push_back(0.0f);
		m_lines.push_back(line[1].y);
		m_lines.push_back(color.r);
		m_lines.push_back(color.g);
		m_lines.push_back(color.b);
	}
	void RenderLine(float x0, float y0, float x1, float y1, Color3f color)
	{
		m_lines.emplace_back(x0);
        m_lines.emplace_back(0.0f);
		m_lines.emplace_back(y0);
		m_lines.emplace_back(color.r);
		m_lines.emplace_back(color.g);
		m_lines.emplace_back(color.b);
		m_lines.emplace_back(x1);
        m_lines.emplace_back(0.0f);
		m_lines.emplace_back(y1);
		m_lines.emplace_back(color.r);
		m_lines.emplace_back(color.g);
		m_lines.emplace_back(color.b);
	}
	void RenderTriangleFan(std::vector<Point2> trianglefan, Color3f color)
	{
		if (trianglefan.size() < 3)
		{
			printf("BatchRenderer trianglefan not enough vertices\n");
			return;
		}
		std::vector<float> fan;
		for (uint i = 0; i < trianglefan.size(); i++)
		{
			fan.push_back(trianglefan[i].x);
            fan.push_back(0);
			fan.push_back(trianglefan[i].y);
			fan.push_back(color.r);
			fan.push_back(color.g);
			fan.push_back(color.b);
		}
		m_trianglefans.push_back(fan);
	}
	void RenderLineStrip(std::vector<Point2> linestrip, Color3f color)
	{
		if (linestrip.size() < 2)
		{
			printf("BatchRenderer linestrip not enough vertices\n");
			return;
		}
		std::vector<float> strip;
		for (uint i = 0; i < linestrip.size(); i++)
		{
			strip.push_back(linestrip[i].x);
            strip.push_back(0);
			strip.push_back(linestrip[i].y);
			strip.push_back(color.r);
			strip.push_back(color.g);
			strip.push_back(color.b);
		}
		m_linestrips.push_back(strip);
	}
    void RenderCircleHollow(Point2 pos, float radius, Color3f color, int numvertices = 21)
    {
        std::vector<Point2> linestrip;


        float dAngle = PI / 10.0f;

        for (int i = 0; i < numvertices; i++)
        {
            linestrip.push_back({ radius * cosf(i * dAngle) + pos.x , radius * sinf(i * dAngle) + pos.y });
        }
        RenderLineStrip(linestrip, color);
    }
    void RenderCircleFilled(Point2 pos, float radius, Color3f color, int numvertices = 20)
    {
        std::vector<Point2> trianglefan;


        float dAngle = PI / 10.0f;

        for (int i = 0; i < numvertices; i++)
        {
            trianglefan.push_back({ radius * cosf(i * dAngle) + pos.x , radius * sinf(i * dAngle) + pos.y });
        }
        RenderTriangleFan(trianglefan, color);
    }

    void RenderExtrudedShape(std::vector<glm::vec3> points, float height, Color3f color)
    {
        int triangles = (int)points.size() - 2;
		int sides = (int)points.size();
        assert(triangles > 0);
        float* vertices = new float[2*6*3*triangles + 2*6*3*sides];

        for (int i = 0; i < triangles; i++)
        {
            vertices[i * 18 + 0] = points[0].x;
            vertices[i * 18 + 1] = points[0].y;
            vertices[i * 18 + 2] = points[0].z;
            vertices[i * 18 + 3] = color.r;
            vertices[i * 18 + 4] = color.g;
            vertices[i * 18 + 5] = color.b;
						 
            vertices[i * 18 + 6] = points[i + 1].x;
            vertices[i * 18 + 7] = points[i + 1].y;
            vertices[i * 18 + 8] = points[i + 1].z;
            vertices[i * 18 + 9] = color.r;
            vertices[i * 18 + 10] = color.g;
            vertices[i * 18 + 11] = color.b;
						 
            vertices[i * 18 + 12] = points[i + 2].x;
            vertices[i * 18 + 13] = points[i + 2].y;
            vertices[i * 18 + 14] = points[i + 2].z;
            vertices[i * 18 + 15] = color.r;
            vertices[i * 18 + 16] = color.g;
            vertices[i * 18 + 17] = color.b;
        }
        for (uint i = 0; i < points.size(); i++)
        {
            points[i].y += height;
        }
        for (int i = 0; i < triangles; i++)
        {
            vertices[18*triangles + i * 18 + 0] = points[0].x;
            vertices[18*triangles + i * 18 + 1] = points[0].y;
            vertices[18*triangles + i * 18 + 2] = points[0].z;
            vertices[18*triangles + i * 18 + 3] = color.r;
            vertices[18*triangles + i * 18 + 4] = color.g;
            vertices[18*triangles + i * 18 + 5] = color.b;
                     					
            vertices[18*triangles + i * 18 + 6] = points[i + 1].x;
            vertices[18*triangles + i * 18 + 7] = points[i + 1].y;
            vertices[18*triangles + i * 18 + 8] = points[i + 1].z;
            vertices[18*triangles + i * 18 + 9] = color.r;
            vertices[18*triangles + i * 18 + 10] = color.g;
            vertices[18*triangles + i * 18 + 11] = color.b;
                     					
            vertices[18*triangles + i * 18 + 12] = points[i + 2].x;
            vertices[18*triangles + i * 18 + 13] = points[i + 2].y;
            vertices[18*triangles + i * 18 + 14] = points[i + 2].z;
            vertices[18*triangles + i * 18 + 15] = color.r;
            vertices[18*triangles + i * 18 + 16] = color.g;
            vertices[18*triangles + i * 18 + 17] = color.b;
        }
		for (uint i = 0; i < points.size(); i++)
		{
			points[i].y -= height;
		}
		for (int i = 0; i < sides; i++)
		{
			int j = i == sides - 1 ? 0 : i + 1;
			vertices[36*triangles + i * 36 + 0] = points[i].x;
			vertices[36*triangles + i * 36 + 1] = points[i].y;
			vertices[36*triangles + i * 36 + 2] = points[i].z;
			vertices[36*triangles + i * 36 + 3] = color.r;
			vertices[36*triangles + i * 36 + 4] = color.g;
			vertices[36*triangles + i * 36 + 5] = color.b;
								 		
			vertices[36*triangles + i * 36 + 6] = points[i].x;
			vertices[36*triangles + i * 36 + 7] = points[i].y + height;
			vertices[36*triangles + i * 36 + 8] = points[i].z;
			vertices[36*triangles + i * 36 + 9] = color.r;
			vertices[36*triangles + i * 36 + 10] = color.g;
			vertices[36*triangles + i * 36 + 11] = color.b;
									 	
			vertices[36*triangles + i * 36 + 12] = points[j].x;
			vertices[36*triangles + i * 36 + 13] = points[j].y;
			vertices[36*triangles + i * 36 + 14] = points[j].z;
			vertices[36*triangles + i * 36 + 15] = color.r;
			vertices[36*triangles + i * 36 + 16] = color.g;
			vertices[36*triangles + i * 36 + 17] = color.b;
							 			
			vertices[36*triangles + i * 36 + 18] = points[j].x;
			vertices[36*triangles + i * 36 + 19] = points[j].y;
			vertices[36*triangles + i * 36 + 20] = points[j].z;
			vertices[36*triangles + i * 36 + 21] = color.r;
			vertices[36*triangles + i * 36 + 22] = color.g;
			vertices[36*triangles + i * 36 + 23] = color.b;
								 		
			vertices[36*triangles + i * 36 + 24] = points[j].x;
			vertices[36*triangles + i * 36 + 25] = points[j].y + height;
			vertices[36*triangles + i * 36 + 26] = points[j].z;
			vertices[36*triangles + i * 36 + 27] = color.r;
			vertices[36*triangles + i * 36 + 28] = color.g;
			vertices[36*triangles + i * 36 + 29] = color.b;
								 	
			vertices[36*triangles + i * 36 + 30] = points[i].x;
			vertices[36*triangles + i * 36 + 31] = points[i].y + height;
			vertices[36*triangles + i * 36 + 32] = points[i].z;
			vertices[36*triangles + i * 36 + 33] = color.r;
			vertices[36*triangles + i * 36 + 34] = color.g;
			vertices[36*triangles + i * 36 + 35] = color.b;
		}

        RenderTriangles(vertices, 2*triangles + 2*sides);

        delete[] vertices;
    }

    void RenderMesh(std::string mesh, glm::mat4 model_mat, Material material)
    {
        m_meshes.push_back({ mesh, model_mat, material });
    }

    //TODO: May not work
	void LoadShader(const char* shader);

    //TODO: May not work
	GLuint LoadTexture(const char * filename);

    //TODO: May not work
	GLint compileShader(const char* filename, GLenum type);

	GLint compileShaderProgramDefault(const char* vSharerSrc, const char* fSharerSrc) 
	{

		//Vertex Shader
		const GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		if (!vertexShader) {
			std::cerr << "Cannot create a shader of type " << vertexShader << std::endl;
			//abort();
		}

		glShaderSource(vertexShader, 1, &vSharerSrc, NULL);
		glCompileShader(vertexShader);

		GLint compiledv;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiledv);
		if (!compiledv) {
			std::cerr << "Cannot compile vertex shader " << std::endl;
			//abort();
		}

		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		printf("%s", buffer);

		//Fragment Shader
		const GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		if (!fragmentShader) {
			std::cerr << "Cannot create a shader of type " << fragmentShader << std::endl;
			//abort();
		}

		glShaderSource(fragmentShader, 1, &fSharerSrc, NULL);
		glCompileShader(fragmentShader);

		GLint compiledf;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiledf);
		if (!compiledf) {
			std::cerr << "Cannot compile fragment shader " << std::endl;
			//abort();
		}

		char buffer2[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer2);
		printf("%s", buffer2);

		//Shader Program
		const GLint program = glCreateProgram();

		if (!program) {
			std::cerr << "Cannot create a shader program" << std::endl;
			//abort();
		}

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);


		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (!linked) {
			std::cerr << "Cannot link shader program with default shaders" << std::endl;
			//abort();
		}


		char buffer3[512];
		glGetProgramInfoLog(program, 512, NULL, buffer3);
		printf("%s", buffer3);

		printf("%s\n", glewGetErrorString(glGetError()));

		return program;

	}
    void SetInt(int location, int value)
    {
        glUniform1i(location, value);
    }
    void SetFloat(int location, float value)
    {
        glUniform1f(location, value);
    }
    void SetVec3(int location, glm::vec3 value)
    {
        glUniform3fv(location, 1, &value[0]);
    }
    void SetMat4(int location, glm::mat4 value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }
    void SetPointLight(int num, const PointLight& Light)
    {
        glUniform3f(200 + 4 * num, Light.Color.x, Light.Color.y, Light.Color.z);
        glUniform1f(201 + 4 * num, Light.AmbientIntensity);
        glUniform1f(202 + 4 * num, Light.DiffuseIntensity);
        Vector3f Position = Light.Position;
        glUniform3f(203 + 4 * num, Position.x, Position.y, Position.z);
    }
    void SetDirectionalLight(int num, const DirectionalLight& Light)
    {
        glUniform3f(100 + 4 * num, Light.Color.x, Light.Color.y, Light.Color.z);
        glUniform1f(101 + 4 * num, Light.AmbientIntensity);
        glUniform1f(102 + 4 * num, Light.DiffuseIntensity);
        Vector3f Direction = Light.Direction;
        Direction = glm::normalize(Direction);
        glUniform3f(103 + 4 * num, Direction.x, Direction.y, Direction.z);
    }

    void SetMaterial(const Material& Material)
    {
        glUniform3f(13, Material.ambient.x, Material.ambient.y, Material.ambient.z);
        glUniform3f(14, Material.diffuse.x, Material.diffuse.y, Material.diffuse.z);
        glUniform3f(15, Material.specular.x, Material.specular.y, Material.specular.z);
        glUniform1f(16, Material.shininess);
    }

    void SetBlinn(bool blinn)
    {
        glUniform1i(23, blinn);
    }
    
    void SetLightAmountP(int numm)
    {
        glUniform1i(24, numm);
    }
    void SetLightAmountD(int numm)
    {
        glUniform1i(25, numm);
    }

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


	void _RenderTriangles()
	{
		int numvertices;
		numvertices = (int)m_triangles.size() / 6;//X,Y,Z,R,G,B
        if (numvertices < 3)return;
		float* vertices;
		vertices = &m_triangles[0];

		glUseProgram(m_shaderprogram);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBufferData(GL_ARRAY_BUFFER, 6 * numvertices * sizeof(float), vertices, GL_STATIC_DRAW);

		glUniformMatrix4fv(2, 1, GL_FALSE, &m_mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, numvertices);
		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void _RenderLines()
	{
		int numvertices;
		numvertices = (int)m_lines.size() / 6;//X,Y,Z,R,G,B
        if (numvertices < 2)return;
		float* vertices;
		vertices = &m_lines[0];

		glUseProgram(m_shaderprogram);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBufferData(GL_ARRAY_BUFFER, 6 * numvertices * sizeof(float), vertices, GL_STATIC_DRAW);

		glUniformMatrix4fv(2, 1, GL_FALSE, &m_mvp[0][0]);

		glDrawArrays(GL_LINES, 0, numvertices);
		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void _RenderTriangleFans()
	{
		int numvertices;
		float* vertices;
		glUseProgram(m_shaderprogram);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		for (uint i = 0; i < m_trianglefans.size(); i++)
		{
			numvertices = (int)m_trianglefans[i].size() / 6;//X,Y,Z,R,G,B
            if (numvertices < 3)continue;
			vertices = &m_trianglefans[i][0];

			glBufferData(GL_ARRAY_BUFFER, 6 * numvertices * sizeof(float), vertices, GL_STATIC_DRAW);

			glUniformMatrix4fv(2, 1, GL_FALSE, &m_mvp[0][0]);

			glDrawArrays(GL_TRIANGLE_FAN, 0, numvertices);
		}

		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void _RenderLineStrips()
	{
		int numvertices;
		float* vertices;
		glUseProgram(m_shaderprogram);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		for (uint i = 0; i < m_linestrips.size(); i++)
		{
			numvertices = (int)m_linestrips[i].size() / 6;//X,Y,Z,R,G,B
            if (numvertices < 2)continue;
			vertices = &m_linestrips[i][0];

			glBufferData(GL_ARRAY_BUFFER, 6 * numvertices * sizeof(float), vertices, GL_STATIC_DRAW);

			glUniformMatrix4fv(2, 1, GL_FALSE, &m_mvp[0][0]);

			glDrawArrays(GL_LINE_STRIP, 0, numvertices);
		}

		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


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



	std::vector<float> m_triangles;
	std::vector<float> m_lines;
	std::vector<std::vector<float>> m_trianglefans;
	std::vector<std::vector<float>> m_linestrips;

    std::map<std::string, Mesh*> m_loaded_meshes;
    std::vector<std::tuple<std::string, glm::mat4, Material>> m_meshes;


};