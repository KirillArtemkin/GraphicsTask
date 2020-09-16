#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 iNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(Position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * iNormal;


	gl_Position = projection * view * model *vec4(Position, 1.0f);
}