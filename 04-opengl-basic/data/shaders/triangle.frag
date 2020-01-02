varying vec3 v_color;
uniform sampler2D s_texture;
varying vec2 tex_coord;

void main()
{
    gl_FragColor = texture2D(s_texture, tex_coord);
}