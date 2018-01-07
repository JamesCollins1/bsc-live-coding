#version 330 core

in vec4 vertexColourOut;
in vec2 vertexTextureCoordOut;
in vec4 diffuse;
in vec4 PosterizedDiffuse;
in vec4 specular;

out vec4 Colour;

uniform vec4 fragColour=vec4(1.0,1.0,1.0,1.0);

uniform float gamma = 0.4;
uniform float numColours = 8.0;

uniform sampler2D baseTexture;
float GetBrightness (vec3 Colour)
{
	return (Colour.r + Colour.g + Colour.b) / 3.0;
}
void main()
{
  vec3 Posterize = texture2D(baseTexture, vertexTextureCoordOut).rgb;
  Posterize = pow(Posterize, vec3(gamma, gamma, gamma));
  Posterize = Posterize * numColours;
  Posterize = floor(Posterize);
  Posterize = Posterize / numColours;
  Posterize = pow(Posterize, vec3(1.0/gamma));
  Colour = diffuse / vec4(Posterize, 1.0);
}