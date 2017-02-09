#version 330

uniform sampler2D tex_depth;
uniform sampler2D tex_albedo;
uniform sampler2D tex_normal;
uniform sampler2D tex_orm;
uniform samplerCube tex_cube;

uniform vec3 view_position;
uniform mat4 inverse_proj_view;

in vec2 tex_coord0;

out vec4 out_color;

/******************************************************************************
 *  Lighting parameters                                                       *
 ******************************************************************************/

const float fresnel_pow = 5.0;

uniform int light_count;
uniform vec3 light_positions[64];
uniform vec3 light_colors[64];
uniform mat4 light_matrices[64];
uniform vec2 shadow_map_coordinates[64];
uniform int lights_shadow[64];

const float tile_step = 1.0 / 8.0;

uniform sampler2D tex_shadow;

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

float getShadowTerm(int matrix, vec3 L) {
    
    // Get the position in the shadow map
    vec4 position_shadow = light_matrices[matrix] * vec4(position + normal * 0.04, 1.0);
    position_shadow = position_shadow / position_shadow.w;

    // Calculate the texture coordinates based off of the shadow atlas
    vec2 tex_coord_shadow = position_shadow.xy / 8.0 + vec2(tile_step, tile_step) * shadow_map_coordinates[matrix];
    float z = texture(tex_shadow, tex_coord_shadow).r;

    // Figure out if we are outside of the shadow map
    float outside = 1.0 - length(clamp(position_shadow.xy, 0.0, 1.0) - position_shadow.xy);
    
    return step(position_shadow.z * outside, z);


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

    // Get albedo and if we're transparent, discard
    vec4 albedo_s = texture(tex_albedo, tex_coord0);
    if (albedo_s.w == 0)
        discard;

    vec3 albedo = albedo_s.xyz;

    // Get normal
    normal = texture(tex_normal, tex_coord0).xyz;

    // Get the PBR properties
    vec4 orm = texture(tex_orm, tex_coord0);
    roughness = orm.y;
    inverse_roughness = 1.0 - roughness;
    metalic = orm.z;

    // Get position from depth
    depth = texture(tex_depth, tex_coord0).x;
    vec4 position_p = vec4(tex_coord0 * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    position_p = inverse_proj_view * position_p;
    position = (position_p / position_p.w).xyz;

    // Compute V
    V = normalize(position.xyz - view_position);
    NDV = dot(normal, V);

    // Calculate fresnel term F
    float F0 = clamp(metalic, 0.05, 1.0);
    float fresnel_pow = pow(1.0 + NDV, fresnel_pow);
    fresnel = F0 + (1.0 - F0) * fresnel_pow;

    // Storage lighting accumulation
    vec3 diffuse_acc, specular_acc;

    // Save how many lights that we shadowed, arrays for shadow mapping only use shadow mapped lights
    int shadow_light = 0;

    for (int i = 0; i < light_count; i++) {

        // Get L
        vec3 L;

        float shadow = 1.0;
        float att = 1.0;

        if (lights_shadow[i] == 1) {

            // Right now all thats supported is directional lights
            L = -light_positions[i];
            
            shadow = getShadowTerm(shadow_light, L);

            shadow_light++;

        } else {

          // Standard point light
          L = position.xyz - light_positions[i];

          // Get attenuation and then normalize the light vector
          att = length(L);
          clamp(att = 1.0 / (att * att), 0.0, 1.0);

        }

        // Make a threshhold for attenuation to be over to actualyl calculate light
        att = att * shadow;
        if (att > 0.01) {

            // Get NDL
            L = normalize(L);
            float NDL = dot(normal, L);

            // Accumulate the lighting
            diffuse_acc += light_colors[i] * getDiffuseTerm(NDL) * att;
            specular_acc += light_colors[i] * getSpecularTerm(L, NDL) * att;

        }

    }

    // Get the reflection color
    vec3 reflection = reflect(-V, normal);
    
    // Mip map selection is done with a cube root
    float reflection_mip_map = (pow(roughness, 0.3333333)) * 12.0;
    vec3 reflection_color = textureLod(tex_cube, reflection, reflection_mip_map).xyz;

    vec3 fresnel_reflection = reflection_color * fresnel_pow * inverse_roughness;
    vec3 metalic_reflection = reflection_color * metalic;

    // Combine lighting and texture
    vec3 color = albedo * (lerp(specular_acc, diffuse_acc, metalic * inverse_roughness) + 0.3 + metalic_reflection) + fresnel_reflection * inverse_roughness;
    
    out_color = vec4(color, 1.0);


}
