#version 330

uniform sampler2D tex_depth;
uniform sampler2D tex_normal;
uniform sampler2D tex_noise;

uniform mat4 mat_view_scene;
uniform mat4 mat_projection_scene;
uniform mat4 inv_proj;

in vec2 tex_coord0;
uniform vec2 tex_coord_scale;

uniform vec3 kernel[64];
uniform int kernel_size;

uniform vec2 planes;

out float occlusion_out;

float linearizeDepth(float depth) {
    
    depth = depth * 2.0 - 1.0;
    return (2.0 * planes.x * planes.y) / (planes.y + planes.x - depth * (planes.y - planes.x));
    
}

void main() {

    // Get the view-space normal
    vec4 normal_sample = texture(tex_normal, tex_coord0);
    if (normal_sample.w == 0.0)
        discard;
    vec3 normal = (mat_view_scene * vec4(normal_sample.xyz, 0.0)).xyz;

    // Get position from depth
    float depth = texture(tex_depth, tex_coord0).x * 2.0 - 1.0;
    vec4 position_p = vec4(tex_coord0 * 2.0 - 1.0, depth, 1.0);
    position_p = inv_proj * position_p;
    vec3 position = (position_p / position_p.w).xyz;
    
    vec3 random_sample = texture(tex_noise, tex_coord0 * tex_coord_scale).xyz * 2.0 - 1.0;
    vec3 tangent = normalize(random_sample - normal * dot(random_sample, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn_matrix = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    
    // Sample all the values
    for (int i = 0; i < kernel_size; i++) {
        
        // Get the position of the sample
        vec3 sample_position = tbn_matrix * kernel[i];
        sample_position = position + sample_position * 0.1;
        
        // Get an offset
        vec4 offset = mat_projection_scene * vec4(sample_position, 1.0);
        offset = (offset / offset.w) * 0.5 + 0.5;
        
        float sampled_depth = linearizeDepth(texture(tex_depth, offset.xy).r);


        occlusion += (-sampled_depth >= sample_position.z ? 1.0 : 0.0);
        
    }
    
    occlusion = 1.0 - occlusion / kernel_size;
    
    occlusion_out = occlusion;

}
