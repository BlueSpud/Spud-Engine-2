#version 330

uniform sampler2D tex_occlusion;
uniform vec2 axis;
uniform vec2 screen_size;

in vec2 tex_coord0;

out float blurred_occlusion;

const float EDGE_SHARPNESS = 1.0;
const float EDGE_SHARPNESS_2k = 2000.0 * EDGE_SHARPNESS;

const float SCALE = 1.0;
const int RADIUS = 4;
//const float EPSILON = 0.0001;

const float kernel[5] = float[5](0.153170, 0.144893, 0.122649, 0.092902, 0.062970);

const float unpack_1 = (256.0 / 257.0);
const float unpack_2 = (1.0 / 257.0);

float unpackZ(vec2 packed) { return packed.x * unpack_1 + packed.y * unpack_2; }

void main() {
	
	// Set sum to the direct sample
	vec4 main_sample = texture(tex_occlusion, tex_coord0);
	float unpacked_zm = unpackZ(main_sample.gb);
	
	if (unpacked_zm == 1.0)
		discard;
	
	float sum = main_sample.r;
	
	// Do initial weight
	float weight_total = kernel[0];
	sum = sum * weight_total;
	
	// Compute how far we are going to go each sample
	vec2 s_step = (axis * SCALE) / screen_size;
	
	for (int i = -RADIUS; i <= RADIUS; i++) {
		
		if (i != 0) {
			
			// Sample from the texture
			vec4 s = texture(tex_occlusion, tex_coord0 + s_step * i);
			float unpacked_z = unpackZ(s.gb);
			
			// Compute the weight of this sample
			float s_weight = 0.3 + kernel[abs(i)];
			s_weight = s_weight * max(0.0, 1.0 - EDGE_SHARPNESS_2k * abs(unpacked_z - unpacked_zm));
			
			sum = sum + s.x * s_weight;
			weight_total = weight_total + s_weight;
			
		}
		
	}
	
	blurred_occlusion = sum / weight_total;
    
}
