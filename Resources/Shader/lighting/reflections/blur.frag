#version 330

uniform sampler2D tex_albedo;
uniform vec2 screen_size;

in vec2 tex_coord0;
out vec3 albedo;

void main() {

    // Output the texture
	ivec2 pixel = ivec2(tex_coord0 * screen_size);
	
	vec3 sum = texelFetch(tex_albedo, pixel, 0).rgb;
	
	if (length(sum) == 0.0) {
		
		albedo = vec3(0.0);
		return;
		
	}
	
	sum = sum + texelFetch(tex_albedo, pixel + ivec2(0.0, 1.0), 0).rgb;
	sum = sum + texelFetch(tex_albedo, pixel + ivec2(0.0, -1.0), 0).rgb;
	sum = sum + texelFetch(tex_albedo, pixel + ivec2(1.0, 0.0), 0).rgb;
	sum = sum + texelFetch(tex_albedo, pixel + ivec2(-1.0, 0.0), 0).rgb;

	albedo = sum / 5.0;
	
}
