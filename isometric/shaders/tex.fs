#version 400

in vec2 texcoord;

uniform sampler2D texBuffer;
uniform vec3 vertexColor;

out vec4 color;
void main()
{
	color = texture(texBuffer, texcoord) * vec4(vertexColor,1.0);

}