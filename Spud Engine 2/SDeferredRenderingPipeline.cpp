//
//  SDeferredRenderingPipeline.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/21/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SDeferredRenderingPipeline.hpp"

SDeferredRenderingPipleline::SDeferredRenderingPipleline(SViewport* _viewport_2D, SViewport3D* _viewport_3D) : SRenderingPipeline(_viewport_2D, _viewport_3D) {
    
    // Create the gbuffer and its attatchments
    std::vector<SFramebufferAttatchment*> attatchments;
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_DEPTH, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT, GBUFFER_DEPTH));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GBUFFER_ALBEDO));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GBUFFER_NORMAL));
    
    gbuffer = new SFramebuffer(attatchments, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2);
    
    std::cout << "Construction\n";
    
    // Get the lighting shader
    lit_shader = (SShader*)SResourceManager::getResource(SPath("Shader/Simple.glsl"));
    
}

void SDeferredRenderingPipleline::unload() {
    
    // Unload the gbuffer
    gbuffer->unload();
    delete gbuffer;
    
}

void SDeferredRenderingPipleline::render(double interpolation, SCamera& camera, SSceneGraph& scene_graph) {
    
    // Render the 3D scene into the GBuffer
    gbuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 projection_matrix = SGL::getProjectionMatrix3D(*viewport_3D);
    SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
    
    SGL::setUpViewport(*viewport_3D);
    
    // The 3D world has culling enabled
    glEnable(GL_CULL_FACE);
    
    scene_graph.render(camera, interpolation);
    
    // Render the lit buffer to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up the new viewport
    SGL::setUpViewport(*viewport_2D);
    projection_matrix = SGL::getProjectionMatrix2D(*viewport_2D);
    SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
    SGL::clearMatrix(MAT_VIEW_MATRIX);
    
    // No culling for this stage
    glDisable(GL_CULL_FACE);
    
    lit_shader->bind();
    
    // Bind the texture locations
    int texture = GBUFFER_DEPTH;
    lit_shader->bindUniform(&texture, "tex_depth", UNIFORM_INT, 1);
    
    texture = GBUFFER_ALBEDO;
    lit_shader->bindUniform(&texture, "tex_albedo", UNIFORM_INT, 1);
    
    texture = GBUFFER_NORMAL;
    lit_shader->bindUniform(&texture, "tex_normal", UNIFORM_INT, 1);
    
    // Bind the textures and then render
    glActiveTexture(GL_TEXTURE0);
    gbuffer->bindTexture(GBUFFER_DEPTH);
    glActiveTexture(GL_TEXTURE1);
    gbuffer->bindTexture(GBUFFER_ALBEDO);
    glActiveTexture(GL_TEXTURE2);
    gbuffer->bindTexture(GBUFFER_NORMAL);
    
    SGL::drawRect(glm::vec2(0, 0), glm::vec2(WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2));

    
    
}
