#version 330

uniform sampler2D tex_albedo;

uniform vec3 view_position;

in vec3 position_world;
in vec3 position_view;
in vec3 normal0;
in vec2 tex_coord0;

out vec4 out_color;

/******************************************************************************
 *  Lighting parameters                                                       *
 ******************************************************************************/

const float light_intensity = 1.0;
const vec3 light_color = vec3(1.0, 0.0, 0.0);
const vec3 light_position = vec3(2.0, 2.0, 2.0);

const float fresnel_pow = 0.3;

// TEMP
const float roughness = 0.5;
const float metalic = 1.0;

float lerp(float a, float b, float p) {
    
    return a * p + (1.0 - p) * b;
    
}

float G1(float NDV, float k) {
    
    float den = NDV * (1.0 - k) + k;
    return NDV / den;
    
}

void main() {
    
    vec3 L = normalize(position_world - light_position);
    
    // Diffuse intensity uses the Lambertian reflectance model
    // I = L dot N * C * I
    float intensity_diffuse = dot(-L, normal0) * light_intensity + 0.1;
    
    // Calculate fresnel term (D)
    vec3 V = normalize(position_world - view_position);
    float fresnel = pow(1 + dot(normal0, V), fresnel_pow);
    
    // Get H
    vec3 H = normalize(L + V);
    
    float a = 1.0 - roughness;
    a = a * a;
    float a_sqrd = a * a;
    
    float NDH = dot(normal0, H);
    float D_den = NDH * NDH * (a_sqrd - 1.0) + 1.0;
    D_den = D_den * D_den * 3.14159;
    
    float D = a_sqrd / D_den;
    
    float k = 2.0 / sqrt(3.14159 * (roughness * roughness + 2));
    float G = G1(dot(normal0, -L), k) * G1(dot(normal0, -V), k);
    
    
    float specular = fresnel * G * D * 4.0;
    
    // Combine lighting
    vec3 color = vec3(1.0, 1.0, 1.0) * (lerp(specular, intensity_diffuse, metalic + 0.08));
    
    out_color = vec4(color, 1.0);
    
}
