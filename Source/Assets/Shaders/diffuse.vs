precision mediump float;

attribute vec4 position;
attribute vec4 color;
attribute vec2 texcoords;

uniform mat4 model;
uniform mat4 viewProjection;
uniform vec3 time;

varying vec2 v_texcoords;
varying vec4 v_color;

void main()
{   v_texcoords = texcoords;
    v_color = color;// * sin(time.y);
    gl_Position = viewProjection * model * position;
}
 