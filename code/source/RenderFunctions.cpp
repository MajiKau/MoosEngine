#include "code/headers/RenderFunctions.h"

#include <fstream>

const float PI = 3.14159265359f; 

const Color3f RED = Color3f(1, 0, 0);
const Color3f GREEN = Color3f(0, 1, 0);
const Color3f BLUE = Color3f(0, 0, 1);
const Color3f WHITE = Color3f(1, 1, 1);
const Color3f BLACK = Color3f(0, 0, 0);
const Color3f YELLOW = Color3f(1, 1, 0);

void BatchRenderer::RenderSkybox()
{
	glm::mat4 view = glm::mat4(glm::mat3(m_view));

	glDepthMask(GL_FALSE);
	glUseProgram(m_shaderprogram_skybox);
	glUniformMatrix4fv(1, 1, GL_FALSE, &m_projection[0][0]);
	glUniformMatrix4fv(2, 1, GL_FALSE, &view[0][0]);	
	
	glEnableVertexAttribArray(0);
	glBindVertexArray(m_vao_skybox);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);

	m_skybox_texture->Bind(GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glUseProgram(0);
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDepthMask(GL_TRUE);
}

GLchar* BatchRenderer::LoadShader(const char* filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	file.seekg(0, file.end);
	int length = file.tellg();
	file.seekg(0, file.beg);
	GLchar* data = new GLchar[length + 1];
	file.read(data, length); 
	data[length] = '\0';
	file.close();
	return data;

	/*FILE* file;
	fopen_s(&file, filename, "rb");

	if (file == NULL) {
		std::cerr << "Cannot open shader " << filename << std::endl;
		abort();
	}

	fseek(file, 0, SEEK_END);
	const int size = ftell(file);
	rewind(file);

	const GLchar* source = new GLchar[size + 1];
	fread(const_cast<char*>(source), sizeof(char), size, file);
	const_cast<char&>(source[size]) = '\0';*/
}

GLuint BatchRenderer::LoadTexture(const char * filename)
{
	GLuint texture;

	int width, height;

	unsigned char * data;


	FILE * file;
	fopen_s(&file, filename, "rb");

	if (file == NULL)
	{
		printf("Can't open file %s\n", filename);
		return 0;
	}
	width = 256;
	height = 256;
	data = (unsigned char *)malloc(width * height * 3);

	fread(data, width * height * 3, 1, file);
	fclose(file);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}


GLint BatchRenderer::compileShader(const char* filename, GLenum type) {

	FILE* file;
	fopen_s(&file, filename, "rb");

	if (file == NULL) {
		std::cerr << "Cannot open shader " << filename << std::endl;
		abort();
	}

	fseek(file, 0, SEEK_END);
	const int size = ftell(file);
	rewind(file);

	const GLchar* source = new GLchar[size + 1];
	fread(const_cast<char*>(source), sizeof(char), size, file);
	const_cast<char&>(source[size]) = '\0';

	const GLint shader = glCreateShader(type);

	if (!shader) {
		std::cerr << "Cannot create a shader of type " << shader << std::endl;
		abort();
	}

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	{
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			std::cerr << "Cannot compile shader " << filename << std::endl;
			abort();
		}
	}

	return shader;
}

void BatchRenderer::CompileShaders()
{
	//Not textured
	GLchar* vertex = LoadShader("Content/Shaders/Basic/shader.vertex");
	GLchar* fragment = LoadShader("Content/Shaders/Basic/shader.fragment");
	m_shaderprogram = compileShaderProgramDefault(vertex, fragment);

	//Textured
	vertex = LoadShader("Content/Shaders/Basic_Textured/shader.vertex");
	fragment = LoadShader("Content/Shaders/Basic_Textured/shader.fragment");
	m_shaderprogram_textured = compileShaderProgramDefault(vertex, fragment);

	//Better textured
	vertex = LoadShader("Content/Shaders/MVP_Textured/shader.vertex");
	fragment = LoadShader("Content/Shaders/MVP_Textured/shader.fragment");
	m_shaderprogram_mvp_textured = compileShaderProgramDefault(vertex, fragment);

	//Unlit textured
	vertex = LoadShader("Content/Shaders/MVP_Textured_Unlit/shader.vertex");
	fragment = LoadShader("Content/Shaders/MVP_Textured_Unlit/shader.fragment");
	m_shaderprogram_mvp_textured_unlit = compileShaderProgramDefault(vertex, fragment);

	//Skybox
	vertex = LoadShader("Content/Shaders/Skybox/shader.vertex");
	fragment = LoadShader("Content/Shaders/Skybox/shader.fragment");
	m_shaderprogram_skybox = compileShaderProgramDefault(vertex, fragment);
}

void RenderVector3(Vec3 vector, Color3f color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    float vertices[2 * 3] =
    {
        0           ,0          ,-20,
        vector.x    ,vector.y   ,-20-vector.z
    };
    float colors[3 * 2] =
    {
        color.r,color.g,color.b,
        color.r,color.g,color.b
    };

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glDrawArrays(GL_LINES, 0, 2);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
}

void RenderVector3At(Vec3 position, Vec3 vector, Color3f color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    float vertices[2 * 3] =
    {
        position.x              ,position.y             ,-20 - position.z,
        position.x + vector.x   ,position.y + vector.y  ,-20 - position.z - vector.z
    };
    float colors[3 * 2] =
    {
        color.r,color.g,color.b,
        color.r,color.g,color.b
    };

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    glDrawArrays(GL_LINES, 0, 2);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

}
