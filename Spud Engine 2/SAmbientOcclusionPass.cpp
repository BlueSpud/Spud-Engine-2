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
SShader* SAmbientOcclusionPass::blur_shader;



/******************************************************************************
 *  Implementation for the ambient occlusion pass                             *
 ******************************************************************************/

SAmbientOcclusionPass::SAmbientOcclusionPass(glm::vec2 main_framebuffer_size) : viewport(main_framebuffer_size, glm::vec2(0)) {
    
    // Get the shader for an ambient occlusion pass if we dont have it
    if (!ambient_occlusion_shader) {
        
        ambient_occlusion_shader = SResourceManager::getResource<SShader>(SPath("Shader/ambient_occlusion/ambient_occlusion.glsl"));
        blur_shader = SResourceManager::getResource<SShader>(SPath("Shader/ambient_occlusion/blur.glsl"));
    
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
        
            // CURRENTLY NOT SCALING KERNAL, SEEMS TO LOOK BETTER WITHOUT IT
            // Scale the point so we get more samples closer to the origin
            //float scale = (float)i / (float)AO_KERNAL_SIZE;
            //scale = glm::mix(0.1, 1.0, scale * scale);
        
            
            //kernel[i] = kernel[i] * scale;
        
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
    
    
    // Create the framebuffer for ambient occlusion
    std::vector<SFramebufferAttatchment*> attatchments_occlusion;
    attatchments_occlusion.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RED, GL_RED, GL_UNSIGNED_INT, 0));
    
    occlusion_framebuffer = new SFramebuffer(attatchments_occlusion, viewport.screen_size.x, viewport.screen_size.y);
    
    // Create the framebuffer for the blur
    std::vector<SFramebufferAttatchment*> attatchments_blur;
    attatchments_blur.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RED, GL_RED, GL_UNSIGNED_INT, 0));
    
    blur_framebuffer = new SFramebuffer(attatchments_blur, viewport.screen_size.x / 2.0, viewport.screen_size.y / 2.0);
    
}

void SAmbientOcclusionPass::renderAmbientOcclusion(int depth, int normal, int to_place_noise,
                                                   SViewport3D& viewport_3D,
                                                   glm::mat4& projection_matrix,
                                                   glm::mat4& inv_projection_matrix,
                                                   glm::mat4& view_matrix) {
    
    // Set the viewport up
    SGL::setUpViewport(viewport);
    glm::mat4 projection_matrix_2D = SGL::getProjectionMatrix2D(viewport);
    SGL::loadMatrix(projection_matrix_2D, MAT_PROJECTION_MATRIX);
    
    // Bind the ambient occlusion framebuffer and get it ready
    occlusion_framebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    
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
    
    // Let the shader know how far to step for the noise based on the resolution and noise size
    int noise_size = sqrt(AO_NOISE_SIZE);
    glm::vec2 tex_coord_scale = viewport.screen_size / (float)noise_size;
    ambient_occlusion_shader->bindUniform(&tex_coord_scale, "tex_coord_scale", UNIFORM_VEC2, 1);
    
    // Bind the kernel size
    int kernel_size = AO_KERNAL_SIZE;
    ambient_occlusion_shader->bindUniform(&kernel_size, "kernel_size", UNIFORM_INT, 1);
    
    // Bind the near and far planes
    ambient_occlusion_shader->bindUniform(&viewport_3D.planes, "planes", UNIFORM_VEC2, 1);

    // Bind the matrices
    ambient_occlusion_shader->bindUniform(&view_matrix, "mat_view_scene", UNIFORM_MAT4, 1);
    ambient_occlusion_shader->bindUniform(&projection_matrix, "mat_projection_scene", UNIFORM_MAT4, 1);
    ambient_occlusion_shader->bindUniform(&inv_projection_matrix, "inv_proj", UNIFORM_MAT4, 1);
    
    SGL::renderRect(glm::vec2(0), viewport.screen_size);
    
    // Blur the framebuffer
    blur_framebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Bind the shader
    blur_shader->bind();
    
    // Bind the texture and its location
    blur_shader->bindUniform(&to_place_noise, "tex_occlusion", UNIFORM_INT, 1);
    occlusion_framebuffer->bindTexture(0);
    
    // Bind the data we need to blur
    blur_shader->bindUniform(&viewport.screen_size, "occlusion_size", UNIFORM_VEC2, 1);
    
    blur_shader->bindUniform(&noise_size, "noise_size", UNIFORM_INT, 1);
    
    // Get the viewport ready for the blur, done at 1/2 occlusion resolution
    viewport.screen_size = viewport.screen_size / 2.0f;
    SGL::setUpViewport(viewport);
    projection_matrix_2D = SGL::getProjectionMatrix2D(viewport);
    SGL::loadMatrix(projection_matrix_2D, MAT_PROJECTION_MATRIX);
    
    SGL::renderRect(glm::vec2(0), viewport.screen_size);
    
    // Reset the viewport
    viewport.screen_size = viewport.screen_size * 2.0f;
    
}

void SAmbientOcclusionPass::bindAmbientOcclusionTexture(int texture) {
    
    glActiveTexture(GL_TEXTURE0 + texture);
    blur_framebuffer->bindTexture(0);
    
}

void SAmbientOcclusionPass::unload() {
    
    // Unload the framebuffer and delete it
    occlusion_framebuffer->unload();
    blur_framebuffer->unload();
    
    delete occlusion_framebuffer;
    delete blur_framebuffer;
    
}
