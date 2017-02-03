//
//  SDeferredRenderingPipeline.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/21/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SDeferredRenderingPipeline.hpp"

/******************************************************************************
 * Implementation for deferred rendering pipeline                             *
 ******************************************************************************/

SDeferredRenderingPipleline::SDeferredRenderingPipleline(SViewport* _viewport_2D, SViewport3D* _viewport_3D) : SRenderingPipeline(_viewport_2D, _viewport_3D) {
    
    // Create the gbuffer and its attatchments
    std::vector<SFramebufferAttatchment*> attatchments;
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_DEPTH, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, GBUFFER_DEPTH));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GBUFFER_ALBEDO));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_FLOAT, GBUFFER_NORMAL));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GBUFFER_ORM));
    gbuffer = new SFramebuffer(attatchments, viewport_3D->screen_size.x, viewport_3D->screen_size.y);
    
    // Create an ambient occlusion pass
    addPostProcessPass(new SAmbientOcclusionPass(_viewport_3D->screen_size));
    
    // Get the cube map
    environment_map =  SResourceManager::getResource<SCubeMap>(SPath("Texture/room.cube"));
    
    // Get the lighting shader
    lit_shader = SResourceManager::getResource<SShader>(SPath("Shader/lighting/deferred_lighting.glsl"));
    simple_shader = SResourceManager::getResource<SShader>(SPath("Shader/simple/simple_texture.glsl"));
    
    // Get the view pos
    view_pos_u = SUniformManger::instance()->getUniformWithName("view_position");
    
}

SDeferredRenderingPipleline::~SDeferredRenderingPipleline() {
    
    // Unload the gbuffer
    gbuffer->unload();
    delete gbuffer;
}

void SDeferredRenderingPipleline::render(SSceneGraph& scene_graph, SLightGraph& light_graph, SCamera& camera, double interpolation) {
    
    /******************************************************************************
     * 3D viewport setup                                                          *
     ******************************************************************************/
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glm::mat4 projection_matrix_3D = SGL::getProjectionMatrix3D(*viewport_3D);
    
    glm::mat4 view_matrix = camera.getCameraMatrix(interpolation);
    glm::mat4 projection_view_matrix = projection_matrix_3D * view_matrix;
    
    // Calculate the view frustum
    //glm::vec3* camera_frustum = SCamera::calculateFrustumWithPlanes(projection_view_matrix, 0.0, 1.0);
    
    /******************************************************************************
     * Shadow mapping updates                                                     *
     ******************************************************************************/

    light_graph.cullLights(projection_view_matrix);
    light_graph.updateShadows(scene_graph, projection_matrix_3D, view_matrix, interpolation);
    
    /******************************************************************************
     * Gbuffer render                                                             *
     ******************************************************************************/
    
    SGL::setUpViewport(*viewport_3D);
    SGL::loadMatrix(projection_matrix_3D, MAT_PROJECTION);
    
    // Render the 3D scene into the GBuffer
    gbuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene_graph.render(camera, true, interpolation);
    
    /******************************************************************************
     * Lighting pass                                                              *
     ******************************************************************************/
    
    // No culling or depth testing for this stage
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    // Get view matrix
    glm::mat4 inverse_proj_view = glm::inverse(projection_view_matrix);
    
    /******************************************************************************
     * GBuffer and shadow atlas texture bind                                      *
     ******************************************************************************/
    
    glActiveTexture(GL_TEXTURE0);
    gbuffer->bindTexture(GBUFFER_DEPTH);
    glActiveTexture(GL_TEXTURE1);
    gbuffer->bindTexture(GBUFFER_ALBEDO);
    glActiveTexture(GL_TEXTURE2);
    gbuffer->bindTexture(GBUFFER_NORMAL);
    glActiveTexture(GL_TEXTURE3);
    gbuffer->bindTexture(GBUFFER_ORM);
    environment_map->bind(ENVIRONMENT_MAP);
    glActiveTexture(GL_TEXTURE5);
    light_graph.shadow_map_buffer->bindTexture(0);
    
    // Render the lit buffer to the screen
    final_framebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up the new viewport
    SGL::setUpViewport(*viewport_2D);
    glm::mat4 projection_matrix_2D = SGL::getProjectionMatrix2D(*viewport_2D);
    SGL::loadMatrix(projection_matrix_2D, MAT_PROJECTION);
    SGL::clearMatrix(MAT_MODEL);
    SGL::clearMatrix(MAT_VIEW);
    
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
    
    texture = ENVIRONMENT_MAP;
    lit_shader->bindUniform(&texture, "tex_cube", UNIFORM_INT, 1);
    
    texture = SHADOW_ATLAS;
    lit_shader->bindUniform(&texture, "tex_shadow", UNIFORM_INT, 1);
    
    // Bind other uniforms needed for lighting
    lit_shader->bindUniform(&inverse_proj_view, "inverse_proj_view", UNIFORM_MAT4, 1);
    
    // Lighting uniforms
    int light_count = light_graph.getLightCount();
    lit_shader->bindUniform(&light_count, "light_count", UNIFORM_INT, 1);
    
    lit_shader->bindUniform(light_graph.getLightPositions(interpolation).data(), "light_positions", UNIFORM_VEC3, light_count);
    lit_shader->bindUniform(light_graph.getColors().data(), "light_colors", UNIFORM_VEC3, light_count);
    lit_shader->bindUniform(light_graph.getShadowLights().data(), "lights_shadow", UNIFORM_INT, light_count);
    
    std::vector<glm::mat4> light_matrices = light_graph.getShadowMatrices();
    lit_shader->bindUniform(light_matrices.data(), "light_matrices", UNIFORM_MAT4, (int)light_matrices.size());
    
    lit_shader->bindUniform(light_graph.getShadowMapCoordinates().data(), "shadow_map_coordinates", UNIFORM_VEC2, (int)light_matrices.size());
    
    lit_shader->bindUniform(view_pos_u);
    
    SGL::renderRect(glm::vec2(0, 0), viewport_2D->screen_size);
    
    // Perform post-processing
    runPostProcess(view_matrix, projection_matrix_3D, POST_RPOCESS_START);
    
}
