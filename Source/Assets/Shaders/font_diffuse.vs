precision mediump float;

attribute vec4 position;
attribute vec4 color;
attribute vec2 texcoords;

uniform mat4 model;
uniform mat4 viewProjection;

varying vec2 v_texcoords;
varying vec4 v_color;

void main()
{   v_texcoords = texcoords;
    v_color = color;
    gl_Position = viewProjection * model * position;
}
 