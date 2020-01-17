precision mediump float;
varying vec3 v_color;
uniform sampler2D s_texture;
varying vec2 tex_coord;

void main()
{   
    vec4 color = vec4(v_color,1.0);
    gl_FragColor = texture2D(s_texture, tex_coord) * color;
}
