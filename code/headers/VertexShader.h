//NOT USED
#pragma once
#include <GL\glew.h>

extern const char* vertex_shader_example =
"#version 430\n"
"layout(location = 0) in vec2 vertex_position;"
"layout(location = 1) in vec3 vertex_colour;"
"layout(location = 2) uniform mat4 MVP;"
"out vec3 colour;"
"void main() {"
"  colour = vertex_colour;"
"  gl_Position = MVP * vec4(vertex_position, 1.0, 1.0);"
"}";

extern const GLchar* vertexShaderSource =
"in vec2 texCoord;"
"varying vec2 TexCoord0;"
//"in vec4 color;"
"void main()"
"{"
"	TexCoord0 = texCoord;"
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"};";

/*"void main(void)"
"{"
"	vec4 a = gl_Vertex;"
"	a.x = a.x * 0.5;"
"	a.y = a.y * 0.5;"
"	gl_Position = gl_ModelViewProjectionMatrix * a;"
"};";*/
