#version 330 core

in vec4 vertexColourOut;
in vec2 vertexTextureCoordOut;
in vec4 diffuse;
in vec4 specular;

out vec4 Colour;

uniform vec4 fragColour=vec4(1.0,1.0,1.0,1.0);

uniform sampler2D baseTexture;
float GetBrightness (vec3 Colour)
{
	return (Colour.r + Colour.g + Colour.b) / 3.0;
}
void main()
{
vec4 TextureColour = texture(baseTexture , vertexTextureCoordOut);
float Brightness = GetBrightness(TextureColour.rgb);
	if(Brightness > 0.86){
	Colour = (diffuse * texture(baseTexture,vertexTextureCoordOut));
	}
	else if(Brightness > 0.80){
	Colour = (diffuse / 2 * texture(baseTexture,vertexTextureCoordOut));
	}
	else{
	Colour = (diffuse / 8 * texture(baseTexture,vertexTextureCoordOut));
	}

}