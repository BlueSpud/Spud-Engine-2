#version 330

uniform mat4 mat_projection;
uniform mat4 mat_model;
uniform mat4 mat_view;

uniform mat4 bones[16];

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 tangent;
layout(location = 5) in vec4 bone_indicies;
layout(location = 6) in vec4 vertex_weights;

out vec3 position0;
out vec2 tex_coord0;
out mat3 tbn_matrix;

void main() {


	position0 = position;
	vec3 normal0 = normal;
	vec3 tangent0 = tangent;

	// Calculate the skinning
	vec4 temp_position = vec4(position0, 1.0);
	position0 = vec3(0.0);

	vec4 temp_normal = vec4(normal0, 0.0);
	normal0 = vec3(0.0);

	vec4 temp_tangent = vec4(tangent0, 0.0);
	tangent0 = vec3(0.0);

	for (int i = 0; i < 4; i++) {

		if (bone_indicies[i] != -1) {

			position0 = position0 + ((temp_position * bones[int(bone_indicies[i])]) * vertex_weights[i]).xyz;
			normal0 = normal0 + ((temp_normal * bones[int(bone_indicies[i])]) * vertex_weights[i]).xyz;
			tangent0 = tangent0 + ((temp_tangent * bones[int(bone_indicies[i])]) * vertex_weights[i]).xyz;

		} else break;

	}

	// Transform it into world space
	position0 = (mat_model * vec4(position0, 1.0)).xyz;
	normal0 = normalize((mat_model * vec4(normal0, 0.0)).xyz);
	tangent0 = normalize((mat_model * vec4(tangent0, 0.0)).xyz);

	tex_coord0 = tex_coord;

	// Calculate stuff for normal mapping
	tangent0 = normalize(tangent0 - dot(tangent0, normal0) * normal0);

	vec3 bitangent = normalize(cross(tangent0, normal0));

	tbn_matrix = mat3(tangent0, bitangent, normal0);

	gl_Position = mat_projection * mat_view * vec4(position0, 1.0);


}
