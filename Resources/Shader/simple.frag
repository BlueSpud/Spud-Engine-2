#version 330

uniform sampler2D tex_albedo;

in vec2 tex_coord0;

out vec4 albedo;

void main() {

    // Output the texture
    albedo = texture(tex_albedo, tex_coord0);
    
}
