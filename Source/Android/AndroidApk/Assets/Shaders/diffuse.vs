attribute vec4 position;
attribute vec4 color;
attribute vec2 texcoords;

uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;
} transform;

varying vec2 v_texcoords;
varying vec4 v_color;

void main()
{   v_texcoords = texcoords;
    v_color = color;
    gl_Position = transform.viewProjection * transform.model * position;
}
 