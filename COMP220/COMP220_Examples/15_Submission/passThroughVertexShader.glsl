#version 330 core

layout(location=0) in vec2 vertexPosition;


uniform mat4 MVPMatrix;

out vec4 vertexColourOut;
out vec2 vertexTextureCoordOut;

void main()
{
	gl_Position = MVPMatrix * vec4(vertexPosition,0.0f, 1.0f);
	vertexTextureCoordOut = (vertexPosition + 1.0f) / 2.0f;
}