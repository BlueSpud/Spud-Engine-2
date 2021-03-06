
#version 330

uniform mat4 mat_projection;
uniform mat4 mat_model;
uniform mat4 mat_view;

in vec3 position;

void main() {
    
    gl_Position = mat_projection * mat_view * mat_model * vec4(position, 1.0);
    
}
