#version 330

uniform sampler2D tex_depth;
uniform sampler2D tex_albedo;
uniform sampler2D tex_normal;
uniform sampler2D tex_orm;
uniform samplerCube tex_cube;
uniform sampler2D tex_shadow;
uniform sampler2D tex_brdf;
uniform sampler2D tex_ssr;

uniform vec3 view_position;
uniform mat4 inverse_proj_view;
uniform vec2 screen_size;

out vec4 out_color;

/******************************************************************************
 *  Lighting constants                                                        *
 ******************************************************************************/

#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_DIRECTIONAL 2
#define LIGHT_TYPE_SPOT 3

/******************************************************************************
 *  Lighting parameters                                                       *
 ******************************************************************************/

const float fresnel_pow = 5.0;

uniform vec3 light_positions[64];
uniform vec4 light_params[64];
uniform vec2 light_infos[64];
uniform vec4 spot_data[64];
uniform int light_types[64];

uniform mat4 light_matrices[64];
uniform vec2 shadow_map_coordinates[64];
uniform int lights_shadow[64];


// Tile shading
uniform int light_indicies[64];
uniform int light_count;

const float tile_step = 1.0 / 8.0;

/******************************************************************************
 *  Values that only need to be computed once per pixel                       *
 ******************************************************************************/
float NDH, NDV;
float roughness, inverse_roughness, metalic;

vec3 V;
float fresnel;

float depth;
vec3 position;
vec3 normal;

vec3 lerp(vec3 a, vec3 b, float percent) {
	
	return a * percent + b * (1.0 - percent);
	
}

vec3 getLightVector(int light, vec3 position) {
	
	switch(light_types[light]) {
			
		case LIGHT_TYPE_DIRECTIONAL:
			return -light_positions[light];
			break;
			
		default:
			return position - light_positions[light];
			break;
			
	}
	
}

float getAttPoint(float dist) {
	
	float second_pow = pow(dist, 2);
	float fourth_pow = pow(second_pow, 2);
	return pow(clamp(1.0 - fourth_pow, 0.0, 1.0), 2) / (second_pow + 1.0);
	
}

float getAtt(int light, vec3 L) {
	
	switch(light_types[light]) {
			
		case LIGHT_TYPE_DIRECTIONAL:
			return 1.0;
			break;
			
		case LIGHT_TYPE_POINT:
			
			//float att = length(L) * 2.0;
			//return clamp(0.5 / (att * att + att), 0.0, 1.0);
			
			// Epic's falloff algorithm from https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
			float scaled_dist = length(L) / light_params[light].w;
			
			return getAttPoint(scaled_dist);
			
			break;
			
		case LIGHT_TYPE_SPOT:
			
			float att_spot = 0.0;
			int spot_index = int(light_infos[light].y);
			
			// Check if this pixel is inside the spot light
			float spot_dot = dot(spot_data[spot_index].xyz, normalize(L));
			float spot_scaled_dist = length(L) / light_params[light].w;
			
			if (spot_dot > spot_data[spot_index].w) {
				
				att_spot = getAttPoint(spot_scaled_dist);
				
			} else {
				
				// Dot product was further than cutoff
				float fade = clamp(1.0 - (spot_data[spot_index].w - spot_dot) * 15.0, 0.0, 1.0);
				att_spot = getAttPoint(spot_scaled_dist) * fade;
				
			}
			
			return att_spot;
			
			break;
			
	}
	
}

float getVSM(float pos, vec2 moments) {
	
	float p = step(pos, moments.x);
	float variance = max(moments.y - moments.x * moments.x, 0.00002);
	
	float d = (moments.x - pos);
	float p_max = smoothstep(0.25, 1.0, variance / (variance + d * d));
	
	return min(max(p, p_max), 1.0);
	
}

float getShadowTerm(int matrix, vec3 L) {
	
	// Get the position in the shadow map
	vec4 position_shadow = light_matrices[matrix] * vec4(position, 1.0);
	position_shadow = position_shadow / position_shadow.w;
	
	if (length(clamp(position_shadow.xy, 0.0, 1.0) - position_shadow.xy) > 0.0)
		return 1.0;
	
	// Calculate the texture coordinates based off of the shadow atlas
	vec2 tex_coord_shadow = position_shadow.xy / 8.0 + vec2(tile_step) * shadow_map_coordinates[matrix];
	vec2 moments = texture(tex_shadow, tex_coord_shadow).xy;
	
	return getVSM(position_shadow.z, moments);
	
}

float G1(float NDV, float k) {
	
	float den = NDV * (1.0 - k) + k;
	return NDV / den;
	
}

float getDiffuseTerm(float NDL) {
	
	// Diffuse intensity uses the Lambertian reflectance model, ambient is added here
	return clamp(-NDL, 0.0, 1.0);
	
}

float getSpecularTerm(vec3 L, float NDL) {
	
	// Get H
	vec3 H = normalize(L + V);
	
	// Calculate a few things with alpha
	float a = roughness * roughness;
	float a_sqrd = a * a;
	
	// Get G
	NDH = dot(normal, H);
	float D_den = NDH * NDH * (a_sqrd - 1.0) + 1.0;
	D_den = D_den * D_den * 3.14159;
	
	float D = a_sqrd / D_den;
	
	// Get G
	float k = 2.0 / sqrt(3.14159 * (a + 2.0));
	float G = G1(NDL, k) * G1(NDV, k);
	
	return clamp((fresnel * D * G) / (4.0 * NDL * NDV), 0.0, 1.0);
	
}

void main() {
	
	// Calculate the texture coordinate
	vec2 tex_coord0 = (gl_FragCoord.xy) / screen_size;
	
	// Get albedo and if we're transparent, discard
	vec4 albedo_s = texture(tex_albedo, tex_coord0);
	
	vec3 albedo = albedo_s.xyz;
	
	// Get normal
	normal = texture(tex_normal, tex_coord0).xyz;
	
	// Get the PBR properties
	vec4 orm = texture(tex_orm, tex_coord0);
	roughness = clamp(orm.y, 0.04, 0.96);
	inverse_roughness = 1.0 - roughness;
	metalic = orm.z;
	float inverse_metalic = 1.0 - metalic;
	
	// Get position from depth
	depth = texture(tex_depth, tex_coord0).x;
	vec4 position_p = vec4(tex_coord0 * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	position_p = inverse_proj_view * position_p;
	position = (position_p / position_p.w).xyz;
	
	// Compute V
	V = normalize(position.xyz - view_position);
	NDV = dot(normal, V);
	
	if (albedo_s.w == 0) {
		
		// Skybox
		vec3 reflection = reflect(-V, normal);
		out_color = textureLod(tex_cube, vec3(reflection.x, reflection.y, -reflection.z), 5.0);
		return;
		
	}
	
	// Calculate fresnel term F
	float F0 = clamp(metalic, 0.05, 1.0);
	float fresnel_pow = pow(1.0 + NDV, fresnel_pow);
	fresnel = clamp(F0 + (1.0 - F0) * fresnel_pow, 0.0, 1.0);
	
	// Storage lighting accumulation
	vec3 diffuse_acc, specular_acc;
	
	// Save how many lights that we shadowed, arrays for shadow mapping only use shadow mapped lights
	int shadow_light = 0;
	
	for (int i = 0; i < light_count; i++) {
		
		// Get L
		vec3 L = getLightVector(light_indicies[i], position.xyz);
		
		float shadow = 1.0;
		float att = getAtt(light_indicies[i], L);
		
		if (lights_shadow[light_indicies[i]] == 1) {
			
			shadow = getShadowTerm(shadow_light, L);
			
			shadow_light++;
			
		}
		
		// Make a threshhold for attenuation to be over to actually calculate light
		att = att * shadow;
		if (att > 0.001) {
			
			// Get NDL
			L = normalize(L);
			float NDL = dot(normal, L);
			
			// Accumulate the lighting
			diffuse_acc += light_params[light_indicies[i]].xyz * getDiffuseTerm(NDL) * att;
			specular_acc += light_params[light_indicies[i]].xyz * getSpecularTerm(L, NDL) * att;
			
		}
		
	}
	
	// Calculate how much the specular and the diffuse will be blended to satisfy diffuse + specular <= 1
	float term_blend = metalic + inverse_roughness * inverse_metalic / 2.0 + 0.08;
	
	// Colors
	vec3 specular_color = mix(vec3(1.0), albedo, metalic);
	vec3 fresnel_color = mix(albedo, vec3(1.0), metalic * 0.5);
	
	// Get the reflection color
	vec3 reflection = reflect(-V, normal);
	
	// Mip map selection is done with a cube root
	float reflection_mip_map = sqrt(roughness) * 12.0;
	vec3 reflection_color = textureLod(tex_cube, vec3(reflection.x, reflection.y, -reflection.z), reflection_mip_map).xyz;
	
//	vec3 ssr = texture(tex_ssr, tex_coord0).rgb;
//	if (length(ssr) > 0.0)
//		reflection_color = ssr;
//	
	vec2 env_spec = texture(tex_brdf, vec2(max(-NDV, 0.0), roughness)).rg;
	
	// Calculate ambient color from the blurriest mipmap
	vec3 ambient_color = textureLod(tex_cube, -normal, 12).xyz;
	vec3 ambient = ambient_color * albedo * ((1.0 - fresnel) * inverse_metalic);
	ambient = ambient + specular_color * reflection_color * (fresnel * env_spec.x + env_spec.y);
	
	// Combine lighting and texture
	vec3 color = mix(albedo * diffuse_acc, specular_acc * specular_color, term_blend) + ambient * orm.x;
	//color = pow(color, vec3(2.0 / 2.2));
	
	out_color = vec4(color, 1.0);
}
