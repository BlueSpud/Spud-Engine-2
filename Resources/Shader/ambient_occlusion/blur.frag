#version 330

uniform sampler2D tex_occlusion;

int blur_size = 4;

in vec2 tex_coord0;

out vec4 albedo;

void main() {

    // Get how far a pixel is away
    vec2 tex_coord_step = 1.0 / vec2(textureSize(tex_occlusion, 0));
    float final_result;
    int blur_start = -blur_size / 2;
    int blur_end = blur_size / 2;
    
    for (int i = -2; i < 2; i++)
        for (int j = -2; j < 2; j++) {
            
            final_result += texture(tex_occlusion, tex_coord0 + vec2(float(i), float(j)) * tex_coord_step).r;
            
        }
    
    final_result = final_result / (blur_size * blur_size);
    
    // Output the texture
    albedo = vec4(final_result, 0.0, 0.0, 1.0);
    
}
