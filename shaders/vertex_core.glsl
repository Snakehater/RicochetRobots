#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
  
out vec2 TexCoord;
out vec4 vCol_out;
flat out int specialColorEn_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int  ignoreMatrix;
uniform int specialColorEn;
uniform vec4 vCol;

void main()
{
	if (ignoreMatrix == 0)
		gl_Position = projection * view * model * vec4( aPos, 1.0f );
	else
		gl_Position = vec4(aPos, 1.0f);
	TexCoord = vec2( aTexCoord.x, 1.0 - aTexCoord.y );
	specialColorEn_out = specialColorEn;
	vCol_out = vCol;
}
