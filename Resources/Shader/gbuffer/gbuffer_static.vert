#version 330

uniform mat4 mat_projection;
uniform mat4 mat_model;
uniform mat4 mat_view;

in vec3 position;
in vec3 normal;
in vec2 tex_coord;
in vec3 tangent;

out vec3 position0;
out vec2 tex_coord0;
out mat3 tbn_matrix;

void main() {

	position0 = (mat_model * vec4(position, 1.0)).xyz;
	tex_coord0 = tex_coord;

	// Calculate stuff for normal mapping
	vec3 normal0 = normalize((mat_model * vec4(normal, 0.0)).xyz);

	vec3 tangent0 = normalize((mat_model * vec4(tangent, 0.0)).xyz);
	tangent0 = normalize(tangent0 - dot(tangent0, normal0) * normal0);

	vec3 bitangent = normalize(cross(tangent0, normal0));

	tbn_matrix = mat3(tangent0, bitangent, normal0);
	gl_Position = mat_projection * mat_view * vec4(position0, 1.0);


}
