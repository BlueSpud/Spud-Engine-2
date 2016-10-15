//
//  SAmbientOcclusionPass.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SAmbientOcclusionPass.hpp"
glm::vec3 SAmbientOcclusionPass::kernel[AO_KERNAL_SIZE];
GLuint SAmbientOcclusionPass::noise_texture_id;
SShader* SAmbientOcclusionPass::ambient_occlusion_shader;



/******************************************************************************
 *  Functions for the ambient occlusion pass                                  *
 ******************************************************************************/

SAmbientOcclusionPass::SAmbientOcclusionPass(glm::ivec2 size) : viewport(size, glm::vec2(0.0)) {
    
    // Get the shader for an ambient occlusion pass if we dont have it
    if (!ambient_occlusion_shader) {
        
        ambient_occlusion_shader = (SShader*)SResourceManager::getResource(SPath("Shader/ambient_occlusion.glsl"));
    
        // Create a random engine for the kernal and noise
        std::default_random_engine random_engine;
        std::uniform_real_distribution<float> random_distribution_xy(-1.0, 1.0);
        std::uniform_real_distribution<float> random_distribution_z(0.0, 1.0);
    
        // Create a kernal for the ambient occlusion
        for (int i = 0; i < AO_KERNAL_SIZE; i++) {
        
            // Make the value random
            kernel[i] = glm::normalize(glm::vec3(random_distribution_xy(random_engine),
                                                 random_distribution_xy(random_engine),
                                                 random_distribution_z(random_engine)));
        
            // Scale the point so we get more samples closer to the origin
            float scale = (float)i / (float)AO_KERNAL_SIZE;
            scale = glm::mix(0.1, 1.0, scale * scale);
        
            kernel[i] = kernel[i] * scale;
        
        }
    
        // Generate the data for a noise texture
        glm::vec2* noise_data = new glm::vec2[AO_NOISE_SIZE];
    
        for (int i = 0; i < AO_NOISE_SIZE; i++)
            noise_data[i] = glm::normalize(glm::vec2(random_distribution_xy(random_engine), random_distribution_xy(random_engine)));
        
        // Create a noise texture
        glGenTextures(1, &noise_texture_id);
        glBindTexture(GL_TEXTURE_2D, noise_texture_id);
        
        int noise_size = sqrt(AO_NOISE_SIZE);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, noise_size, noise_size, 0, GL_RG, GL_FLOAT, &noise_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        delete [] noise_data;
    
    }
    
    
    // Create a framebuffer
    std::vector<SFramebufferAttatchment*> attatchments;
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RED, GL_RED, GL_UNSIGNED_INT, 0));
    
    framebuffer = new SFramebuffer(attatchments, size.x, size.y);
    
}

void SAmbientOcclusionPass::renderAmbientOcclusion(int depth, int normal, int to_place_noise, glm::ivec2 screen_size,
                                                   glm::mat4& projection_matrix,
                                                   glm::mat4& inv_projection_matrix,
                                                   glm::mat4& view_matrix) {
    
    // Bind the ambient occlusion framebuffer
    //framebuffer->bind();
    
    // Use the shader
    ambient_occlusion_shader->bind();
    
    ambient_occlusion_shader->bindUniform(&depth, "tex_depth", UNIFORM_INT, 1);
    ambient_occlusion_shader->bindUniform(&normal, "tex_normal", UNIFORM_INT, 1);
    ambient_occlusion_shader->bindUniform(&to_place_noise, "tex_noise", UNIFORM_INT, 1);
    
    // Bind the noise texture
    glActiveTexture(GL_TEXTURE0 + to_place_noise);
    glBindTexture(GL_TEXTURE_2D, noise_texture_id);
    
    // Bind the kernel
    ambient_occlusion_shader->bindUniform(&kernel, "kernel", UNIFORM_VEC3, AO_KERNAL_SIZE);

    // Bind the matrices
    ambient_occlusion_shader->bindUniform(&view_matrix, "mat_view_scene", UNIFORM_MAT4, 1);
    ambient_occlusion_shader->bindUniform(&projection_matrix, "mat_projection_scene", UNIFORM_MAT4, 1);
    ambient_occlusion_shader->bindUniform(&inv_projection_matrix, "inv_proj", UNIFORM_MAT4, 1);
    
    SGL::drawRect(glm::vec2(0), viewport.screen_size);
    
}

void SAmbientOcclusionPass::unload() {
    
    // Unload the framebuffer and delete it
    framebuffer->unload();
    delete framebuffer;
    
}
