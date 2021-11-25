#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
flat in int  specialColorEn_out;
in vec4 vCol_out;

// sampler2D holds texture id that we have assigned with glUniform1i and activated with glActiveTexture in main.cpp
uniform sampler2D ourTexture;

void main()
{
	if (specialColorEn_out == 0)
		FragColor = texture( ourTexture, TexCoord );
	else if (specialColorEn_out == 1)
		FragColor = vCol_out;
	else
		FragColor = mix(texture(ourTexture, TexCoord), vCol_out, 0.5);
}
