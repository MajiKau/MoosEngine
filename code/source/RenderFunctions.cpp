#include "code/headers/RenderFunctions.h"

#include <fstream>
#include <gtx/matrix_decompose.hpp>

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

void BatchRenderer::RenderTriangle(float * vertices)
{
	m_triangles.insert(m_triangles.end(), vertices, vertices + 18);
}

void BatchRenderer::RenderTriangles(float * vertices, int numoftriangles)
{
	m_triangles.insert(m_triangles.end(), vertices, vertices + numoftriangles * 18);
}

void BatchRenderer::RenderTriangle(std::vector<Point2> triangle, Color3f color)
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

void BatchRenderer::RenderRegularTriangle(Point2 pos, float radius, float angle, Color3f color)
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

void BatchRenderer::RenderRegularTetrahedron(glm::vec3 pos, float radius, Color3f color)
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
	RenderTriangles(vertices, 4);
}

void BatchRenderer::RenderLine(Vector3f pos1, Vector3f pos2, Color3f color)
{
	float temp_lines[] =
	{
		pos1.x,
		pos1.y,
		pos1.z,
		color.r,
		color.g,
		color.b,
		pos2.x,
		pos2.y,
		pos2.z,
		color.r,
		color.g,
		color.b
	};
	m_lines.insert(m_lines.end(), &temp_lines[0], &temp_lines[0] + 12);

}

void BatchRenderer::RenderLines(float * lines, int number_of_lines)
{
	m_lines.insert(m_lines.end(), &lines[0], &lines[0] + number_of_lines * 12);
}

void BatchRenderer::RenderLineStrip(std::vector<float> linestrip)
{
	m_linestrips.emplace_back(linestrip);
}

void BatchRenderer::RenderLineStrip(std::vector<Vector3f> linestrip, Color3f color)
{
	if (linestrip.size() < 2)
	{
		printf("BatchRenderer linestrip not enough vertices\n");
		return;
	}
	std::vector<float> strip;
	for (uint i = 0; i < linestrip.size(); i++)
	{
		float temp_strip[] =
		{
			linestrip[i].x,
			linestrip[i].y,
			linestrip[i].z,
			color.r,
			color.g,
			color.b
		};
		strip.insert(strip.end(), &temp_strip[0], &temp_strip[0] + 6);
	}
	m_linestrips.emplace_back(strip);
}
void BatchRenderer::RenderLine(std::vector<Point2> line, Color3f color)
{
	if (line.size() < 2)
	{
		printf("BatchRenderer line not enough vertices\n");
		return;
	}
	m_lines.emplace_back(line[0].x);
	m_lines.emplace_back(0.0f);
	m_lines.emplace_back(line[0].y);
	m_lines.emplace_back(color.r);
	m_lines.emplace_back(color.g);
	m_lines.emplace_back(color.b);
	m_lines.emplace_back(line[1].x);
	m_lines.emplace_back(0.0f);
	m_lines.emplace_back(line[1].y);
	m_lines.emplace_back(color.r);
	m_lines.emplace_back(color.g);
	m_lines.emplace_back(color.b);
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
GLint BatchRenderer::compileShaderProgramDefault(const char* vSharerSrc, const char* fSharerSrc)
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
GLint BatchRenderer::GetUniformLocation(GLchar* uniform_name)
{
	return glGetUniformLocation(m_shaderprogram_mvp_textured, uniform_name);
}
GLint BatchRenderer::GetAttributeLocation(GLchar* attribute_name)
{
	return glGetAttribLocation(m_shaderprogram_mvp_textured, attribute_name);
}

void BatchRenderer::SetInt(int location, int value)
{
	glUniform1i(location, value);
}
void BatchRenderer::SetFloat(int location, float value)
{
	glUniform1f(location, value);
}
void BatchRenderer::SetVec3(int location, glm::vec3 value)
{
	glUniform3fv(location, 1, &value[0]);
}
void BatchRenderer::SetMat4(int location, glm::mat4 value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}
void BatchRenderer::SetPointLight(int num, const PointLight& Light)
{
	glUniform3f(200 + 4 * num, Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(201 + 4 * num, Light.AmbientIntensity);
	glUniform1f(202 + 4 * num, Light.DiffuseIntensity);
	Vector3f Position = Light.Position;
	glUniform3f(203 + 4 * num, Position.x, Position.y, Position.z);
}
void BatchRenderer::SetDirectionalLight(int num, const DirectionalLight& Light)
{
	glUniform3f(100 + 4 * num, Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(101 + 4 * num, Light.AmbientIntensity);
	glUniform1f(102 + 4 * num, Light.DiffuseIntensity);
	Vector3f Direction = Light.Direction;
	Direction = glm::normalize(Direction);
	glUniform3f(103 + 4 * num, Direction.x, Direction.y, Direction.z);
}

void BatchRenderer::SetMaterial(const Material& Material)
{
	glUniform3f(13, Material.ambient.x, Material.ambient.y, Material.ambient.z);
	glUniform3f(14, Material.diffuse.x, Material.diffuse.y, Material.diffuse.z);
	glUniform3f(15, Material.specular.x, Material.specular.y, Material.specular.z);
	glUniform1f(16, Material.shininess);
}

void BatchRenderer::SetBlinn(bool blinn)
{
	glUniform1i(23, blinn);
}

void BatchRenderer::SetLightAmountP(int numm)
{
	glUniform1i(24, numm);
}
void BatchRenderer::SetLightAmountD(int numm)
{
	glUniform1i(25, numm);
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

void BatchRenderer::RenderCuboid(const Cuboid & cuboid, Color3f color)
{
	float lines[] =
	{
		cuboid.x, cuboid.y, cuboid.z, color.r, color.g, color.b,
		cuboid.x + cuboid.w, cuboid.y, cuboid.z, color.r, color.g, color.b,
		cuboid.x + cuboid.w, cuboid.y, cuboid.z, color.r, color.g, color.b,
		cuboid.x + cuboid.w, cuboid.y + cuboid.h, cuboid.z, color.r, color.g, color.b,
		cuboid.x + cuboid.w, cuboid.y + cuboid.h, cuboid.z, color.r, color.g, color.b,
		cuboid.x, cuboid.y + cuboid.h, cuboid.z, color.r, color.g, color.b,
		cuboid.x, cuboid.y + cuboid.h, cuboid.z, color.r, color.g, color.b,
		cuboid.x, cuboid.y, cuboid.z, color.r, color.g, color.b,

		cuboid.x,cuboid.y,cuboid.z + cuboid.d ,color.r,color.g,color.b,
		cuboid.x + cuboid.w,cuboid.y,cuboid.z + cuboid.d ,color.r,color.g,color.b,
		cuboid.x + cuboid.w,cuboid.y,cuboid.z + cuboid.d ,color.r,color.g,color.b,
		cuboid.x + cuboid.w,cuboid.y + cuboid.h,cuboid.z + cuboid.d ,color.r,color.g,color.b,
		cuboid.x + cuboid.w,cuboid.y + cuboid.h,cuboid.z + cuboid.d ,color.r,color.g,color.b,
		cuboid.x ,cuboid.y + cuboid.h,cuboid.z + cuboid.d ,color.r,color.g,color.b,
		cuboid.x ,cuboid.y + cuboid.h,cuboid.z + cuboid.d ,color.r,color.g,color.b,
		cuboid.x ,cuboid.y,cuboid.z + cuboid.d,color.r,color.g,color.b,

		cuboid.x, cuboid.y, cuboid.z,color.r,color.g,color.b,
		cuboid.x, cuboid.y, cuboid.z + cuboid.d,color.r,color.g,color.b,

		cuboid.x + cuboid.w, cuboid.y, cuboid.z,color.r,color.g,color.b,
		cuboid.x + cuboid.w, cuboid.y, cuboid.z + cuboid.d,color.r,color.g,color.b,

		cuboid.x + cuboid.w, cuboid.y + cuboid.h, cuboid.z,color.r,color.g,color.b,
		cuboid.x + cuboid.w, cuboid.y + cuboid.h, cuboid.z + cuboid.d,color.r,color.g,color.b,

		cuboid.x, cuboid.y + cuboid.h, cuboid.z,color.r,color.g,color.b,
		cuboid.x, cuboid.y + cuboid.h, cuboid.z + cuboid.d,color.r,color.g,color.b
	};
	RenderLines(lines, 12);
}

void BatchRenderer::RenderLine(float x0, float y0, float x1, float y1, Color3f color)
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
void BatchRenderer::RenderTriangleFan(std::vector<Point2> trianglefan, Color3f color)
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
void BatchRenderer::RenderLineStrip(std::vector<Point2> linestrip, Color3f color)
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
void BatchRenderer::RenderCircleHollow(Point2 pos, float radius, Color3f color, int numvertices)
{
	std::vector<Point2> linestrip;


	float dAngle = PI / 10.0f;

	for (int i = 0; i < numvertices; i++)
	{
		linestrip.push_back({ radius * cosf(i * dAngle) + pos.x , radius * sinf(i * dAngle) + pos.y });
	}
	RenderLineStrip(linestrip, color);
}
void BatchRenderer::RenderCircleFilled(Point2 pos, float radius, Color3f color, int numvertices)
{
	std::vector<Point2> trianglefan;


	float dAngle = PI / 10.0f;

	for (int i = 0; i < numvertices; i++)
	{
		trianglefan.push_back({ radius * cosf(i * dAngle) + pos.x , radius * sinf(i * dAngle) + pos.y });
	}
	RenderTriangleFan(trianglefan, color);
}

void BatchRenderer::RenderExtrudedShape(std::vector<glm::vec3> points, float height, Color3f color)
{
	int triangles = (int)points.size() - 2;
	int sides = (int)points.size();
	assert(triangles > 0);
	float* vertices = new float[2 * 6 * 3 * triangles + 2 * 6 * 3 * sides];

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
		vertices[18 * triangles + i * 18 + 0] = points[0].x;
		vertices[18 * triangles + i * 18 + 1] = points[0].y;
		vertices[18 * triangles + i * 18 + 2] = points[0].z;
		vertices[18 * triangles + i * 18 + 3] = color.r;
		vertices[18 * triangles + i * 18 + 4] = color.g;
		vertices[18 * triangles + i * 18 + 5] = color.b;

		vertices[18 * triangles + i * 18 + 6] = points[i + 1].x;
		vertices[18 * triangles + i * 18 + 7] = points[i + 1].y;
		vertices[18 * triangles + i * 18 + 8] = points[i + 1].z;
		vertices[18 * triangles + i * 18 + 9] = color.r;
		vertices[18 * triangles + i * 18 + 10] = color.g;
		vertices[18 * triangles + i * 18 + 11] = color.b;

		vertices[18 * triangles + i * 18 + 12] = points[i + 2].x;
		vertices[18 * triangles + i * 18 + 13] = points[i + 2].y;
		vertices[18 * triangles + i * 18 + 14] = points[i + 2].z;
		vertices[18 * triangles + i * 18 + 15] = color.r;
		vertices[18 * triangles + i * 18 + 16] = color.g;
		vertices[18 * triangles + i * 18 + 17] = color.b;
	}
	for (uint i = 0; i < points.size(); i++)
	{
		points[i].y -= height;
	}
	for (int i = 0; i < sides; i++)
	{
		int j = i == sides - 1 ? 0 : i + 1;
		vertices[36 * triangles + i * 36 + 0] = points[i].x;
		vertices[36 * triangles + i * 36 + 1] = points[i].y;
		vertices[36 * triangles + i * 36 + 2] = points[i].z;
		vertices[36 * triangles + i * 36 + 3] = color.r;
		vertices[36 * triangles + i * 36 + 4] = color.g;
		vertices[36 * triangles + i * 36 + 5] = color.b;

		vertices[36 * triangles + i * 36 + 6] = points[i].x;
		vertices[36 * triangles + i * 36 + 7] = points[i].y + height;
		vertices[36 * triangles + i * 36 + 8] = points[i].z;
		vertices[36 * triangles + i * 36 + 9] = color.r;
		vertices[36 * triangles + i * 36 + 10] = color.g;
		vertices[36 * triangles + i * 36 + 11] = color.b;

		vertices[36 * triangles + i * 36 + 12] = points[j].x;
		vertices[36 * triangles + i * 36 + 13] = points[j].y;
		vertices[36 * triangles + i * 36 + 14] = points[j].z;
		vertices[36 * triangles + i * 36 + 15] = color.r;
		vertices[36 * triangles + i * 36 + 16] = color.g;
		vertices[36 * triangles + i * 36 + 17] = color.b;

		vertices[36 * triangles + i * 36 + 18] = points[j].x;
		vertices[36 * triangles + i * 36 + 19] = points[j].y;
		vertices[36 * triangles + i * 36 + 20] = points[j].z;
		vertices[36 * triangles + i * 36 + 21] = color.r;
		vertices[36 * triangles + i * 36 + 22] = color.g;
		vertices[36 * triangles + i * 36 + 23] = color.b;

		vertices[36 * triangles + i * 36 + 24] = points[j].x;
		vertices[36 * triangles + i * 36 + 25] = points[j].y + height;
		vertices[36 * triangles + i * 36 + 26] = points[j].z;
		vertices[36 * triangles + i * 36 + 27] = color.r;
		vertices[36 * triangles + i * 36 + 28] = color.g;
		vertices[36 * triangles + i * 36 + 29] = color.b;

		vertices[36 * triangles + i * 36 + 30] = points[i].x;
		vertices[36 * triangles + i * 36 + 31] = points[i].y + height;
		vertices[36 * triangles + i * 36 + 32] = points[i].z;
		vertices[36 * triangles + i * 36 + 33] = color.r;
		vertices[36 * triangles + i * 36 + 34] = color.g;
		vertices[36 * triangles + i * 36 + 35] = color.b;
	}

	RenderTriangles(vertices, 2 * triangles + 2 * sides);

	delete[] vertices;
}

void BatchRenderer::RenderMesh(std::string mesh, glm::mat4 model_mat, Material material, std::set<int> render_layer)
{
	m_meshes.push_back({ mesh, model_mat, material, render_layer });
}

void BatchRenderer::RenderPortal(std::string mesh, glm::mat4 model_mat, glm::mat4 other_model_mat, std::set<int> render_layers, int portal_render_layer, bool m_flip_clip_plane)
{
	m_portals.push_back({ mesh, model_mat, other_model_mat, render_layers, portal_render_layer, m_flip_clip_plane });
}

BatchRenderer::BatchRenderer(float Zoom, float Ratio)
{
	m_triangles = std::vector<float>();
	m_lines = std::vector<float>();
	m_trianglefans = std::vector<std::vector<float>>();
	m_linestrips = std::vector<std::vector<float>>();

	//m_texture = LoadTexture("test3.bmp");

	//Not textured
	GLchar* vertex = LoadShader("Content/Shaders/Basic/shader.vertex");
	GLchar* fragment = LoadShader("Content/Shaders/Basic/shader.fragment");
	m_shaderprogram = compileShaderProgramDefault(vertex, fragment);

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
	vertex = LoadShader("Content/Shaders/Basic_Textured/shader.vertex");
	fragment = LoadShader("Content/Shaders/Basic_Textured/shader.fragment");
	m_shaderprogram_textured = compileShaderProgramDefault(vertex, fragment);

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
	vertex = LoadShader("Content/Shaders/MVP_Textured/shader.vertex");
	fragment = LoadShader("Content/Shaders/MVP_Textured/shader.fragment");
	m_shaderprogram_mvp_textured = compileShaderProgramDefault(vertex, fragment);

	//Unlit textured
	vertex = LoadShader("Content/Shaders/MVP_Textured_Unlit/shader.vertex");
	fragment = LoadShader("Content/Shaders/MVP_Textured_Unlit/shader.fragment");
	m_shaderprogram_mvp_textured_unlit = compileShaderProgramDefault(vertex, fragment);

	//Skybox
	float skyboxVertices[] =
	{
		// positions          
		1.0f, -1.0f, -1.0f	   ,
		1.0f, 1.0f, -1.0f	   ,
		-1.0f, 1.0f, -1.0f	   ,
		-1.0f, 1.0f, -1.0f	   ,
		-1.0f, -1.0f, -1.0f	   ,
		1.0f, -1.0f, -1.0f	   ,

		-1.0f, -1.0f, 1.0f	   ,
		-1.0f, 1.0f, 1.0f	   ,
		1.0f, 1.0f, 1.0f	   ,
		1.0f, 1.0f, 1.0f	   ,
		1.0f, -1.0f, 1.0f	   ,
		-1.0f, -1.0f, 1.0f	   ,

		-1.0f, 1.0f, 1.0f	   ,
		-1.0f, -1.0f, 1.0f	   ,
		-1.0f, -1.0f, -1.0f    ,
		-1.0f, -1.0f, -1.0f    ,
		-1.0f, 1.0f, -1.0f	   ,
		-1.0f, 1.0f, 1.0f	   ,

		1.0f, 1.0f, -1.0f	   ,
		1.0f, -1.0f, -1.0f	   ,
		1.0f, -1.0f, 1.0f	   ,
		1.0f, -1.0f, 1.0f	   ,
		1.0f, 1.0f, 1.0f	   ,
		1.0f, 1.0f, -1.0f	   ,

		-1.0f, -1.0f, -1.0f	   ,
		-1.0f, -1.0f, 1.0f	   ,
		1.0f, -1.0f, 1.0f	   ,
		1.0f, -1.0f, 1.0f	   ,
		1.0f, -1.0f, -1.0f	   ,
		-1.0f, -1.0f, -1.0f	   ,

		1.0f, 1.0f, -1.0f	   ,
		1.0f, 1.0f, 1.0f	   ,
		-1.0f, 1.0f, 1.0f	   ,
		-1.0f, 1.0f, 1.0f	   ,
		-1.0f, 1.0f, -1.0f	   ,
		1.0f, 1.0f, -1.0f
	};
	vertex = LoadShader("Content/Shaders/Skybox/shader.vertex");
	fragment = LoadShader("Content/Shaders/Skybox/shader.fragment");
	m_shaderprogram_skybox = compileShaderProgramDefault(vertex, fragment);
	glUseProgram(m_shaderprogram_skybox);
	glGenBuffers(1, &m_vbo_skybox);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);

	glGenVertexArrays(1, &m_vao_skybox);
	glBindVertexArray(m_vao_skybox);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NULL, NULL);//X,Y,Z

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_skybox_texture = new Texture(GL_TEXTURE_CUBE_MAP, "Content/Images/skybox/");
	m_skybox_texture->Load();
	//

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

	m_loaded_meshes.insert({ "fulltank",new Mesh("Content/Models/Tank/fulltank.obj") });
	m_loaded_meshes.insert({ "tank_bottom",new Mesh("Content/Models/Tank/tank_bottom.obj") });
	m_loaded_meshes.insert({ "tank_top",new Mesh("Content/Models/Tank/tank_top.obj") });

	m_loaded_meshes.insert({ "v_platform_full",new Mesh("Content/Models/VehiclePlatform/vehicle_platform_full.obj") });
	m_loaded_meshes.insert({ "v_platform_left",new Mesh("Content/Models/VehiclePlatform/vehicle_platform_left.obj") });
	m_loaded_meshes.insert({ "v_platform_right",new Mesh("Content/Models/VehiclePlatform/vehicle_platform_right.obj") });
	m_loaded_meshes.insert({ "v_platform_frame",new Mesh("Content/Models/VehiclePlatform/vehicle_platform_frame.obj") });

	m_loaded_meshes.insert({ "testcube",new Mesh("Content/Models/TestCube/TestCube.obj") });

	m_loaded_meshes.insert({ "gasmask",new Mesh("Content/Models/Gasmask/GasmaskV2_5.obj") });

	m_loaded_meshes.insert({ "BasicCube",new Mesh("Content/Models/BasicCube/BasicCube.obj") });

	m_loaded_meshes.insert({ "Door",new Mesh("Content/Models/Door/Door.obj") });
	m_loaded_meshes.insert({ "DoorFrame",new Mesh("Content/Models/Door/DoorFrame.obj") });

	m_loaded_meshes.insert({ "PortalFrame",new Mesh("Content/Models/Portals/Frame.obj") });
	m_loaded_meshes.insert({ "PortalF",new Mesh("Content/Models/Portals/PortalF.obj") });
	m_loaded_meshes.insert({ "PortalB",new Mesh("Content/Models/Portals/PortalB.obj") });
	m_loaded_meshes.insert({ "Room1",new Mesh("Content/Models/Portals/Room1.obj") });

	//CUBE
	{

		const std::vector<Vertex> cube_vertices = {
			// positions          // texture coords          // normals				//tangents?	//bitangents?
			Vertex({ 0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }) ,
			Vertex({ -0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f },{ 0.0f,  0.0f, -1.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f },{ 0.0f,  0.0f, -1.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f },{ 0.0f,  0.0f, -1.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),

			Vertex({ -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ -1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ -1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f,  0.5f,  0.5f },{ 1.0f, 1.0f } ,{ 0.0f,  0.0f, 1.0f },{ -1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f,  0.5f,  0.5f },{ 1.0f, 1.0f },{ 0.0f,  0.0f, 1.0f },{ -1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f,  0.5f,  0.5f },{ 0.0f, 1.0f },{ 0.0f,  0.0f, 1.0f },{ -1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ -1.0f,  0.0f, 0.0f },{ 0.0f,  1.0f, 0.0f }),

			Vertex({ -0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f },{ -1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f },{ -1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f },{ -1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ -0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f },{ 0.0f,  1.0f, 0.0f }),

			Vertex({ 0.5f,  0.5f,  -0.5f },{ 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f,  0.5f, 0.5f },{ 1.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f, -0.5f, 0.5f },{ 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f, -0.5f, 0.5f },{ 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f, -0.5f,  -0.5f },{ 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ 0.0f,  1.0f, 0.0f }),
			Vertex({ 0.5f,  0.5f,  -0.5f },{ 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f },{ 0.0f,  1.0f, 0.0f }),

			Vertex({ -0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f },{ 0.0f, -1.0f,  0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f }),
			Vertex({ 0.5f, -0.5f, -0.5f },{ 1.0f, 1.0f },{ 0.0f, -1.0f,  0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f }),
			Vertex({ 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f },{ 0.0f, -1.0f,  0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f }),
			Vertex({ 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f },{ 0.0f, -1.0f,  0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f }),
			Vertex({ -0.5f, -0.5f,  0.5f } ,{ 0.0f, 0.0f },{ 0.0f, -1.0f,  0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f }),
			Vertex({ -0.5f, -0.5f, -0.5f } ,{ 0.0f, 1.0f },{ 0.0f, -1.0f,  0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, 1.0f }),

			Vertex({ 0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f },{ 0.0f,  1.0f,  0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f }),
			Vertex({ -0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f }),
			Vertex({ -0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f }),
			Vertex({ -0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f }),
			Vertex({ 0.5f,  0.5f,  0.5f },{ 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f }),
			Vertex({ 0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f,  0.0f, 0.0f },{ 0.0f,  0.0f, -1.0f })
		};
		const std::vector<GLuint> cube_indices
		{
			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36
		};
		Mesh* Cube = new Mesh("Content/Models/TestCube/TestCube.obj");
		//Cube->m_Entries[0].Init(cube_vertices, cube_indices);
		Cube->m_Textures[1] = new Texture(GL_TEXTURE_2D, "Content/Images/box_d.png");
		bool success = Cube->m_Textures[1]->Load();
		assert(success);
		Cube->m_Textures_Specular[1] = new Texture(GL_TEXTURE_2D, "Content/Images/box_s.png");
		success = Cube->m_Textures_Specular[1]->Load();
		assert(success);
		Cube->m_Textures_Normal[1] = new Texture(GL_TEXTURE_2D, "Content/Images/box_n.png");
		success = Cube->m_Textures_Normal[1]->Load();
		assert(success);

		m_loaded_meshes.insert({ "Cube",Cube });
	}

	//SKYBOX
	{
		const std::vector<Vertex> skybox_vertices = std::vector<Vertex>
		{
			Vertex({ -0.5f,-0.5f,-0.5f },{ 0.25f,0.0f },{ -0.5f,-0.5f,-0.5f }),//0
			Vertex({ 0.5f,-0.5f,-0.5f },{ 0.5f,0.0f },{ 0.5f,-0.5f,-0.5f }),//1
			Vertex({ 0.5f,-0.5f,0.5f },{ 0.5f,1.0f / 3 },{ 0.5f,-0.5f,0.5f }),//2
			Vertex({ -0.5f,-0.5f,0.5f },{ 0.25f,1.0f / 3 },{ -0.5f,-0.5f,0.5f }),//3

			Vertex({ -0.5f,0.5f,-0.5f },{ 0.25f,1.0f },{ -0.5f,0.5f,-0.5f }),//4
			Vertex({ 0.5f,0.5f,-0.5f },{ 0.5f,1.0f },{ 0.5f,0.5f,-0.5f }),//5
			Vertex({ 0.5f,0.5f,0.5f },{ 0.5f,2.0f / 3 },{ 0.5f,0.5f,0.5f }),//6
			Vertex({ -0.5f,0.5f,0.5f },{ 0.25f,2.0f / 3 },{ -0.5f,0.5f,0.5f }),//7

			Vertex({ -0.5f,-0.5f,-0.5f },{ 0.0f,1.0f / 3 },{ -0.5f,-0.5f,-0.5f }),//8
			Vertex({ 0.5f,-0.5f,-0.5f },{ 0.75f,1.0f / 3 },{ 0.5f,-0.5f,-0.5f }),//9

			Vertex({ -0.5f,0.5f,-0.5f },{ 0.0f,2.0f / 3 },{ -0.5f,0.5f,-0.5f }),//10
			Vertex({ 0.5f,0.5f,-0.5f },{ 0.75f,2.0f / 3 },{ 0.5f,0.5f,-0.5f }),//11

			Vertex({ -0.5f,-0.5f,-0.5f },{ 1.0f,1.0f / 3 },{ -0.5f,-0.5f,-0.5f }),//12
			Vertex({ -0.5f,0.5f,-0.5f },{ 1.0f,2.0f / 3 },{ -0.5f,0.5f,-0.5f })//13
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
		//TODO: Make skybox with cube map
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

	m_default_material = Material();
	m_default_material.shininess = 32.0f;
	//m_texture_new = new Texture(GL_TEXTURE_2D, "test.png");
	//m_texture_new->Load();
	/*GLchar* test = LoadShader("Content/Shaders/MVP_Textured/shader.vertex");
	printf("%s\n", test);
	printf("\n");*/
}

void BatchRenderer::Render()
{
	RenderSkybox();
	//printf("Lines: %d\n", (int)m_lines.size());
	//printf("Linestrips: %d\n", (int)m_linestrips.size());

	m_mvp = m_projection * m_view;
	m_mvp_new = m_projection * m_view*m_model;

	/*_RenderTriangles();
	_RenderTriangleFans();
	_RenderLines();
	_RenderLineStrips();*/

	Material material;
	material.ambient = { 1.0f,1.0f,1.0f };
	material.diffuse = { 0.5f,0.5f,0.5f };
	material.specular = { 1.0f,1.0f,1.0f };
	material.shininess = 32.0f;

	//material.shininess = rand() % 100 / 100.0f;

	glUseProgram(m_shaderprogram_mvp_textured);
	//glBindVertexArray(m_vao_new);
	glUniformMatrix4fv(12, 1, GL_FALSE, &m_view[0][0]);
	SetDirectionalLight(0, m_directional_light);
	for (uint i = 0; i<m_point_light.size(); i++)
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
	SetInt(28, 2);//Set normal texture to GL_TEXTURE2
	SetInt(29, 3);//Set skybox cubemap to GL_TEXTURE3
	m_skybox_texture->Bind(GL_TEXTURE3);

	SetBlinn(Blinn);

	GLuint err = glGetError();
	if (GLEW_OK != err)
	{
		printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
	}

	GLint pos = GetAttributeLocation("vertex_position");
	glEnableVertexAttribArray(pos);

	GLint tex = GetAttributeLocation("texture_coordinate");
	glEnableVertexAttribArray(tex);

	GLint nor = GetAttributeLocation("vertex_normal");
	glEnableVertexAttribArray(nor);

	GLint tan = GetAttributeLocation("vertex_tangent");
	GLint test3 = GetAttributeLocation("vertex_bitangent");
	glEnableVertexAttribArray(tan);

	GLint bit = GetAttributeLocation("vertex_bitangent");
	glEnableVertexAttribArray(bit);

	/*for each (auto mo in m_meshes)
	{
		GLuint err = glGetError();
		if (GLEW_OK != err)
		{
			printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
		}
		m_mvp_new = m_projection * m_view *std::get<1>(mo);
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
		glUniformMatrix4fv(11, 1, GL_FALSE, &std::get<1>(mo)[0][0]);
		if (m_loaded_meshes[std::get<0>(mo)])
		{
			if (std::get<0>(mo) == "Skybox")
			{
				glUseProgram(m_shaderprogram_mvp_textured_unlit);
				glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
				SetMaterial(std::get<2>(mo));
			}
			else if (std::get<0>(mo) == "DoorFrame")
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
	}*/


	_RenderMeshes(Camera.RenderLayer);
	_RenderPortals(Camera.RenderLayer);



	//_RenderTriangles();



	//Portal1
	/*{
		//Stencil
		glEnable(GL_STENCIL_TEST);

		// Draw window
		glStencilFunc(GL_ALWAYS, 2, 0xFF); // Set any stencil to 2
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); // Write to stencil buffer
		glDepthMask(GL_FALSE); // Don't write to depth buffer
		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

		m_model = glm::translate(glm::vec3(0, 1, -100));
		m_mvp_new = m_projection * m_view * m_model;
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
		glUniformMatrix4fv(11, 1, GL_FALSE, &m_model[0][0]);

		glUseProgram(m_shaderprogram_mvp_textured);
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		m_loaded_meshes["Door"]->Render();
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT); //Clear depth buffer to avoid flickering

		//Draw objects inside window
		glStencilFunc(GL_EQUAL, 2, 0xFF); // Pass test if stencil value is 2
		glStencilMask(0x00); // Don't write anything to stencil buffer
		glDepthMask(GL_TRUE); // Write to depth buffer

		glm::mat4 portal_view = glm::translate(m_view, glm::vec3(0,100,0));
		glUniformMatrix4fv(12, 1, GL_FALSE, &portal_view[0][0]);

		for each (auto mo in m_meshes)
		{
			GLuint err = glGetError();
			if (GLEW_OK != err)
			{
				printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
			}
			m_model = std::get<1>(mo);
			m_mvp_new = m_projection * portal_view * m_model;
			glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
			glUniformMatrix4fv(11, 1, GL_FALSE, &m_model[0][0]);
			if (m_loaded_meshes[std::get<0>(mo)])
			{
				if (std::get<0>(mo) == "Skybox")
				{
					glUseProgram(m_shaderprogram_mvp_textured_unlit);
					glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
					SetMaterial(std::get<2>(mo));
				}
				else if (std::get<0>(mo) == "DoorFrame")
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
		glDisable(GL_STENCIL_TEST);
	}*/
	
	

	//Mirror
	/*{
		//Stencil
		glEnable(GL_STENCIL_TEST);

		// Draw floor
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); // Write to stencil buffer
		glDepthMask(GL_FALSE); // Don't write to depth buffer
		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

		m_model = glm::scale(glm::vec3(200, 200, 200));
		m_mvp_new = m_projection * m_view * m_model;
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
		glUniformMatrix4fv(11, 1, GL_FALSE, &m_model[0][0]);

		glUseProgram(m_shaderprogram_mvp_textured);
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);

		//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		m_loaded_meshes["flatplane"]->Render();
		//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		//glClear(GL_DEPTH_BUFFER_BIT); //Clear depth buffer to avoid flickering

		// Draw reflections
		glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
		glStencilMask(0x00); // Don't write anything to stencil buffer
		glDepthMask(GL_TRUE); // Write to depth buffer

		//Render mirrored models
		glCullFace(GL_FRONT); // culls front faces
		//m_projection = glm::scale(m_projection, glm::vec3(1, -1, 1));
		for each (auto mo in m_meshes)
		{
			GLuint err = glGetError();
			if (GLEW_OK != err)
			{
				printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
			}
			m_model = std::get<1>(mo);// glm::scale(glm::translate(std::get<1>(mo), glm::vec3(0, 0, 0)), glm::vec3(-10, -10, -10));
			glm::mat4 reflection =
			{
				1,0,0,0,
				0,-1,0,0,
				0,0,1,0,
				0,0,0,1
			};
			m_model = reflection * m_model;
			//m_model = glm::scale(std::get<1>(mo), glm::vec3(1, -1, 1));
			m_mvp_new = m_projection * m_view * m_model;
			glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
			glUniformMatrix4fv(11, 1, GL_FALSE, &m_model[0][0]);
			if (m_loaded_meshes[std::get<0>(mo)])
			{
				if (std::get<0>(mo) == "Skybox")
				{
					glUseProgram(m_shaderprogram_mvp_textured_unlit);
					glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
					SetMaterial(std::get<2>(mo));
				}
				else if (std::get<0>(mo) == "DoorFrame")
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
		glDisable(GL_STENCIL_TEST);
		glCullFace(GL_BACK); // culls back faces
	}*/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glActiveTexture(GL_TEXTURE0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(tan);
	glDisableVertexAttribArray(bit);

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
	m_portals.clear();

	m_triangles.clear();
	m_lines.clear();
	m_trianglefans.clear();
	m_linestrips.clear();

	//RenderSkybox();
}

void BatchRenderer::_ClearScreen()
{
	/*float vertices[] = {
		-1.0f,-1.0f,0.0f,1.0f,0.0f,0.0f,
		 1.0f,-1.0f,0.0f,1.0f,0.0f,0.0f,
		 1.0f, 1.0f,0.0f,1.0f,0.0f,0.0f,
					
		 1.0f, 1.0f,0.0f,1.0f,0.0f,0.0f,
		-1.0f, 1.0f,0.0f,1.0f,0.0f,0.0f,
		-1.0f,-1.0f,0.0f,1.0f,0.0f,0.0f,
	}; */
	float vertices[] = {
		-1.0f,-1.0f,1.0f,1.0f,0.0f,0.0f,
		 1.0f,-1.0f,1.0f,1.0f,0.0f,0.0f,
		 1.0f, 1.0f,1.0f,1.0f,0.0f,0.0f,
					
		 1.0f, 1.0f,1.0f,1.0f,0.0f,0.0f,
		-1.0f, 1.0f,1.0f,1.0f,0.0f,0.0f,
		-1.0f,-1.0f,1.0f,1.0f,0.0f,0.0f,
	};

	_RenderTriangle(vertices, 6); 	

	Material material;
	material.ambient = { 1.0f,1.0f,1.0f };
	material.diffuse = { 0.5f,0.5f,0.5f };
	material.specular = { 1.0f,1.0f,1.0f };
	material.shininess = 32.0f;
	glUseProgram(m_shaderprogram_mvp_textured);
	//glBindVertexArray(m_vao_new);
	glUniformMatrix4fv(12, 1, GL_FALSE, &m_view[0][0]);
	SetDirectionalLight(0, m_directional_light);
	for (uint i = 0; i<m_point_light.size(); i++)
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
	SetInt(28, 2);//Set normal texture to GL_TEXTURE2
	SetInt(29, 3);//Set skybox cubemap to GL_TEXTURE3
	m_skybox_texture->Bind(GL_TEXTURE3);

	SetBlinn(Blinn);

	GLuint err = glGetError();
	if (GLEW_OK != err)
	{
		printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
	}

	GLint pos = GetAttributeLocation("vertex_position");
	glEnableVertexAttribArray(pos);

	GLint tex = GetAttributeLocation("texture_coordinate");
	glEnableVertexAttribArray(tex);

	GLint nor = GetAttributeLocation("vertex_normal");
	glEnableVertexAttribArray(nor);

	GLint tan = GetAttributeLocation("vertex_tangent");
	GLint test3 = GetAttributeLocation("vertex_bitangent");
	glEnableVertexAttribArray(tan);

	GLint bit = GetAttributeLocation("vertex_bitangent");
	glEnableVertexAttribArray(bit);
}

void BatchRenderer::_RenderTriangle(float* triangle, int numvertices = 3)
{
	float* vertices;
	vertices = &triangle[0];

	glUseProgram(m_shaderprogram);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, 6 * numvertices * sizeof(float), vertices, GL_STATIC_DRAW);

	glm::mat4 imat = glm::mat4(1);
	glUniformMatrix4fv(2, 1, GL_FALSE, &imat[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, numvertices);
	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BatchRenderer::_RenderTriangles()
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

void BatchRenderer::_RenderLines()
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

void BatchRenderer::_RenderTriangleFans()
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

void BatchRenderer::_RenderLineStrips()
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

void BatchRenderer::_RenderMeshes(int render_layer)
{

	//planeEqn[0] = rand() % 100 / 100.0f;
	/*planeEqn[1] = rand() % 100 / 100.0f;
	planeEqn[2] = rand() % 100 / 100.0f;
	planeEqn[3] = rand() % 100 / 100.0f;*/

	/*glClipPlane(GL_CLIP_PLANE0, planeEqn);  // Define the plane equation
	glEnable(GL_CLIP_PLANE0);    // Enable clipping
	glDisable(GL_CLIP_PLANE0);    // Disable clipping*/


	for each (auto mo in m_meshes)
	{
		if (std::get<3>(mo).count(render_layer) == 0)
		{
			continue;
		}
		GLuint err = glGetError();
		if (GLEW_OK != err)
		{
			printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
		}
		m_mvp_new = m_projection * m_view *std::get<1>(mo);
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
		glUniformMatrix4fv(11, 1, GL_FALSE, &std::get<1>(mo)[0][0]);
		if (m_loaded_meshes[std::get<0>(mo)])
		{
			if (std::get<0>(mo) == "Skybox")
			{
				glUseProgram(m_shaderprogram_mvp_textured_unlit);
				glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
				SetMaterial(std::get<2>(mo));
			}
			/*else if (std::get<0>(mo) == "DoorFrame")
			{
				glUseProgram(m_shaderprogram_mvp_textured_unlit);
				glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
			}*/
			else
			{
				glUseProgram(m_shaderprogram_mvp_textured);
				glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
				SetMaterial(std::get<2>(mo));
			}
			m_loaded_meshes[std::get<0>(mo)]->Render();
		}
	}
}

void BatchRenderer::_RenderPortals(int render_layer)
{
	for each(auto po in m_portals)
	{
		if (std::get<3>(po).count(render_layer) == 0)
		{
			continue;
		}

		/*float distance = glm::dot(Camera.Forward, glm::vec3(std::get<1>(po)[3]) - Camera.Position );
		if (distance < 0.0f)
		{
			continue;
		}*/
		glm::quat rotation;
		glm::decompose(m_view, glm::vec3(), rotation, glm::vec3(), glm::vec3(), glm::vec4());
		glm::vec3 forward = glm::vec3(0, 0, -1)*rotation;

		float distance = glm::dot(forward, glm::vec3(std::get<1>(po)[3]) - Camera.Position);
		if (distance < 0.0f)
		{
			continue;
		}
		//Stencil
		glEnable(GL_STENCIL_TEST);

		// Draw window
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF); // Write to stencil buffer
		glDepthMask(GL_FALSE); // Don't write to depth buffer
		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)*/

		m_model = std::get<1>(po);
		m_mvp_new = m_projection * m_view * m_model;
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
		glUniformMatrix4fv(11, 1, GL_FALSE, &m_model[0][0]);

		glUseProgram(m_shaderprogram_mvp_textured);
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		m_loaded_meshes[std::get<0>(po)]->Render();
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glClear(GL_DEPTH_BUFFER_BIT); //Clear depth buffer to avoid flickering

		//Draw objects inside window
		glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
		glStencilMask(0x00); // Don't write anything to stencil buffer
		glDepthMask(GL_TRUE); // Write to depth buffer


		/*GLdouble planeEqn[4] = { 0.0, 0.0, 1.0, 0.0 };

		glEnable(GL_CLIP_PLANE0);    // Enable clipping
		glClipPlane(GL_CLIP_PLANE0, planeEqn);  // Define the plane equation
		glDisable(GL_CLIP_PLANE0);    // Disable clipping*/

		glDepthFunc(GL_ALWAYS);
		//glDisable(GL_DEPTH_TEST); 
		//glDepthMask(GL_FALSE);
		_ClearScreen();
		//glDepthMask(GL_TRUE);
		//glEnable(GL_DEPTH_TEST)
		glDepthFunc(GL_LEQUAL);


		glm::vec4 planeEqn;
		if (std::get<5>(po))
		{
			planeEqn = { 0.0f, 0.0f, 1.0f, 0.0f };
		}
		else
		{
			planeEqn = { 0.0f, 0.0f, -1.0f, 0.0f };
		}
		//glm::mat4 transform = glm::translate(glm::vec3(0, 0, 0));
		//planeEqn = glm::transpose(glm::inverse(transform))*planeEqn;

		glm::mat4 mat = std::get<2>(po);
		planeEqn = glm::transpose(glm::inverse(mat))*planeEqn; //TODO: Figure out where to move the clipping plane
		//planeEqn = glm::transpose(glm::inverse( std::get<2>(po) ))*planeEqn;

		glUniform4fv(GetUniformLocation("clip_plane"), 1, &planeEqn[0]);
		glEnable(GL_CLIP_DISTANCE0);

		glm::mat4 portal_view = m_view * std::get<1>(po) / std::get<2>(po);
		glUniformMatrix4fv(12, 1, GL_FALSE, &portal_view[0][0]);
		SetVec3(GetUniformLocation("gPortalOffset"), (std::get<1>(po) / std::get<2>(po))[3]);
		int portal_it = 0;
		for each (auto mo in m_meshes)
		{
			portal_it++;
			if (std::get<3>(mo).count(std::get<4>(po)) == 0)
			{
				continue;
			}
			GLuint err = glGetError();
			if (GLEW_OK != err)
			{
				printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
			}
			m_model = std::get<1>(mo);
			m_mvp_new = m_projection * portal_view * m_model;
			
			glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
			glUniformMatrix4fv(11, 1, GL_FALSE, &m_model[0][0]);
			if (m_loaded_meshes[std::get<0>(mo)])
			{
				if (std::get<0>(mo) == "Skybox")
				{
					glUseProgram(m_shaderprogram_mvp_textured_unlit);
					glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
					SetMaterial(std::get<2>(mo));
				}
				else if (std::get<0>(mo) == "DoorFrame")
				{
					//SetVec3(GetUniformLocation("gPortalOffset"), glm::vec3(0, 0, 0));
					//glUseProgram(m_shaderprogram_mvp_textured_unlit);
					glUseProgram(m_shaderprogram_mvp_textured);
					glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
				}
				else
				{
					//SetVec3(GetUniformLocation("gPortalOffset"), (std::get<1>(po) / std::get<2>(po))[3]);
					glUseProgram(m_shaderprogram_mvp_textured);
					glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
					SetMaterial(std::get<2>(mo));
				}
				m_loaded_meshes[std::get<0>(mo)]->Render();
			}
		}
		_RenderPortalsInPortals(std::get<4>(po), 1, 3, portal_view, portal_it, std::get<1>(po), std::get<2>(po));//TODO: Skip current portal

		glDisable(GL_CLIP_DISTANCE0);
		glDisable(GL_STENCIL_TEST);
		SetVec3(GetUniformLocation("gPortalOffset"), glm::vec3(0,0,0));
	}

}

void BatchRenderer::_RenderPortalsInPortals(int render_layer, int stencil_depth, int portal_depth, glm::mat4 view, int iterator, glm::mat4 portal_start, glm::mat4 portal_end)
{
	if (portal_depth <= 0)
	{
		return;
	}
	int portal_it = 0;
	for each(auto po in m_portals)
	{
		portal_it++;
		if (iterator == portal_it)//TODO: Also skip portal that is in the same portal entity as the exit portal
		{
			continue;
		}
		if (std::get<3>(po).count(render_layer) == 0)
		{
			continue;
		}

		glm::quat rotation;
		glm::decompose(view, glm::vec3(), rotation, glm::vec3(), glm::vec3(), glm::vec4());
		glm::vec3 forward = glm::vec3(0,0,-1)*rotation;

		float distance = glm::dot(forward, glm::vec3(std::get<1>(po)[3]) - glm::vec3(portal_end[3]));
		if (distance < 0.0f)
		{
			continue;
		}
		// Draw window
		glStencilFunc(GL_GEQUAL, stencil_depth, 0xFF); // Pass test if stencil value is 1 or higher
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);//Increment stencil by 1
		glStencilMask(0xFF); // Write to stencil buffer
		glDepthMask(GL_FALSE); // Don't write to depth buffer
		
		m_model = std::get<1>(po);
		m_mvp_new = m_projection * view * m_model;
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
		glUniformMatrix4fv(11, 1, GL_FALSE, &m_model[0][0]);

		glUseProgram(m_shaderprogram_mvp_textured);
		glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		m_loaded_meshes[std::get<0>(po)]->Render();
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glClear(GL_DEPTH_BUFFER_BIT); //Clear depth buffer to avoid flickering

		//Draw objects inside window
		glStencilFunc(GL_EQUAL, stencil_depth+1, 0xFF); // Pass test if stencil value is 1
		glStencilMask(0x00); // Don't write anything to stencil buffer
		glDepthMask(GL_TRUE); // Write to depth buffer

		/*GLdouble planeEqn[4] = { 0.0, 0.0, 1.0, 0.0 };

		glEnable(GL_CLIP_PLANE0);    // Enable clipping
		glClipPlane(GL_CLIP_PLANE0, planeEqn);  // Define the plane equation
		glDisable(GL_CLIP_PLANE0);    // Disable clipping*/

		glDepthFunc(GL_ALWAYS);
		//glDisable(GL_DEPTH_TEST); 
		//glDepthMask(GL_FALSE);
		_ClearScreen();
		//glDepthMask(GL_TRUE);
		//glEnable(GL_DEPTH_TEST)
		glDepthFunc(GL_LEQUAL);

		glm::vec4 planeEqn;
		if (std::get<5>(po))
		{
			planeEqn = { 0.0f, 0.0f, 1.0f, 0.0f };
		}
		else
		{
			planeEqn = { 0.0f, 0.0f, -1.0f, 0.0f };
		}
		//glm::mat4 transform = glm::translate(glm::vec3(0, 0, 0));
		//planeEqn = glm::transpose(glm::inverse(transform))*planeEqn;
		glm::mat4 clip_plane_mat;
		/*glm::vec3 off;
		glm::quat rot;
		glm::decompose(portal_offset, glm::vec3(), rot, off, glm::vec3(), glm::vec4());
		mat = glm::mat4(glm::inverse(rot)) * std::get<2>(po);
		mat = glm::translate(-off) * mat;*/
		clip_plane_mat =  std::get<2>(po);
		planeEqn = glm::transpose(glm::inverse(clip_plane_mat))*planeEqn; //TODO: Figure out where to move the clipping plane

		glUniform4fv(GetUniformLocation("clip_plane"), 1, &planeEqn[0]);
		glEnable(GL_CLIP_DISTANCE0);//TODO:Fix
		//glDisable(GL_CLIP_DISTANCE0);//TODO:Remove
		glm::mat4 portal_view = view * (std::get<1>(po) / std::get<2>(po));
		glUniformMatrix4fv(12, 1, GL_FALSE, &portal_view[0][0]);
		SetVec3(GetUniformLocation("gPortalOffset"), ((portal_start/portal_end) * (std::get<1>(po) / std::get<2>(po)))[3]);
		for each (auto mo in m_meshes)
		{
			if (std::get<3>(mo).count(std::get<4>(po)) == 0)
			{
				continue;
			}
			GLuint err = glGetError();
			if (GLEW_OK != err)
			{
				printf("i:%d 0x%X %s\n", err, err, glewGetErrorString(err));
			}
			m_model = std::get<1>(mo);
			m_mvp_new = m_projection * portal_view * m_model;

			glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
			glUniformMatrix4fv(11, 1, GL_FALSE, &m_model[0][0]);
			if (m_loaded_meshes[std::get<0>(mo)])
			{
				if (std::get<0>(mo) == "Skybox")
				{
					glUseProgram(m_shaderprogram_mvp_textured_unlit);
					glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
					SetMaterial(std::get<2>(mo));
				}
				else if (std::get<0>(mo) == "DoorFrame")
				{
					//SetVec3(GetUniformLocation("gPortalOffset"), glm::vec3(0, 0, 0));
					//glUseProgram(m_shaderprogram_mvp_textured_unlit);
					glUseProgram(m_shaderprogram_mvp_textured);
					glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
				}
				else
				{
					//SetVec3(GetUniformLocation("gPortalOffset"), (std::get<1>(po) / std::get<2>(po))[3]);
					glUseProgram(m_shaderprogram_mvp_textured);
					glUniformMatrix4fv(3, 1, GL_FALSE, &m_mvp_new[0][0]);
					SetMaterial(std::get<2>(mo));
				}
				m_loaded_meshes[std::get<0>(mo)]->Render();
			}
		}

		_RenderPortalsInPortals(std::get<4>(po), stencil_depth+1, portal_depth-1, portal_view, portal_it,  std::get<1>(po), std::get<2>(po) );

		glStencilFunc(GL_GEQUAL, stencil_depth-1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // 'Clear' stencil buffer
		glStencilMask(0xFF); // Write to stencil buffer
		//glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		_ClearScreen();
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

	}

}

