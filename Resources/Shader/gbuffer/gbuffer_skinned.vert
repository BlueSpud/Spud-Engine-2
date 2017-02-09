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


	position0 = (mat_model * vec4(position, 1.0)).xyz;

	// Calculate the skinning
	vec4 temp_position = vec4(position0, 1.0);
	vec3 skinned_position;

	for (int i = 0; i < 4; i++) {

		if (bone_indicies[i] != -1)
			skinned_position = skinned_position + ((temp_position * bones[int(bone_indicies[i])]) * vertex_weights[i]).xyz;
		else break;

	}

	position0 = skinned_position;

	tex_coord0 = tex_coord;

	// Calculate stuff for normal mapping
	vec3 normal0 = normalize((mat_model * vec4(normal, 0.0)).xyz);

	vec3 tangent0 = normalize((mat_model * vec4(tangent, 0.0)).xyz);
	tangent0 = normalize(tangent0 - dot(tangent0, normal0) * normal0);

	vec3 bitangent = normalize(cross(tangent0, normal0));

	tbn_matrix = mat3(tangent0, bitangent, normal0);

	gl_Position = mat_projection * mat_view * vec4(position0, 1.0);


}
