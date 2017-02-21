#version 330

uniform sampler2D tex_shadow;

uniform int tile_size;

const int diameter = 3;
const int radius = diameter / 2;

in vec2 tex_coord0;

out vec2 moments;

void main() {

	vec2 sum;
	int samples;
	
	for (int i = -radius; i <= radius; i++) {
		
		sum = sum + texture(tex_shadow, tex_coord0.xy + vec2(0.0, float(i) / float(tile_size))).rg;
		samples++;
		
	}
	
	moments = sum / samples;

}
