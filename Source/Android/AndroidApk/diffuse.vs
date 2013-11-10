attribute vec4 position;
attribute vec4 color;

varying vec4 frag_color;

void main()
{   
    frag_color = color;
    gl_Position = position;
}
 