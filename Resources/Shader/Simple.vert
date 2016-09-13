#version 330

uniform mat4 mat_projection;
uniform mat4 mat_model;
uniform mat4 mat_view;

in vec3 position;
in vec3 normal;
in vec2 tex_coord;

out vec3 position_world;
out vec3 position_view;
out vec3 normal0;
out vec2 tex_coord0;

void main() {
    
    mat4 mat_model_view = mat_view * mat_model;
    
    normal0 = (transpose(inverse(mat_model)) * vec4(normal, 0.0)).xyz;
    tex_coord0 = tex_coord;
    
    // Calculate world space and view space positions
    position_world = (mat_model * vec4(position, 1.0)).xyz;
    
    vec4 position_view_f = gl_Position = mat_projection * mat_view * vec4(position_world, 1.0);
    position_view = position_view_f.xyz;
    
}
