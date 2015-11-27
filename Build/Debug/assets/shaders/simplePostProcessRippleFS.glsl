#version 150

//http://adrianboeing.blogspot.in/2011/02/ripple-effect-in-webgl.html

out vec4 FragColor;
in vec2 textureCoords;

uniform sampler2D texture0;
uniform float time;
uniform float scale=0.03;
uniform float phaseShift=4.0;
uniform float height=12.0;
uniform vec2 resolution;

void main()
{
	vec2 cPos = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
	float cLength = length(cPos);
	vec2 uv = gl_FragCoord.xy/resolution.xy+(cPos/cLength)*cos(cLength*height-time*phaseShift)*scale;
	vec3 col = texture(texture0,uv).xyz;
	FragColor = vec4(col,1.0f);
}
