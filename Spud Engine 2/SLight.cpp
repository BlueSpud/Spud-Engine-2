//
//  SLight.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLight.hpp"

glm::mat4 bias = glm::mat4(0.5, 0.0, 0.0, 0.0,
                           0.0, 0.5, 0.0, 0.0,
                           0.0, 0.0, 0.5, 0.0,
                           0.5, 0.5, 0.5, 1.0);

/******************************************************************************
 *  Functions for directional light                                           *
 ******************************************************************************/

SDirectionalLight::SDirectionalLight() : shadow_viewport(glm::vec2(1024, 1024), glm::vec2(0), 45.0f, glm::vec2(0.1, 1000.0)) {
    
    // Initialize the framebuffer
    std::vector<SFramebufferAttatchment*> attatchments;
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_DEPTH, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
    
    shadow_buffer = new SFramebuffer(attatchments, 1024, 1024);
    
}

glm::mat4 SDirectionalLight::renderShadowMap(SSceneGraph& scene_graph, double interpolation) {
    
    // Create a camera that we can use to render the scene
    SCamera camera;
    camera.transform = transform;
    
    // Bind the framebuffer and the viewport
    glm::mat4 projection_matrix = SGL::getProjectionMatrix3D(shadow_viewport);
    SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
    
    SGL::setUpViewport(shadow_viewport);
    
    shadow_buffer->bind();
    
    // Clear the framebuffer and draw the scene
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Render the scene from the camera
    scene_graph.render(camera, interpolation);

    return bias * projection_matrix * camera.getCameraMatrix(interpolation);
    
}
