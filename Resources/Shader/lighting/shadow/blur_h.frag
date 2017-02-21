#version 330

uniform sampler2D tex_shadow;

uniform ivec2 shadow_tile;
uniform vec2 direction;

const int diameter = 3;
const int radius = diameter / 2;

const float tile_step = 1.0 / 8.0;

in vec2 tex_coord0;

out vec2 moments;

void main() {

	vec2 sum;
	int samples;
	
	for (int i = -radius; i <= radius; i++) {
		
		vec2 tex_coord_shadow = tex_coord0.xy / 8.0 + vec2(tile_step) * shadow_tile + vec2(float(i) / 8192.0, 0.0);
		sum = sum + texture(tex_shadow, tex_coord_shadow).rg;
		samples++;
		
	}
	
	moments = sum / samples;

}
