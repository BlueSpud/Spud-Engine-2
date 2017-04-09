#version 330

uniform sampler2D tex_depth;
uniform sampler2D tex_albedo;
uniform sampler2D tex_normal;

uniform mat4 mat_view_scene;
uniform mat4 mat_inverse_proj;
uniform mat4 mat_proj;
uniform mat4 mat_view_proj;
uniform mat4 inverse_proj_view;

uniform vec3 view_position;

in vec2 tex_coord0;
out vec3 reflection;

float linearizeDepth(float depth) {
	
	depth = depth * 2.0 - 1.0;
	return (2.0 * 0.1 * 500.0) / (500.0 + 0.1 - depth * (500.0 - 0.1));
	
}


vec3 binarySearch(vec3 last, vec3 current) {
	
	vec3 ray_min = last;
	vec3 ray_max = current;
	vec3 ray_mid;
	
	for (int i = 0; i < 10; i++) {
		
		ray_mid = mix(ray_min, ray_max, 0.5);
		float depth = texture(tex_depth, ray_mid.xy).r;
		
		if (ray_mid.z > depth)
			ray_max = ray_mid;
		else ray_min = ray_max;
		
	}
	
	return ray_mid;
	
}

void main() {

	// Get the normal
	vec3 normal = texture(tex_normal, tex_coord0).rgb;
	
	// Calculate world space position
	float depth = texture(tex_depth, tex_coord0).x;
	vec4 position_p = vec4(tex_coord0 * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	position_p = inverse_proj_view * position_p;
	vec3 position = (position_p / position_p.w).xyz;

	// Get the reflection vector
	vec3 V = normalize(position - view_position);
	vec3 reflection_vector = reflect(V, normal);
	
	vec4 reflection_pos = mat_view_proj * vec4(position + reflection_vector, 1.0);
	reflection_pos = reflection_pos / reflection_pos.w;
	vec3 reflection_pos_s = reflection_pos.xyz * 0.5 + 0.5;
	
	// Calculate a jitter to add some noise
	float c = (tex_coord0.x * 14400.0 + tex_coord0.y * 8500.0) * 0.25;
	float jitter = mod( c, 1.0);
	
	vec3 NDC = vec3(tex_coord0, depth);
	vec3 ray_direction = normalize(reflection_pos_s - NDC);
	
	vec3 last_ray_step = NDC;
	vec3 reflection_color;
	float depth_diff;
	
	vec3 final_pos;
	
	for (int i = 0; i < 32; i++) {
	
		vec3 ray_step = last_ray_step + ray_direction * 0.02;
		float depth_ray = texture(tex_depth, ray_step.xy).r;
	
		if (depth_ray < ray_step.z) {
			
			final_pos = binarySearch(last_ray_step, ray_step);
			
			// Get difference in depth
			depth_diff = linearizeDepth(final_pos.z) - linearizeDepth(depth_ray);
			
			reflection_color = texture(tex_albedo, final_pos.xy).rgb;
			break;
			
		}
		
		last_ray_step = ray_step;
		
	}
	
	if (depth_diff > 0.2 || length(clamp(final_pos.xy, 0.0, 1.0) - final_pos.xy) > 0.0)
		reflection_color = vec3(0.0);
	
	reflection = reflection_color * (1.0 - smoothstep(0.0, 1.0, dot(-V, ray_direction))) * smoothstep(-0.17, 0.0, dot(normal, reflection_vector));;

}
