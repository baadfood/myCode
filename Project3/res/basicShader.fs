#version 150

uniform mat4 model;
uniform sampler2D tex;

in vec2 fragTexCoord;

out vec4 finalColor;

void main()
{
	gl_FragColor = texture2D(tex, fragTexCoord);
}