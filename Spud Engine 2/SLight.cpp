//
//  SLight.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLight.hpp"

glm::mat4 SLight::bias = glm::mat4(0.5, 0.0, 0.0, 0.0,
                                   0.0, 0.5, 0.0, 0.0,
                                   0.0, 0.0, 0.5, 0.0,
                                   0.5, 0.5, 0.5, 1.0);

SShader* SLight::shadow_shader;

/******************************************************************************
 *  Functions for generic light                                               *
 ******************************************************************************/

SLight::SLight() {
    
    // Get the shader for shadow mapping if we dont already have it
    if (!SLight::shadow_shader)
        SLight::shadow_shader = (SShader*)SResourceManager::getResource(SPath("Shader/simple.glsl"));
    
}

/******************************************************************************
 *  Functions for point light                                                 *
 ******************************************************************************/

SPointLight::SPointLight() : bounding_box(glm::vec3(-4.0), glm::vec3(4.0), &transform) { /* No initialization */ }

void SPointLight::renderShadowMap(SSceneGraph& scene_graph, double interpolation) {
    
    // Shadows not supported currently!
    
}

bool SPointLight::needsShadowUpdate() {
    
    // For now always return false
    return false;
}

bool SPointLight::shouldBeCulled(glm::mat4& projection_view_matrix) {
    
    // Project thhe bounding box
    return true;
    return bounding_box.frustrumCull(projection_view_matrix);
    
}

/******************************************************************************
 *  Functions for directional light                                           *
 ******************************************************************************/

void SDirectionalLight::renderShadowMap(SSceneGraph& scene_graph, double interpolation) {
    
    // Create a camera that we can use to render the scene
    SCamera camera;
    camera.transform = transform;
    
    SViewport3D viewport = SViewport3D(glm::vec2(SHADOW_MAP_ATLAS_TILE_SIZE), shadow_map_position * SHADOW_MAP_ATLAS_TILE_SIZE, 45.0f, glm::vec2(0.1, 1000.0));
    
    // Bind the framebuffer and the viewport
    glm::mat4 projection_matrix = SGL::getProjectionMatrix3D(viewport);
    SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
    
    SGL::setUpViewport(viewport);
    glScissor(viewport.screen_pos.x, viewport.screen_pos.y, viewport.screen_size.x, viewport.screen_size.y);
    
    // Clear the framebuffer and draw the scene
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Render the scene from the camera
    scene_graph.render(camera, SLight::shadow_shader, interpolation);

    light_matrix = SLight::bias * projection_matrix * camera.getCameraMatrix(interpolation);
    
}

bool SDirectionalLight::needsShadowUpdate() {
   
    // If we need an update, update oursleves
    if (needs_shadow_update) {
        
        needs_shadow_update = false;
        return true;
        
    }
    
    return false;
   
}

bool SDirectionalLight::shouldBeCulled(glm::mat4& projection_view_matrix) {
    
    // Directional lights are always rendered as a full screen quad, never culled
    return true;
    
}
