#version 330

uniform sampler2D tex_depth;
uniform sampler2D tex_albedo;
uniform sampler2D tex_normal;

uniform mat4 mat_view_scene;
uniform mat4 mat_inverse_proj;
uniform mat4 mat_proj;

uniform vec2 planes;

in vec2 tex_coord0;

out vec3 reflection;

float linearizeDepth(float depth) {
    
    depth = depth * 2.0 - 1.0;
    return (2.0 * planes.x * planes.y) / (planes.y + planes.x - depth * (planes.y - planes.x));
    
}

vec2 binarySearch(vec3 reflection_vector, vec3 hit_position) {
    
    // Binary search
    for (int j = 0; j < 10; j++) {
        
        vec4 projected_hit = mat_proj * vec4(hit_position, 1.0);
        projected_hit = projected_hit / projected_hit.w;
        projected_hit = projected_hit / 2.0 + 0.5;
        
        float depth = texture(tex_depth, projected_hit.xy).x;

        if (depth > projected_hit.z) {
            
            hit_position = hit_position + reflection_vector;
            
        }
        
        reflection_vector = reflection_vector / 2.0;
        hit_position = hit_position - reflection_vector;
        
    }
    
    vec4 projected_hit = mat_proj * vec4(hit_position, 1.0);
    projected_hit = projected_hit / projected_hit.w;
    projected_hit = projected_hit / 2.0 + 0.5;
    
    return projected_hit.xy;
    
}

void main() {

    // Get the view-space normal
    vec4 normal_sample = texture(tex_normal, tex_coord0);
    if (normal_sample.w == 0.0)
        discard;
    vec3 normal = normalize((mat_view_scene * vec4(normal_sample.xyz, 0.0)).xyz);
    
    float depth = texture(tex_depth, tex_coord0).x;
    vec4 position_p = vec4(tex_coord0 * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    position_p = mat_inverse_proj * position_p;
    vec3 position = position_p.xyz / position_p.w;
    
    vec3 reflection_vector = normalize(reflect(normalize(position), normal)) * 0.5;
    
    vec3 hit_position = position;
    vec3 reflection_color;
    
    if (dot(reflection_vector, position) == 0)
        discard;
    
    for (int i = 0; i < 10; i++) {
        
        hit_position = hit_position + reflection_vector;
        
        // project it
        vec4 projected_hit = mat_proj * vec4(hit_position, 1.0);
        projected_hit = projected_hit / projected_hit.w;
        projected_hit = projected_hit / 2.0 + 0.5;
        
        depth = texture(tex_depth, projected_hit.xy).x;

        if (1.0 - length(normalize(clamp(projected_hit.xy, 0.0, 1.0) - projected_hit	.xy)) != 1.0)
            break;
        
        if (depth < projected_hit.z) {
            
            reflection_color = texture(tex_albedo, projected_hit.xy).xyz;
            break;
            
        }
        
    }
    
    // Get the regular color
    vec3 albedo = texture(tex_albedo, tex_coord0).xyz + reflection_color;
    
    reflection = albedo;

}
