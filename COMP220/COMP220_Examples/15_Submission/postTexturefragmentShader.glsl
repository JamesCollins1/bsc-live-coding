#version 330 core

in vec4 vertexColourOut;
in vec2 vertexTextureCoordOut;

out vec4 Colour;


uniform sampler2D texture0;

void main()
{
	Colour = texture(texture0, vertexTextureCoordOut);
}