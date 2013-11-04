attribute vec4 position;
attribute vec4 color;
attribute vec2 texcoords;

varying vec2 v_texcoords;
varying vec4 frag_color;

void main()
{   v_texcoords = texcoords;
    frag_color = color;
    gl_Position = position;
}
 