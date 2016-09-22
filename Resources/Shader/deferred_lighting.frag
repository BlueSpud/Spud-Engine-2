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

const float light_intensity = 1.0;
const vec3 light_color = vec3(1.0, 0.0, 0.0);
const vec3 light_position = vec3(2.0, 2.0, 2.0);

const float fresnel_pow = 5.0;

float G1(float NDV, float k) {
    
    float den = NDV * (1.0 - k) + k;
    return NDV / den;
    
}

void main() {

    vec4 albedo_s = texture(tex_albedo, tex_coord0);
    if (albedo_s.w == 0)
        discard;
    
    vec3 albedo = albedo_s.xyz;
    
    // Get the PBR properties
    vec4 orm = texture(tex_orm, tex_coord0);
    float roughness = orm.y;
    float inverse_roughness = 1.0 - roughness;
    float metalic = max(orm.z, 0.12);
    float occlusion = orm.x;
    
    vec3 normal = texture(tex_normal, tex_coord0).xyz;
    
    // Get position from depth
    float depth = texture(tex_depth, tex_coord0).x * 2.0 - 1.0;
    vec4 position = vec4(tex_coord0 * 2.0 - 1.0, depth, 1.0);
    position = inverse_proj_view * position;
    position = position / position.w;
    
    vec3 L = normalize(-light_position);
    
    // Diffuse intensity uses the Lambertian reflectance model
    float diffuse = dot(-L, normal) * light_intensity * (1.0 - metalic) + 0.1;
    
    // Calculate fresnel term F
    float F0 = clamp(roughness, 0.05, 0.5);
    vec3 V = normalize(position.xyz - view_position);
    
    float fresnel_pow = pow(1.0 - dot(normal, -V), fresnel_pow);
    float fresnel = F0 + (1.0 - F0) * fresnel_pow;
    
    // Get H
    vec3 H = normalize(L + V);
    
    // Calculate a few things with alpha
    float a = roughness * roughness;
    float a_sqrd = a * a;
    
    // Get G
    float NDH = dot(normal, -H);
    float D_den = NDH * NDH * (a_sqrd - 1.0) + 1.0;
    D_den = D_den * D_den * 3.14159;
    
    float D = a_sqrd / D_den;
    
    // Get G
    float k = 2.0 / sqrt(3.14159 * (a + 2));
    float G = G1(dot(normal, L), k) * G1(dot(normal, V), k);
    
    // Combine specular
    float specular = (fresnel * G * D) / (4 * dot(normal, L) * dot(normal, V));
    
    // Get the reflection color
    vec3 reflection = reflect(-V, normal);
    vec3 reflection_color = textureLod(tex_cube, reflection, roughness * 7.0).xyz;
    
    vec3 fresnel_reflection = reflection_color * fresnel_pow * inverse_roughness;
    vec3 metalic_reflection = reflection_color * inverse_roughness * metalic;
    
    // Combine lighting and texture
    vec3 color = albedo * occlusion * (fresnel_reflection + metalic_reflection + diffuse + specular * metalic);
    
    out_color = vec4(color, 1.0);


}
