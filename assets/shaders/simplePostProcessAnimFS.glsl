#version 150

out vec4 FragColor;
in vec2 textureCoords;

uniform sampler2D texture0;
uniform float time;
uniform float scale=0.3;

void main()
{
	vec2 newTextureCoords=textureCoords;
	newTextureCoords.x += sin(time)*scale;
	newTextureCoords.y += cos(time)*scale;
	FragColor = texture(texture0, newTextureCoords);
}
