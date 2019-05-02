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
	//glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_GREATER);
	//glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_STENCIL_TEST); // Enable stencil testing

	glEnable(GL_CULL_FACE); // enable face culling
	glCullFace(GL_BACK); // culls back faces
	//glCullFace(GL_FRONT); // culls front faces
	//glDisable(GL_CULL_FACE);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFFFFFF);

	//Shaders

	//Default
	{
		m_shader_textured = Shader("Content/Shaders/Textured2D/shader.vertex", "Content/Shaders/Textured2D/shader.fragment");

		glUseProgram(m_shader_textured.m_shaderprogram);

		glGenVertexArrays(1, &m_shader_textured.m_vao);
		glBindVertexArray(m_shader_textured.m_vao);

		glGenBuffers(1, &m_shader_textured.m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_shader_textured.m_vbo);


		glGenBuffers(1, &m_shader_textured.m_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_shader_textured.m_ebo);

		GLint vertexAttr = m_shader_textured.GetAttributeLocation("vertex_position");
		GLint texcoordAttr = m_shader_textured.GetAttributeLocation("texture_coordinate");
		glEnableVertexAttribArray(vertexAttr);
		glEnableVertexAttribArray(texcoordAttr);

		glVertexAttribPointer(vertexAttr, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);//Vertex: 3 floats
		glVertexAttribPointer(texcoordAttr, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));//Texcoord: 2 floats, offset is 3

		m_shader_textured.SetInt("texture", 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//Stencil
	
	{
		m_shader_stencil = Shader("Content/Shaders/Stencil2D/shader.vertex", "Content/Shaders/Stencil2D/shader.fragment");

		glUseProgram(m_shader_stencil.m_shaderprogram);

		glGenVertexArrays(1, &m_shader_stencil.m_vao);
		glBindVertexArray(m_shader_stencil.m_vao);

		glGenBuffers(1, &m_shader_stencil.m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_shader_stencil.m_vbo);


		glGenBuffers(1, &m_shader_stencil.m_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_shader_stencil.m_ebo);

		GLint vertexAttr = m_shader_stencil.GetAttributeLocation("vertex_position");
		glEnableVertexAttribArray(vertexAttr);

		glVertexAttribPointer(vertexAttr, 2, GL_FLOAT, GL_FALSE, NULL, NULL);//Vertex: 2 floats

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//Color

	{
		m_shader_color = Shader("Content/Shaders/Color2D/shader.vertex", "Content/Shaders/Color2D/shader.fragment");

		glUseProgram(m_shader_color.m_shaderprogram);

		glGenVertexArrays(1, &m_shader_color.m_vao);
		glBindVertexArray(m_shader_color.m_vao);

		glGenBuffers(1, &m_shader_color.m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_shader_color.m_vbo);


		glGenBuffers(1, &m_shader_color.m_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_shader_color.m_ebo);

		GLint vertexAttr = m_shader_color.GetAttributeLocation("vertex_position");
		glEnableVertexAttribArray(vertexAttr);

		glVertexAttribPointer(vertexAttr, 3, GL_FLOAT, GL_FALSE, NULL, NULL);//Vertex: 3 floats

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

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
	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	_SortSprites();
	for (auto& sprite_data : m_sprites)
	{
		Sprite* sprite = std::get<0>(sprite_data);
		_RenderSprite(sprite->GetTexture(), std::get<1>(sprite_data), std::get<2>(sprite_data), sprite->GetSpriteOffset(), sprite->GetSpriteSize(), sprite->GetColor());
	}
	m_sprites.clear();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	_RenderStencil();
	m_stencil.vertices.clear();
	m_stencil.indices.clear();

	_RenderLines();
	m_line_data.clear();

	glEnable(GL_STENCIL_TEST);
	_RenderSprites();
	m_sprite_data.clear();
	glDisable(GL_STENCIL_TEST);

	//_RenderSprites();
}

GLchar * Shader::LoadShader(const char * filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	file.seekg(0, file.end);
	int length = (int)file.tellg();
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

	m_loaded_textures[texturename] = texture ;
}

Texture2D * Renderer2D::GetTexture(const char * texturename)
{
	return m_loaded_textures.at(texturename);
}

GLint Shader::CompileShaderProgram(const char * vSharerSrc, const char * fSharerSrc)
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

Shader::Shader(const char* vertex, const char * fragment)
{
	GLchar* gl_vertex = LoadShader(vertex);
	GLchar* gl_fragment = LoadShader(fragment);

	m_shaderprogram = CompileShaderProgram(gl_vertex, gl_fragment);
}

Shader::Shader()
{
}

GLint Shader::GetUniformLocation(GLchar * uniform_name)
{
	return glGetUniformLocation(m_shaderprogram, uniform_name);
}

GLint Shader::GetAttributeLocation(GLchar * attribute_name)
{
	return glGetAttribLocation(m_shaderprogram, attribute_name);
}

void Shader::UseShader()
{
	glUseProgram(m_shaderprogram);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}

void Shader::SetInt(GLchar* uniform_name, int value)
{
	glUniform1i(GetUniformLocation(uniform_name), value);
}

void Renderer2D::_RenderSprite(Texture2D* texture, glm::vec3 position, glm::vec2 scale, glm::ivec2 sprite_offset, glm::ivec2 sprite_size, glm::vec4 color)
{
	if (!texture)
	{
		printf("RenderSprite: Texture is missing\n");
		return;
	}
	glm::vec2 texture_size = texture->GetSize();
	if (sprite_size.x == 0)
	{
		sprite_size = texture_size;
	}

	GLfloat vertices[4*5] =
	{
		position.x								, position.y							, position.z, sprite_offset.x / texture_size.x					, sprite_offset.y / texture_size.y					,
		position.x + scale.x * sprite_size.x	, position.y							, position.z, (sprite_offset.x + sprite_size.x) / texture_size.x, sprite_offset.y / texture_size.y					,
		position.x								, position.y + scale.y * sprite_size.y	, position.z, sprite_offset.x / texture_size.x					, (sprite_offset.y + sprite_size.y) / texture_size.y,
		position.x + scale.x * sprite_size.x	, position.y + scale.y * sprite_size.y	, position.z, (sprite_offset.x + sprite_size.x) / texture_size.x, (sprite_offset.y + sprite_size.y) / texture_size.y
	};

	

	VertexIndexArrays* data = NULL;
	for (auto& sprite : m_sprite_data)
	{
		if (sprite.material.texture == texture && sprite.material.color == color)
		{
			data = &sprite.data;
			break;
		}
	}
	if (!data)
	{
		m_sprite_data.push_back({ { texture, color }, { {}, {} } });
		data = &m_sprite_data.back().data;
	}

	GLuint indices_size = (GLuint)data->indices.size() / 5 * 4;
	GLuint indices[5] =
	{
		indices_size, indices_size + 1, indices_size + 2, indices_size + 3, 0xFFFFFFFF
	};

	data->vertices.insert(data->vertices.end(), vertices, vertices+4*5);
	data->indices.insert(data->indices.end(), indices, indices+5);
}

void Renderer2D::RenderSprite(Sprite * sprite, glm::vec3 position, glm::vec2 scale)
{
	m_sprites.push_back({ sprite, position, scale });
}

void Renderer2D::RenderStencil(std::vector<GLfloat> vertices)
{
	GLuint num_vertices = (GLuint)vertices.size() / 2;
	GLuint indices_size = (GLuint)m_stencil.vertices.size() / 2;

	for (GLuint i = 0; i < num_vertices; i++)
	{
		m_stencil.indices.emplace_back(indices_size + i);
	}
	m_stencil.indices.emplace_back(0xFFFFFFFF);

	m_stencil.vertices.insert(m_stencil.vertices.end(), vertices.begin(), vertices.end());
}

void Renderer2D::RenderRectangle(Rectangle2D rect, glm::vec3 color)
{
	VertexIndexArrays* data = NULL;
	for (auto& line : m_line_data)
	{
		if (line.color == color)
		{
			data = &line.data;
			break;
		}
	}
	if (!data)
	{
		m_line_data.push_back({ color,{} });
		data = &m_line_data.back().data;
	}

	GLuint num_vertices = 4;
	GLuint indices_size = (GLuint)data->vertices.size() / 3;

	for (GLuint i = 0; i < num_vertices; i++)
	{
		data->indices.emplace_back(indices_size + i);
	}
	data->indices.emplace_back(indices_size);
	data->indices.emplace_back(0xFFFFFFFF);

	GLfloat vertices[] =
	{
		rect.x,			 rect.y,		  0,
		rect.x + rect.w, rect.y,		  0,
		rect.x + rect.w, rect.y + rect.h, 0,
		rect.x,			 rect.y + rect.h, 0,
	};

	data->vertices.insert(data->vertices.end(), vertices, vertices + 3*4);

	


}

void Renderer2D::RenderLines(std::vector<GLfloat> vertices, glm::vec3 color)
{
	VertexIndexArrays* data = NULL;
	for (auto& line : m_line_data)
	{
		if (line.color == color)
		{
			data = &line.data;
			break;
		}
	}
	if (!data)
	{
		m_line_data.push_back({ color, {} });
		data = &m_line_data.back().data;
	}

	GLuint num_vertices = (GLuint)vertices.size() / 3;
	GLuint indices_size = (GLuint)data->vertices.size() / 3;

	for (GLuint i = 0; i < num_vertices; i++)
	{
		data->indices.emplace_back(indices_size + i);
	}
	data->indices.emplace_back(0xFFFFFFFF);

	data->vertices.insert(data->vertices.end(), vertices.begin(), vertices.end());
}

bool CompareSpriteDepths(const std::tuple<Sprite*, glm::vec3, glm::vec2>& a, const std::tuple<Sprite*, glm::vec3, glm::vec2>& b)
{
	if (std::get<0>(a)->GetColor().a == 1.0f)
	{
		if (std::get<0>(b)->GetColor().a == 1.0f)
		{
			return std::get<1>(a).z < std::get<1>(b).z;
		}
		else
		{
			return true;
		}
	}
	else if (std::get<0>(b)->GetColor().a != 1.0f)
	{
		return std::get<1>(a).z > std::get<1>(b).z;
	}
	return false;
}

void Renderer2D::_RenderSprites()
{
	m_shader_textured.UseShader();
	glUniformMatrix4fv(m_shader_textured.GetUniformLocation("projection"), 1, GL_FALSE, &m_projection[0][0]);

	for (auto& sprite_data : m_sprite_data)
	{
		sprite_data.material.texture->Bind(GL_TEXTURE0);
		glUniform4fv(m_shader_textured.GetUniformLocation("colour"), 1, &sprite_data.material.color[0]);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sprite_data.data.indices.size() * sizeof(unsigned int), &sprite_data.data.indices[0], GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sprite_data.data.vertices.size() * sizeof(GLfloat), &(sprite_data.data.vertices[0]), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)sprite_data.data.indices.size(), GL_UNSIGNED_INT, (void*)0);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, pair.second.first.size() / 9);
	}

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Renderer2D::_SortSprites()
{
	std::sort(m_sprites.begin(), m_sprites.end(), CompareSpriteDepths);
}

void Renderer2D::_RenderLines()
{
	m_shader_color.UseShader();
	glUniformMatrix4fv(m_shader_color.GetUniformLocation("projection"), 1, GL_FALSE, &m_projection[0][0]);

	for (auto& line_data : m_line_data)
	{
		glUniform3fv(m_shader_color.GetUniformLocation("color"), 1, &line_data.color[0]);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, line_data.data.indices.size() * sizeof(unsigned int), &line_data.data.indices[0], GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, line_data.data.vertices.size() * sizeof(GLfloat), &(line_data.data.vertices[0]), GL_STATIC_DRAW);

		glDrawElements(GL_LINE_STRIP, (GLsizei)line_data.data.indices.size(), GL_UNSIGNED_INT, (void*)0);
	}

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer2D::_RenderStencil()
{
	if (m_stencil.indices.size() == 0)
		return;

	m_shader_stencil.UseShader();
	glUniformMatrix4fv(m_shader_stencil.GetUniformLocation("projection"), 1, GL_FALSE, &m_projection[0][0]);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_stencil.indices.size() * sizeof(unsigned int), &m_stencil.indices[0], GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, m_stencil.vertices.size() * sizeof(GLfloat), &(m_stencil.vertices[0]), GL_STATIC_DRAW);

	//Drawing to stencil buffer
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF); // Write to stencil buffer
	glDepthMask(GL_FALSE); // Don't write to depth buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glDrawElements(GL_TRIANGLE_FAN, (GLsizei)m_stencil.indices.size(), GL_UNSIGNED_INT, (void*)0);

	glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
	glStencilMask(0x00); // Don't write anything to stencil buffer
	glDepthMask(GL_TRUE); // Write to depth buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_STENCIL_TEST);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

Sprite::Sprite()
{
	m_texture = NULL;
	m_sprite_offset = {};
	m_sprite_size = {};
	m_color = { 1.0f,1.0f,1.0f,1.0f };
}

Sprite::Sprite(Texture2D * texture, glm::ivec2 sprite_offset, glm::ivec2 sprite_size, glm::vec4 color)
{
	m_texture = texture;
	m_sprite_offset = sprite_offset;
	m_sprite_size = sprite_size;
	m_color = color;
}

void Sprite::SetTexture(Texture2D * texture)
{
	m_texture = texture;
}

void Sprite::SetColor(glm::vec4 color)
{
	m_color = color;
}

Texture2D * Sprite::GetTexture()
{
	return m_texture;
}

glm::vec4 Sprite::GetColor()
{
	return m_color;
}

glm::ivec2 Sprite::GetSpriteOffset()
{
	return m_sprite_offset;
}

glm::ivec2 Sprite::GetSpriteSize()
{
	return m_sprite_size;
}

