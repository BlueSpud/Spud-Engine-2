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

const vec3 lights[4] = vec3[](vec3(-1.0, 4.0, 1.0), vec3(-1.0, 1.0, 3.0), vec3(2.0, 1.0, -2.0), vec3(2.2, 1.5, 3.0));

/******************************************************************************
 *  Values that only need to be computed once per pixel                       *
 ******************************************************************************/

vec3 normal;

float NDH, NDV;
float roughness, inverse_roughness, metalic;

vec3 V;
float fresnel;

float G1(float NDV, float k) {
    
    float den = NDV * (1.0 - k) + k;
    return NDV / den;
    
}

float getDiffuseTerm(float NDL) {
    
    // Diffuse intensity uses the Lambertian reflectance model
    return -NDL * (1.0 - metalic) + 0.1;
    
}

float getSpecularTerm(vec3 L, float NDL) {
    
    // Get H
    vec3 H = normalize(L + V);
    
    // Calculate a few things with alpha
    float a = roughness * roughness;
    float a_sqrd = a * a;
    
    // Get G
    NDH = dot(normal, -H);
    float D_den = NDH * NDH * (a_sqrd - 1.0) + 1.0;
    D_den = D_den * D_den * 3.14159;
    
    float D = a_sqrd / D_den;
    
    // Get G
    float k = 2.0 / sqrt(3.14159 * (a + 2));
    float G = G1(NDL, k) * G1(NDV, k);
    
    return (fresnel * D * G) / (4.0 * NDL * NDV);
    
}



void main() {

    // Get albedo and if we're transparent, discard
    vec4 albedo_s = texture(tex_albedo, tex_coord0);
    if (albedo_s.w == 0)
        discard;
    
    vec3 albedo = albedo_s.xyz;
    
    // Get normal
    normal = normalize(texture(tex_normal, tex_coord0).xyz);
    
    // Get the PBR properties
    vec4 orm = texture(tex_orm, tex_coord0);
    roughness = orm.y;
    inverse_roughness = 1.0 - roughness;
    metalic = max(orm.z, 0.12);
    float occlusion = orm.x;
    
    // Get position from depth
    float depth = texture(tex_depth, tex_coord0).x * 2.0 - 1.0;
    vec4 position = vec4(tex_coord0 * 2.0 - 1.0, depth, 1.0);
    position = inverse_proj_view * position;
    position = position / position.w;
    
    vec3 L = normalize(-light_position);
    float NDL = dot(L, normal);
    
    // Compute V
    V = normalize(position.xyz - view_position);
    NDV = dot(normal, V);
    
    // Calculate fresnel term F
    float F0 = clamp(roughness, 0.05, 0.5);
    float fresnel_pow = pow(1.0 + NDV, fresnel_pow);
    fresnel = F0 + (1.0 - F0) * fresnel_pow;
    
    float att_acc;
    
    // Calculate lighting accumulation
    float diffuse_acc, specular_acc;
    
    for (int i = 0; i < 4; i++) {
     
        // Get L
        vec3 L = position.xyz - lights[i];
        
        // Get attenuation and then normalize the light vector
        float att = length(L) / 2.0;
        att = 1.0 / (att * att);
        
        L = normalize(L);
        
        // If attenuation is too low, why calcualte light?
        if (att > 0.01) {
        
            // Get NDL
            float NDL = dot(normal, L);
        
            // Accumulate the lighting
            diffuse_acc += getDiffuseTerm(NDL) * att;
            specular_acc += getSpecularTerm(L, NDL) * att;
            att_acc += att;
            
        }
        
    }
    
    // Get the reflection color
    vec3 reflection = reflect(-V, normal);
    vec3 reflection_color = textureLod(tex_cube, reflection, roughness * 7.0).xyz;
    
    vec3 fresnel_reflection = reflection_color * fresnel_pow * inverse_roughness;
    vec3 metalic_reflection = reflection_color * inverse_roughness * metalic * inverse_roughness;
    
    // Combine lighting and texture
    vec3 color = albedo * occlusion * (fresnel_reflection + metalic_reflection + diffuse_acc + specular_acc * roughness);
    
    out_color = vec4(color, 1.0);


}
