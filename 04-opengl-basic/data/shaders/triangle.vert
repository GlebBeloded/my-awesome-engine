attribute vec3 a_position;
attribute vec3 a_color;
attribute vec2 a_tex_coord;
varying vec4 v_position;
varying vec3 v_color;
varying vec2 tex_coord;

void main()
{   
    v_color = a_color;
    v_position = vec4(a_position, 1.0);
    gl_Position = v_position;
    tex_coord = a_tex_coord;
}