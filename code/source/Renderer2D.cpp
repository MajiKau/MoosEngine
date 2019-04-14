#include "code/headers/Renderer2D.h"

Renderer2D::Renderer2D(float Zoom, float Ratio)
{
	GLenum err = glGetError();
	if (GLEW_OK != err)
	{
		printf("%s\n", glewGetErrorString(err));
	}
	//glEnable(GL_ALPHA_TEST); // enable alpha-testing (maybe deprecated?)

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
							 //glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_STENCIL_TEST); // Enable stencil testing

	glEnable(GL_CULL_FACE); // enable face culling
							//glDisable(GL_CULL_FACE);
							//glCullFace(GL_FRONT); // culls front faces
	glCullFace(GL_BACK); // culls back faces

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFFFFFF);

	GLchar* vertex = LoadShader("Content/Shaders/Textured2D/shader.vertex");
	GLchar* fragment = LoadShader("Content/Shaders/Textured2D/shader.fragment");
	m_shaderprogram = CompileShaderProgram(vertex, fragment);

	glUseProgram(m_shaderprogram);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);


	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	
	err = glGetError();
	if (GLEW_OK != err)
	{
		printf("%s\n", glewGetErrorString(err));
	}

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2); 

	err = glGetError();
	if (GLEW_OK != err)
	{
		printf("%s\n", glewGetErrorString(err));
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), NULL);//Vertex: 3 floats
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));//Texcoord: 2 floats, offset is 3
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 5));//Color: 4 floats, offset is 5

	err = glGetError();
	if (GLEW_OK != err)
	{
		printf("%s\n", glewGetErrorString(err));
	}

	SetInt("texture", 0);

	err = glGetError();
	if (GLEW_OK != err)
	{
		printf("%s\n", glewGetErrorString(err));
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	err = glGetError();
	if (GLEW_OK != err)
	{
		printf("%s\n", glewGetErrorString(err));
	}

	LoadTexture("Content/Images/white.png", "white");
	LoadTexture("Content/Images/Sprites/test.png", "test");
}

void Renderer2D::Render()
{
	/*GLfloat vertices[] =
	{
		0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,1.0f,
		1.0f,0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,1.0f,1.0f,
		0.0f,1.0f,0.0f,0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,
		1.0f,1.0f,0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f


	};

	int numvertices;
	numvertices = 4;

	glUseProgram(m_shaderprogram);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);


	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glUniformMatrix4fv(GetUniformLocation("projection"), 1, GL_FALSE, &m_projection[0][0]);

	m_loaded_textures.at("dvd-logo")->Bind(GL_TEXTURE0);
	glm::vec2 size = 8*m_loaded_textures.at("test")->GetSize(); 
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(size.x,size.y,1.0f));
	glUniformMatrix4fv(GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, numvertices);
	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	//TODO

	_RenderSprites();
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

void Renderer2D::LoadTexture(const char * filename, const char * texturename)
{
	for (auto& texture_pair : m_loaded_textures)
	{
		if (texture_pair.first == texturename)
		{
			printf("LoadTexture: Texture with the same name already exists (%s)\n", texturename);
			return;
		}
	}

	Texture2D* texture = new Texture2D(GL_TEXTURE_2D, filename);
	texture->Load();

	m_loaded_textures.push_back({ texturename, texture });
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

void Renderer2D::RenderSprite(std::string texture_name, glm::vec3 position, glm::vec2 scale, glm::ivec2 sprite_offset, glm::ivec2 sprite_size, glm::vec4 color)
{
	Texture2D* texture = NULL;
	for (auto& texture_pair : m_loaded_textures)
	{
		if (texture_pair.first == texture_name)
		{
			texture = texture_pair.second;
			break;
		}
	}
	if (!texture)
	{
		printf("RenderSprite: Texture is missing (%s)\n", texture_name);
		return;
	}
	glm::vec2 texture_size = texture->GetSize();
	if (sprite_size.x == 0)
	{
		sprite_size = texture_size;
	}

	GLfloat vertices[4*9] =
	{
		position.x								, position.y							, position.z, sprite_offset.x / texture_size.x					, sprite_offset.y / texture_size.y					, color.r, color.g, color.b, color.a,
		position.x + scale.x * sprite_size.x	, position.y							, position.z, (sprite_offset.x + sprite_size.x) / texture_size.x, sprite_offset.y / texture_size.y					, color.r, color.g, color.b, color.a,
		position.x								, position.y + scale.y * sprite_size.y	, position.z, sprite_offset.x / texture_size.x					, (sprite_offset.y + sprite_size.y) / texture_size.y, color.r, color.g, color.b, color.a,
		position.x + scale.x * sprite_size.x	, position.y + scale.y * sprite_size.y	, position.z, (sprite_offset.x + sprite_size.x) / texture_size.x, (sprite_offset.y + sprite_size.y) / texture_size.y, color.r, color.g, color.b, color.a
	};

	

	std::pair<std::vector<GLfloat>, std::vector<GLuint>>* pair = NULL;
	for (auto& sprite : m_sprites)
	{
		if (sprite.first == texture)
		{
			pair = &sprite.second;
			break;
		}
	}
	if (!pair)
	{
		m_sprites.push_back({ texture,{{},{}} });
		pair = &m_sprites.back().second;
	}

	GLuint indices_size = pair->second.size() / 5 * 4;
	GLuint indices[5] =
	{
		indices_size, indices_size + 1, indices_size + 2, indices_size + 3, 0xFFFFFFFF
	};

	pair->first.insert(pair->first.end(), vertices, vertices+4*9);
	pair->second.insert(pair->second.end(), indices, indices+5);
}

void Renderer2D::_RenderSprites()
{
	glUseProgram(m_shaderprogram);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glUniformMatrix4fv(GetUniformLocation("projection"), 1, GL_FALSE, &m_projection[0][0]);

	for (auto pair : m_sprites)
	{
		pair.first->Bind(GL_TEXTURE0);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pair.second.second.size() * sizeof(unsigned int), &pair.second.second[0], GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, pair.second.first.size()*sizeof(GLfloat), &(pair.second.first[0]), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLE_STRIP, pair.second.second.size(), GL_UNSIGNED_INT, (void*)0);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, pair.second.first.size() / 9);
	}

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_sprites.clear();
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

glm::ivec2 Texture2D::GetSize()
{
	return m_size;
}
