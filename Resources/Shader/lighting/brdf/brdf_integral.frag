#version 330

in vec2 tex_coord0;
out vec2 integral;

const float M_PI = 3.14159265359f;
const uint SAMPLES = 1024u;

float VanDerCorpus(uint bits) {
	
	// Do some crazy bit madness to get the sampling pattern
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	
	return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint N) { return vec2(float(i) / float(N), VanDerCorpus(i)); }

vec3 importanceSample(vec2 ham, vec3 normal, float roughness) {
	
	// Get the dot products and get sign from that
	float a = roughness * roughness;
	float phi = 2.0 * M_PI * ham.x;
	float dot_t = sqrt((1.0 - ham.y) / (1.0 + (a * a - 1.0) * ham.y));
	float sin_t = sqrt(1.0 - dot_t * dot_t);
	
	// Get the half vector
	vec3 H = vec3(cos(phi) * sin_t, sin(phi) * sin_t, dot_t);
	
	// Get tangent and bitangent
	vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, normal));
	vec3 bitangent = cross(normal, tangent);
	
	// Return the sample
	return normalize(tangent * H.x + bitangent * H.y + normal * H.z);
	
}

float GGX(float NDV, float roughness) {
	
	float k = roughness * roughness / 2.0;
	return NDV / (NDV * (1.0 - k) + k);
	
}

float smith(vec3 N, vec3 V, vec3 L, float roughness) {
	
	// Get G
	float NDV = max(dot(N, V), 0.0);
	float NDL = max(dot(N, L), 0.0);
	
	float g1 = GGX(NDV, roughness);
	float g2 = GGX(NDL, roughness);
	
	return g1 * g2;
	
}

vec2 computeIntegral(float NDV, float roughness) {
	
	vec3 V = vec3(sqrt(1.0 - NDV * NDV), 0.0, NDV);
	vec2 out_integral;
	
	// Assume normal
	vec3 normal = vec3(0.0, 0.0, 1.0);
	
	for (uint i = 0u; i < SAMPLES; i++) {
		
		vec2 ham = Hammersley(i, SAMPLES);
		vec3 H = importanceSample(ham, normal, roughness);
		vec3 L = normalize(2.0 * dot(normal, H) * H - V);
		
		// Get relevant dot products
		float NDL = max(L.z, 0.0);
		float NDH = max(H.z, 0.0);
		float VDH = max(dot(V, H), 0.0);
		
		if (NDL > 0.0) {
			
			// Calculate the values
			float G = smith(normal, V, L, roughness);
			float G_v = (G * VDH) / (NDH * NDV);
			float F = pow(1.0 - VDH, 5.0);
			
			out_integral = out_integral + vec2((1.0 - F) * G_v, F * G_v);
			
		}
		
	}
	
	return out_integral / SAMPLES;
	
}

void main() {
	
	// Output the texture
	integral = computeIntegral(tex_coord0.x, tex_coord0.y);
	
}
