#version 330

uniform sampler2D tex_albedo;
uniform sampler2D tex_orm;

uniform samplerCube tex_cube;

uniform vec3 view_position;

in vec3 position0;
in vec3 normal0;
in vec2 tex_coord0;

out vec4 out_color;

/******************************************************************************
 *  Lighting parameters                                                       *
 ******************************************************************************/

const float light_intensity = 1.0;
const vec3 light_color = vec3(1.0, 0.0, 0.0);
const vec3 light_position = vec3(2.0, 2.0, 2.0);

const float fresnel_pow = 5.0;

float lerp(float a, float b, float p) {
    
    return a * p + (1.0 - p) * b;
    
}

vec3 lerp(vec3 a, vec3 b, float p) {
    
    return a * p + (1.0 - p) * b;
    
}

float G1(float NDV, float k) {
    
    float den = NDV * (1.0 - k) + k;
    return NDV / den;
    
}

void main() {
    
    vec3 properties = texture(tex_orm, tex_coord0).xyz;
    float roughness = properties.y;
    float metalic = properties.z;
    
    vec3 L = normalize(-light_position);
    
    // Diffuse intensity uses the Lambertian reflectance model
    // I = L dot N * C * I
    float intensity_diffuse = dot(-L, normal0) * light_intensity + 0.1;
    
    // Calculate fresnel term (D)
    float F0 = clamp(roughness, 0.05, 1.0);
    vec3 V = normalize(position0 - view_position);
    float fresnel = F0 + (1.0 - F0) * pow(1.0 + dot(normal0, V), fresnel_pow);
    
    // Get H
    vec3 H = normalize(L + V);
    
    // Calculate a few things with alpha
    float inverse_roughness = 1.0 - roughness;
    float a = inverse_roughness * inverse_roughness;
    float a_sqrd = a * a;
    
    // Get G
    float NDH = dot(normal0, -H);
    float D_den = NDH * NDH * (a_sqrd - 1.0) + 1.0;
    D_den = D_den * D_den * 3.14159;
    
    float D = a_sqrd / D_den;
    
    // Get G
    float k = 2.0 / sqrt(3.14159 * (a + 2));
    float G = G1(dot(normal0, L), k) * G1(dot(normal0, V), k);
    
    // Combine specular
    float specular = (fresnel * G * D) / (dot(normal0, L) * dot(normal0, V));
    
    // Get the regular color
    vec3 tex_color = texture(tex_albedo, tex_coord0).xyz;
    
    // Get the reflection color
    vec3 reflection = reflect(-V, normal0);
    vec3 reflection_color = textureLod(tex_cube, reflection, int(metalic * 5.0)).xyz * tex_color;
    
    // Combine lighting and texture
    vec3 color = lerp(tex_color, reflection_color, roughness) * properties.x * (lerp(specular, intensity_diffuse, metalic));
    
    
    out_color = vec4(color, 1.0);
    
}
