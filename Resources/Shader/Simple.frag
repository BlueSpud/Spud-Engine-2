#version 330

uniform sampler2D tex_albedo;

in vec3 normal0;
in vec2 tex_coord0;

out vec4 out_color;

void main() {
    
    out_color = texture(tex_albedo, tex_coord0);
    
}
