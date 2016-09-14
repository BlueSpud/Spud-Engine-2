#version 330

uniform mat4 mat_projection;
uniform mat4 mat_model;
uniform mat4 mat_view;

in vec3 position;
in vec3 normal;
in vec2 tex_coord;

out vec3 position0;
out vec3 normal0;
out vec2 tex_coord0;

void main() {
    
    position0 = (mat_model * vec4(position, 1.0)).xyz;
    normal0 = (transpose(inverse(mat_model)) * vec4(normal, 0.0)).xyz;
    tex_coord0 = tex_coord;
    
    gl_Position = mat_projection * mat_view * vec4(position0, 1.0);
    
}
