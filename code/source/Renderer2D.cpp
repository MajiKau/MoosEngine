#include "code/headers/Renderer2D.h"

Renderer2D::Renderer2D(float Zoom, float Ratio)
{
	GLchar* vertex = LoadShader("Content/Shaders/Textured2D/shader.vertex");
	GLchar* fragment = LoadShader("Content/Shaders/Textured2D/shader.fragment");
	m_shaderprogram = CompileShaderProgram(vertex, fragment);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), NULL);//Vertex: 3 floats
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));//Texcoord: 2 floats, offset is 3
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 5));//Color: 4 floats, offset is 5

	SetInt("texture", 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	LoadTexture("Content/Images/white.png", "white");
	LoadTexture("Content/Images/Sprites/test.png", "test");
}

void Renderer2D::Render()
{
	GLfloat vertices[] =
	{
		0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,
		1.0f,0.0f,0.0f,1.0f,0.0f,1.0f,0.0f,1.0f,1.0f,
		1.0f,1.0f,0.0f,1.0f,1.0f,1.0f,1.0f,0.0f,1.0f,
		0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.5f,
		0.0f,1.0f,0.0f,0.0f,1.0f,1.0f,1.0f,1.0f,0.5f
	};

	/*float vertices[] =
	{
		0,0,0,
		1,0,0,
		1,1,0,
		0,0,0,
		1,1,0,
		0,1,0
	};*/

	int numvertices;
	numvertices = 6;
	if (numvertices < 3)return;

	glUseProgram(m_shaderprogram);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);


	glBufferData(GL_ARRAY_BUFFER, 9 * numvertices * sizeof(float), vertices, GL_STATIC_DRAW);
	
	glUniformMatrix4fv(GetUniformLocation("projection"), 1, GL_FALSE, &m_projection[0][0]);

	m_loaded_textures.at("test")->Bind(GL_TEXTURE0);
	glm::vec2 size = 8.0f*m_loaded_textures.at("test")->GetSize(); 
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(size.x,size.y,1.0f));
	glUniformMatrix4fv(GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, numvertices);
	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//TODO
}

GLchar * Renderer2D::LoadShader(const char * filename)
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
}

GLuint Renderer2D::LoadTexture(const char * filename, const char * texturename)
{
	Texture2D* texture = new Texture2D(GL_TEXTURE_2D, filename);
	texture->Load();

	m_loaded_textures.insert({ texturename, texture });
	return GLuint();
}

GLint Renderer2D::CompileShaderProgram(const char * vSharerSrc, const char * fSharerSrc)
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

GLint Renderer2D::GetUniformLocation(GLchar * uniform_name)
{
	return glGetUniformLocation(m_shaderprogram, uniform_name);
}

void Renderer2D::SetInt(GLchar* uniform_name, int value)
{
	glUniform1i(GetUniformLocation(uniform_name), value);
}

Texture2D::Texture2D(GLenum TextureTarget, const std::string& FileName)
{
	m_textureTarget = TextureTarget;
	m_fileName = FileName;
}

bool Texture2D::Load()
{
	int width, height;

	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(m_fileName.c_str(), 0);
	assert(formato != FIF_UNKNOWN);
	FIBITMAP* imagen = FreeImage_Load(formato, m_fileName.c_str());
	if (!imagen)
	{
		printf("Failed to load %s\n", m_fileName.c_str());
		return false;
	}
	assert(imagen != NULL);
	FIBITMAP* temp = FreeImage_ConvertTo32Bits(imagen);
	assert(imagen != NULL);

	width = FreeImage_GetWidth(imagen);
	height = FreeImage_GetHeight(imagen);
	m_size.x = width;
	m_size.y = height;

	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);
	glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(temp));
	//glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(m_textureTarget, 0);

	FreeImage_Unload(imagen);
	FreeImage_Unload(temp);

	return true;
}

void Texture2D::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}

glm::vec2 Texture2D::GetSize()
{
	return m_size;
}
