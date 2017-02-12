#version 330

uniform sampler2D tex_albedo;
uniform sampler2D tex_orm;
uniform sampler2D tex_normal;

in vec3 position0;
in vec2 tex_coord0;
in mat3 tbn_matrix;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 orm;

void main() {

    // Get the normal from the map
    vec3 normal_map = tbn_matrix * normalize((2.0 * texture(tex_normal, tex_coord0).xyz - 1.0));

    albedo = texture(tex_albedo, tex_coord0);
    normal = normalize(normal_map);
    orm = texture(tex_orm, tex_coord0).xyz;

}
