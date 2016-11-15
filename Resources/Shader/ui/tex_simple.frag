#version 330

uniform sampler2D tex_albedo;

uniform vec2 start;
uniform vec2 size;

in vec2 tex_coord0;

out vec4 final_color;

void main() {

    // Read the texture
    vec4 texture_s = texture(tex_albedo, vec2(start.x, start.y) + tex_coord0 * size);
    
    if (texture_s.w == 0)
        discard;
    
    final_color = texture_s;
    
}
