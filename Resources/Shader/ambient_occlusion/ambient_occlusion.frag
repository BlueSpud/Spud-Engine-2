#version 330

uniform sampler2D tex_depth;

uniform vec4 projection_info;
uniform vec2 screen_size;
uniform vec2 planes;

in vec2 tex_coord0;

out vec3 occlusion_out;

#define TURNS 2.0
#define SAMPLES 9.0
#define EPSILON 0.03
#define BIAS 0.001

#define SIGMA 0.87
#define KAPPA 1.0

vec3 clip_info = vec3(planes.x * planes.y, planes.x - planes.y, planes.y);

float projectDepth(float depth) { return clip_info.x / (clip_info.y * depth + clip_info.z); }

vec3 reconstructPosition(ivec2 pos, float depth) {
	
	// Do transformations to input data
	vec2 pos_f = vec2(pos) + 0.5;
	return vec3((pos_f * projection_info.xy + projection_info.zw) * depth, depth);
	
}

vec3 reconstructNormal(vec3 C_pos) {
	
	// Construct the normal based on the derivitives
	vec3 normal = vec3(cross(dFdy(C_pos), dFdx(C_pos)));
	return normalize(normal);
	
}

const float TURNS_2PI = (TURNS * 6.28318);

vec3 getSampleOffset(int s, float offset) {
	
	float a = (float(s) + 0.5) * (1.0 / SAMPLES);
	float theta = a * TURNS_2PI + offset;
	
	return vec3(cos(theta), sin(theta), a);
	
}

vec3 getSamplePosition(ivec2 pixel_position, vec3 location) {
	
	// Figure out where we are sampling
	ivec2 sample_pos = ivec2(location.xy * location.z) + pixel_position;
	
	// Sample
	float depth = texture(tex_depth, vec2(sample_pos) / screen_size).r;
	depth = projectDepth(depth);
	
	return reconstructPosition(sample_pos, depth);
	
}

float sampleAO(ivec2 pixel_position, vec3 C_pos, vec3 C_n, float disk_rad, int s, float rotation) {
	
	// Get offset
	vec3 location = getSampleOffset(s, rotation);
	location.z = location.z * disk_rad;
	
	vec3 Q = getSamplePosition(pixel_position, location);
	vec3 V = Q - C_pos;
	
	float V_dot_V = dot(V, V);
	
	// This is differnt than the orignal paper, use this to combat an issue with getting darker as it gets further away
	float V_dot_C_n = dot(V, C_n) - C_pos.z * 0.001;
	
	float f = max(1.0 - V_dot_V, 0.0);
	return pow(f, KAPPA) * max((V_dot_C_n - BIAS) / (EPSILON + V_dot_V), 0.0);
	
}

void main() {
	
	// Hash the coordinates to get a random roation
	ivec2 pixel_position = ivec2(int(tex_coord0.x * screen_size.x), int(tex_coord0.y * screen_size.y));
	float rotation = (3 * pixel_position.x ^ pixel_position.y + pixel_position.x * pixel_position.y) * 10.0;
	
	// Get position and normal of C (origin)
	float depth = texture(tex_depth, tex_coord0).r;
	float C_depth = projectDepth(depth);
	
	vec3 C_pos = reconstructPosition(pixel_position, C_depth);
	vec3 C_n = reconstructNormal(C_pos);
	
	// Get radius of disk
	float disk_rad = -200.0 * 1.0 / C_pos.z;
	
	float occlusion = 0.0;
	for (int s = 0; s < SAMPLES; s++)
		occlusion = occlusion + sampleAO(pixel_position, C_pos, C_n, disk_rad, s, rotation);
	
	occlusion = 1.0 - (occlusion * (2.0 * SIGMA / float(SAMPLES)));
	
	// Now we pack the C_pos.z
	float packed_z = clamp(C_pos.z * (1.0 / planes.y), 0.0, 1.0);
	float packed_f = floor(packed_z * 256.0);
	
	vec2 packed = vec2(packed_f * (1.0 / 256.0), packed_z * 256.0 - packed_f);
	
	// Now we do a small blur
	if (abs(dFdx(C_pos.z)) < 0.02)
		occlusion = occlusion - dFdx(occlusion) * (float(pixel_position.x & 1) - 0.5);
	
	if (abs(dFdy(C_pos.z)) < 0.02)
		occlusion = occlusion - dFdy(occlusion) * (float(pixel_position.y & 1) - 0.5);
	
	occlusion_out = vec3(occlusion, packed);
	
}
