precision mediump float;

attribute vec4 position;
attribute vec4 color;
attribute vec2 texcoords;

uniform vec3 time;
uniform mat4 model;
uniform mat4 viewProjection;

varying vec2 v_texcoords;
varying vec4 v_color;

void main()
{   
	v_texcoords = texcoords;
	v_texcoords.x += time[0]*0.00001;
	
    v_color = color;
    gl_Position = viewProjection * model * position;
}
 