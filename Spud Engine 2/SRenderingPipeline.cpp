//
//  SRenderingPipeline.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SRenderingPipeline.hpp"

/******************************************************************************
 *  Implementation for default rendering pipeline                             *
 ******************************************************************************/

SRenderingPipeline::SRenderingPipeline(SViewport* _viewport_2D, SViewport3D* _viewport_3D) {
    
    // Save the viewports
    viewport_2D = _viewport_2D;
    viewport_3D = _viewport_3D;
    
    // Get the shader
    simple_shader = SResourceManager::getResource<SShader>(SPath("Shader/simple/simple_texture.glsl"));
    
    // Make the main framebuffer
    std::vector<SFramebufferAttatchment*> attatchments;
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, 0));
    final_framebuffer = new SFramebuffer(attatchments, viewport_2D->screen_size.x, viewport_2D->screen_size.y);
    
}

void SRenderingPipeline::finalizeRender(SFramebuffer* output_framebuffer) {
    
    // Unbind the framebuffer and draw
    if (output_framebuffer)
        output_framebuffer->bind();
    else
        SFramebuffer::unbind();
    
    // Reset to the regular 2D viewport
    glm::mat4 projection_matrix = SGL::getProjectionMatrix2D(*viewport_2D);
    SGL::setUpViewport(*viewport_2D);
    SGL::loadMatrix(projection_matrix, MAT_PROJECTION);
    SGL::clearMatrix(MAT_MODEL);
    SGL::clearMatrix(MAT_VIEW);
    
    // Bind the final texture, reset the simple shader
    simple_shader->bind();
    
    int zero = 0;
    simple_shader->bindUniform(&zero, "tex_albedo", UNIFORM_INT, 1);
    
    glActiveTexture(GL_TEXTURE0);
    final_framebuffer->bindTexture(0);
    
    SGL::renderRect(viewport_2D->screen_pos, viewport_2D->screen_size);
    
}

void SRenderingPipeline::runPostProcess(glm::mat4& view_matrix, glm::mat4& projection_matrix, int texture_bind_start) {
    
    // ** texture_space_start is where post-process passes can start to bind their own textures (GL_TEXTURE0 + texture_space_start) **
    // For each of the post processes, perform the render and then set the output
    SPostProcessPassData data;
    data.viewport_2D = viewport_2D;
    data.viewport_3D = viewport_3D;
    
    data.view_matrix = &view_matrix;
    data.projection_matrix = &projection_matrix;
    
    data.framebuffer = final_framebuffer;
    data.texture_bind_start = texture_bind_start;
    
    // For each post-processing pass, perform it
    // Actual process itself is reponsible for blending
    for (int i = 0; i < post_process_passes.size(); i++)
        post_process_passes[i]->render(data);
    
}

void SRenderingPipeline::addPostProcessPass(SPostProcessPass* pass) { post_process_passes.push_back(pass); }
