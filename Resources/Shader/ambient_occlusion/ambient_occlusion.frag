#version 330

uniform sampler2D tex_depth;

uniform mat4 mat_view_scene;
uniform mat4 mat_projection_scene;

uniform vec4 projection_info;
uniform vec2 screen_size;
uniform vec2 planes;

in vec2 tex_coord0;

out vec3 occlusion_out;

#define TURNS 7.0
#define SAMPLES 9.0
#define EPSILON 0.01
#define BIAS 0.01

#define SIGMA 2.5
#define KAPPA 3

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

vec3 getSampleOffset(int s, float offset) {
	
	float a = (float(s) + 0.5) * (1.0 / SAMPLES);
	float theta = a * (TURNS * 6.28318) + offset;
	
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
	float V_dot_C_n = dot(V, C_n);
	
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
	float disk_rad = -500.0 * 1.0 / C_pos.z;
	
	float occlusion = 0.0;
	for (int s = 0; s < SAMPLES; s++)
		occlusion = occlusion + sampleAO(pixel_position, C_pos, C_n, disk_rad, s, rotation);
	
	occlusion = 1.0 - (occlusion * (2.0 * SIGMA / float(SAMPLES)));
	
	occlusion_out = vec3(occlusion);
	
}
