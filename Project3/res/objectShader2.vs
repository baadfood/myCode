#version 330

uniform mat4 camera;

layout (location = 0) in vec3 position; 
layout (location = 1) in vec2 texCoord; 
layout (location = 2) in vec3 normal; 
layout (location = 3) in mat4 model; 

out vec3 fragVert;
out vec2 fragTexCoord;
out vec3 fragNormal;
out mat4 fragModel;
out float fragMaterialShininess;
out vec3 fragMaterialSpecularColor;

void main()
{
  fragModel = model;
  fragTexCoord = texCoord;
  fragNormal = normal;
  fragVert = position;
  fragMaterialShininess = 50;
  fragMaterialSpecularColor = vec3(0.5, 0.5, 0.5);
  gl_Position = camera * model * vec4(position, 1);
}