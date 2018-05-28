//NOT USED
#pragma once
#include <GL\glew.h>

extern const char* fragment_shader_example =
"#version 430\n"
"in vec4 colour;"
"out vec4 frag_colour;"
"void main() {"
"  frag_colour = vec4(colour, 1.0);"
"}";

extern const GLchar* fragmentShaderSource =
/*"uniform sampler2D tex;"
"in vec2 fragTexCoord;"
"out vec4 finalColor;"
"void main() "
"{"
"	finalColor = texture(tex, fragTexCoord);"
"}";*/
"uniform sampler2D Texture0;"
"uniform vec4 LightPosition0;"
"uniform vec4 AllLightAmbient_MaterialAmbient;"
"uniform vec4 LightMaterialDiffuse0;"
"uniform vec4 LightMaterialSpecular0;"
"uniform float MaterialShininess;"
// Varying
"varying vec2 TexCoord0;"
"varying vec3 HalfVector0;"
"varying vec3 EyeNormal;"
/*"uniform sampler2D Texture0;"
"varying vec2 TexCoord0;"*/
"void main(void)"
"{"
"	vec4 texel = gl_Color*texture2D(Texture0, gl_TexCoord[0].st);"
"	gl_FragColor = texel;"
"}";
