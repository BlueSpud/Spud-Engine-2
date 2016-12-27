#version 330

uniform sampler2D blurred;
uniform sampler2D final_render;

in vec2 tex_coord0;

out vec4 albedo;

void main() {

    // Output the texture
    albedo = texture(final_render, tex_coord0) * texture(blurred, tex_coord0).r;
    
}
