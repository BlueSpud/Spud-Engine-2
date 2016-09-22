#version 330

uniform sampler2D tex_depth;
uniform sampler2D tex_albedo;
uniform sampler2D tex_normal;

in vec3 position0;
in vec2 tex_coord0;
out vec4 out_color;

void main() {

    out_color = texture(tex_albedo, tex_coord0);

}
