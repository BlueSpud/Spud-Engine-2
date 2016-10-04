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
    
    gbuffer = new SFramebuffer(attatchments, viewport_3D->screen_size.x, viewport_3D->screen_size.y);
    
    // Get the cube map
    environment_map =  (SCubeMap*)SResourceManager::getResource(SPath("Texture/room.cube"));
    
    // Get the lighting shader
    lit_shader = (SShader*)SResourceManager::getResource(SPath("Shader/deferred_lighting.glsl"));
    simple_shader = (SShader*)SResourceManager::getResource(SPath("Shader/simple.glsl"));
    
    // Get the view pos
    view_pos_u = SUniformManger::instance()->getUniformWithName("view_position");
    
    // Create the light graph
    light_graph = new SSimpleLightGraph();
    
    // Create a temporary light
    light = new SPointLight();
    light->transform.translation.y = 2.0;
    light->transform.translation.z = 4.0;
    
    light_graph->addLight(light);
    
    light = new SPointLight();
    light->transform.translation.y = 1.0;
    light->transform.translation.x = -5.0;
    
    light_graph->addLight(light);
    
    light = new SPointLight();
    light->transform.translation.y = 2.0;
    light->transform.translation.z = -3.0;
    light->transform.translation.x = 2.0;
    
    light_graph->addLight(light);
    
}

void SDeferredRenderingPipleline::unload() {
    
    // Unload the gbuffer
    gbuffer->unload();
    delete gbuffer;
    
    delete light_graph;
    
}

void SDeferredRenderingPipleline::render(double interpolation, SCamera& camera, SSceneGraph& scene_graph) {
    
    /******************************************************************************
     * 3D viewport setup                                                          *
     ******************************************************************************/
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glm::mat4 projection_matrix_3D = SGL::getProjectionMatrix3D(*viewport_3D);
    SGL::loadMatrix(projection_matrix_3D, MAT_PROJECTION_MATRIX);
    
    glm::mat4 view_matrix = camera.getCameraMatrix(interpolation);
    glm::mat4 projection_view_matrix = projection_matrix_3D * view_matrix;
    
    SGL::setUpViewport(*viewport_3D);
    
    /******************************************************************************
     * Shadow mapping updates                                                     *
     ******************************************************************************/

    light_graph->cullLights(projection_view_matrix);
    light_graph->updateShadows(camera, scene_graph, interpolation);
    
    /******************************************************************************
     * Gbuffer render                                                             *
     ******************************************************************************/
    
    // Render the 3D scene into the GBuffer
    gbuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene_graph.render(camera, interpolation);
    
    /******************************************************************************
     * Lighting pass                                                              *
     ******************************************************************************/
    
    // Get view matrix
    glm::mat4 inverse_proj_view = glm::inverse(projection_matrix_3D * view_matrix);
    
    // Render the lit buffer to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up the new viewport
    SGL::setUpViewport(*viewport_2D);
    glm::mat4 projection_matrix_2D = SGL::getProjectionMatrix2D(*viewport_2D);
    SGL::loadMatrix(projection_matrix_2D, MAT_PROJECTION_MATRIX);
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
    int light_count = light_graph->getLightCount();
    lit_shader->bindUniform(&light_count, "light_count", UNIFORM_INT, 1);
    
    glm::vec3* light_positions = light_graph->getLightPositions(interpolation);
    lit_shader->bindUniform(light_positions, "light_positions", UNIFORM_VEC3, light_count);
    
    lit_shader->bindUniform(view_pos_u);
    
    /******************************************************************************
     * GBuffer and shadow atlas texture bind                                      *
     ******************************************************************************/
    
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
    
    SGL::drawRect(glm::vec2(0, 0), glm::vec2(viewport_2D->screen_size.x, viewport_2D->screen_size.y));
    
//    simple_shader->bind();
//    
//
//    
//    glm::vec2 origin = (glm::vec2)light->bounding_box.projected_mins;
//    origin.y = -origin.y;
//    origin = (origin + glm::vec2(1.0)) / glm::vec2(2.0);
//    
//    glm::vec2 end = (glm::vec2)light->bounding_box.projected_maxes;
//    end.y = -end.y;
//    end = (end + glm::vec2(1.0)) / glm::vec2(2.0);
//    SGL::drawRect(origin * viewport_2D->screen_size, (end - origin) * viewport_2D->screen_size);
    
}
