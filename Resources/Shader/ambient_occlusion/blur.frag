#version 330

uniform sampler2D tex_occlusion;

uniform int noise_size;

in vec2 tex_coord0;

out float blurred_occlusion;

void main() {

    // Get how far a pixel is away
    vec2 tex_coord_step = 1.0 / vec2(textureSize(tex_occlusion, 0));
    float final_result;
    int blur_start = (noise_size / 2) * -1;
    int blur_end = noise_size / 2;
    
    for (int i = blur_start; i < blur_end; i++)
        for (int j = blur_start; j < blur_end; j++)
            final_result += texture(tex_occlusion, tex_coord0 + vec2(float(i), float(j)) * tex_coord_step).r;
    
    final_result = final_result / (noise_size * noise_size);
    
    // Output the blurred result
    blurred_occlusion = final_result;
    
}
