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

SDeferredRenderingPipleline::SDeferredRenderingPipleline(SViewport* _viewport_2D, SViewport* _screen_viewport, SViewport3D* _viewport_3D) :
	SRenderingPipeline(_viewport_2D, _screen_viewport, _viewport_3D),
	tile_controller(_viewport_3D->screen_size)																							{
    
    // Create the gbuffer and its attatchments
    std::vector<SFramebufferAttatchment*> attatchments;
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_DEPTH, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, GBUFFER_DEPTH));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GBUFFER_ALBEDO));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGB, GL_RGBA, GL_FLOAT, GBUFFER_NORMAL));
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGB, GL_RGBA, GL_UNSIGNED_INT, GBUFFER_ORM));
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
    
    scene_graph.render(camera, interpolation);
    
    /******************************************************************************
     * Lighting pass                                                              *
     ******************************************************************************/
	
	// Build the lighting tile grid
	tile_controller.buildLightGrid(projection_view_matrix, &light_graph);
	
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
    light_graph.shadow_map_buffer->bindTexture(1);
    
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
	lit_shader->bindTextureLocation("tex_depth", GBUFFER_DEPTH);
	lit_shader->bindTextureLocation("tex_albedo", GBUFFER_ALBEDO);
    lit_shader->bindTextureLocation("tex_normal", GBUFFER_NORMAL);
	lit_shader->bindTextureLocation("tex_orm", GBUFFER_ORM);
    lit_shader->bindTextureLocation("tex_cube", ENVIRONMENT_MAP);
    lit_shader->bindTextureLocation("tex_shadow", SHADOW_ATLAS);
    
    // Bind other uniforms needed for lighting
    lit_shader->bindUniform(&inverse_proj_view, "inverse_proj_view", UNIFORM_MAT4, 1);
	lit_shader->bindUniform(&viewport_3D->screen_size, "screen_size", UNIFORM_VEC2, 1);
    
	// Upload all the lighting information
	light_graph.uploadCulledLightData(lit_shader, interpolation);
    
    lit_shader->bindUniform(view_pos_u);
    
	//SGL::renderRect(glm::vec2(0, 0), viewport_2D->screen_size);
	tile_controller.renderLightGrid(lit_shader);
	
//	STransform t;
//	SBoundingBox b = SBoundingBox(glm::vec3(-2.0), glm::vec3(2.0), &t);
//	b.project(projection_view_matrix, true);
//	
//	glm::vec2 start = (glm::vec2(b.projected_mins) + 1.0f) / 2.0f;
//	glm::vec2 size = ((glm::vec2(b.projected_maxes) + 1.0f) / 2.0f) - start;
//	
//	simple_shader->bind();
//	SGL::renderRect(glm::vec2(start.x, 1.0 - start.y) * viewport_3D->screen_size, glm::vec2(size.x, -size.y) * viewport_3D->screen_size);
	
    // Perform post-processing
	runPostProcess(view_matrix, projection_matrix_3D, POST_RPOCESS_START);
	
}
