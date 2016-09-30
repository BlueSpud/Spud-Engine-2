//
//  SDeferredRenderingPipeline.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/21/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SDeferredRenderingPipeline.hpp"

/******************************************************************************
 * Functions for deferred rendering pipeline                                  *
 ******************************************************************************/

SDeferredRenderingPipleline::SDeferredRenderingPipleline(SViewport* _viewport_2D, SViewport3D* _viewport_3D) : SRenderingPipeline(_viewport_2D, _viewport_3D) {
    
    // Create the gbuffer and its attatchments
    std::vector<SFramebufferAttatchment*> attatchments;
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_DEPTH, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT, GBUFFER_DEPTH));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GBUFFER_ALBEDO));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_FLOAT, GBUFFER_NORMAL));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GBUFFER_ORM));
    
    gbuffer = new SFramebuffer(attatchments, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2);
    
    // Get the cube map
    environment_map =  (SCubeMap*)SResourceManager::getResource(SPath("Texture/room.cube"));
    
    // Get the lighting shader
    lit_shader = (SShader*)SResourceManager::getResource(SPath("Shader/deferred_lighting.glsl"));
    simple_shader = (SShader*)SResourceManager::getResource(SPath("Shader/simple.glsl"));
    
    // Get the view pos
    view_pos_u = SUniformManger::instance()->getUniformWithName("view_position");
    
    // Create a temporary light
    light = new SPointLight();
    light->transform.translation.y = 2.0;
    light->transform.translation.z = 4.0;
    
    light_graph.addLight(light);
    
    light = new SPointLight();
    light->transform.translation.y = 1.0;
    light->transform.translation.x = -5.0;
    
    light_graph.addLight(light);
    
    light = new SPointLight();
    light->transform.translation.y = 2.0;
    light->transform.translation.z = -3.0;
    light->transform.translation.x = 2.0;
    
    light_graph.addLight(light);
    
}

void SDeferredRenderingPipleline::unload() {
    
    // Unload the gbuffer
    gbuffer->unload();
    delete gbuffer;
    
}

void SDeferredRenderingPipleline::render(double interpolation, SCamera& camera, SSceneGraph& scene_graph) {
    
    // The 3D world has culling enabled
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    /******************************************************************************
     * Shadow mapping updates                                                     *
     ******************************************************************************/
    
    light_graph.cullLights();
    //light_graph.updateShadows(camera, scene_graph, interpolation);
    
    // Render the 3D scene into the GBuffer
    gbuffer->bind();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 projection_matrix = SGL::getProjectionMatrix3D(*viewport_3D);
    SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
    
    SGL::setUpViewport(*viewport_3D);
    
    scene_graph.render(camera, interpolation);
    
    // Get view matrix
    glm::mat4 view_matrix = SGL::getMatrix(MAT_VIEW_MATRIX);
    
    glm::mat4 inverse_proj_view = glm::inverse(projection_matrix * view_matrix);
    
    // Render the lit buffer to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up the new viewport
    SGL::setUpViewport(*viewport_2D);
    projection_matrix = SGL::getProjectionMatrix2D(*viewport_2D);
    SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
    SGL::clearMatrix(MAT_VIEW_MATRIX);
    
    // No culling or depth testing for this stage
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    lit_shader->bind();
    
    // Bind the texture locations
    int texture = GBUFFER_DEPTH;
    lit_shader->bindUniform(&texture, "tex_depth", UNIFORM_INT, 1);
    
    texture = GBUFFER_ALBEDO;
    lit_shader->bindUniform(&texture, "tex_albedo", UNIFORM_INT, 1);
    
    texture = GBUFFER_NORMAL;
    lit_shader->bindUniform(&texture, "tex_normal", UNIFORM_INT, 1);
    
    texture = GBUFFER_ORM;
    lit_shader->bindUniform(&texture, "tex_orm", UNIFORM_INT, 1);
    
    texture = 4;
    lit_shader->bindUniform(&texture, "tex_cube", UNIFORM_INT, 1);
    
    texture = 5;
    lit_shader->bindUniform(&texture, "tex_shadow", UNIFORM_INT, 1);
    
    // Bind other uniforms needed for lighting
    lit_shader->bindUniform(&inverse_proj_view, "inverse_proj_view", UNIFORM_MAT4, 1);
    
    // Lighting uniforms
    //lit_shader->bindUniform(&light->light_matrix, "light_matrix", UNIFORM_MAT4, 1);
    int light_count = light_graph.getLightCount();
    lit_shader->bindUniform(&light_count, "light_count", UNIFORM_INT, 1);
    
    glm::vec3* light_positions = light_graph.getLightPositions(interpolation);
    lit_shader->bindUniform(light_positions, "light_positions", UNIFORM_VEC3, light_count);
    
    lit_shader->bindUniform(view_pos_u);
    
    // Bind the textures and then render
    glActiveTexture(GL_TEXTURE0);
    gbuffer->bindTexture(GBUFFER_DEPTH);
    glActiveTexture(GL_TEXTURE1);
    gbuffer->bindTexture(GBUFFER_ALBEDO);
    glActiveTexture(GL_TEXTURE2);
    gbuffer->bindTexture(GBUFFER_NORMAL);
    glActiveTexture(GL_TEXTURE3);
    gbuffer->bindTexture(GBUFFER_ORM);
    environment_map->bind(4);
    
    SGL::drawRect(glm::vec2(0, 0), glm::vec2(WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2));
    
}
