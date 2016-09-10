#version 330

uniform sampler2D tex_albedo;
uniform sampler2D tex_overlay;

in vec3 normal0;
in vec2 tex_coord0;

out vec4 out_color;

void main() {
    
    vec4 overlay = texture(tex_overlay, tex_coord0);
    out_color = texture(tex_albedo, tex_coord0) + overlay * overlay.a;
    
}
