#version 150

uniform mat4 model;
uniform mat4 camera;

in vec3 position;
in vec2 texCoord;
in vec3 normal;

out vec3 fragVert;
out vec2 fragTexCoord;
out vec3 fragNormal;

void main()
{
  fragTexCoord = texCoord;
  gl_Position = camera * model * vec4(position, 1);
}