#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;
	mat3 normal;
	vec3 viewPosition;
} transform;

in Vertex 
{
	vec4  position;
	vec2  texcoord;
} Vert;

void main(void)
{
	Vert.position = position;
	Vert.texcoord = texcoord;
	
	vec4 vertex = transform.model * vec4(position, 1.0)
	gl_Position = transform.viewProjection * vertex;
}
