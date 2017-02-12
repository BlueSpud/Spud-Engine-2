#version 330

uniform sampler2D tex_albedo;

uniform vec2 start;
uniform vec2 size;

in vec2 tex_coord0;

out vec4 final_color;

float a = 0.47;
float b = 0.05;

void main() {

    // Read the texture
    vec4 texture_s = texture(tex_albedo, start + tex_coord0 * size);
    
    // Do the signed distance field calculation
    final_color = vec4(1.0, 1.0, 1.0, smoothstep(a - b, a + b, texture_s.w));
    
}
