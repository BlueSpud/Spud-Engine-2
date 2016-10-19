//
//  SLightGraph.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/29/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLightGraph.hpp"

/******************************************************************************
 *  Functions for default light graph                                         *
 ******************************************************************************/

SLightGraph::SLightGraph() {
    
    // Create a massive framebuffer for a bunch of shadow maps
    std::vector<SFramebufferAttatchment*> attatchments;
    attatchments.push_back(new SFramebufferAttatchment(FRAMEBUFFER_DEPTH, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0));
    shadow_map_buffer = new SFramebuffer(attatchments, SHADOW_MAP_ATLAS_SIZE, SHADOW_MAP_ATLAS_SIZE);
    
    // Create the 2D array for the shadow maps
    shadow_map_atlas = new bool*[SHADOW_MAP_ATLAS_TILE_COUNT];
    for (int i = 0; i < SHADOW_MAP_ATLAS_TILE_COUNT; i++)
        shadow_map_atlas[i] = new bool[SHADOW_MAP_ATLAS_TILE_COUNT];
    
}

SLightGraph::~SLightGraph() {

    // Delete the shadow atlas
    for (int i = 0; i < SHADOW_MAP_ATLAS_TILE_COUNT; i++)
        delete shadow_map_atlas[i];
    delete shadow_map_atlas;
    
    // Unload and delete the shadow map framebuffer
    shadow_map_buffer->unload();
    delete shadow_map_buffer;
}

/******************************************************************************
 *  Functions for simple light graph                                          *
 ******************************************************************************/

SSimpleLightGraph::SSimpleLightGraph() : SLightGraph() {

    // Create the array for the light positions
    light_positions = new glm::vec3[0];

}

void SSimpleLightGraph::cullLights(glm::mat4& projection_view_matrix) {
    
    /******************************************************************************
     *  Cull lights that cant be seen by the camera                               *
     *  Save light list so that we can use it for the lighting pass later         *
     ******************************************************************************/
    
    // Clear the old list of lights
    culled_lights.clear();
    
    std::list<SLight*>::iterator i = lights.begin();
    while (i != lights.end()) {
        
        // Check if the light needs a shadow update and if we can see it in the frustrum
        if ((*i)->shouldBeCulled(projection_view_matrix))
            culled_lights.push_back(*i);
        
        i++;
    }
    
}

void SSimpleLightGraph::updateShadows(SCamera& scene_camera, SSceneGraph& scene_graph, double interpolation) {

    bool bound_shadow_map_buffer = false;
    
    // Take the list of lights that need to be updated and render their shadow maps
    for (int i= 0; i < culled_lights.size(); i++)
        if (culled_lights[i]->needsShadowUpdate()) {
            
            
            // Check if we havent bound the shadow map buffer yet, if we have no lights we dont need to bind it
            if(!bound_shadow_map_buffer) {
                
                bound_shadow_map_buffer = true;
                shadow_map_buffer->bind();
                
                // Enable scissor testing
                glEnable(GL_SCISSOR_TEST);
                
            }
            
            culled_lights[i]->renderShadowMap(scene_graph, interpolation);
            
        }
    
    if (bound_shadow_map_buffer) {
    
        // Disable scissor testing if we enabled it
        glDisable(GL_SCISSOR_TEST);
        
    }

}

void SSimpleLightGraph::addLight(SLight* light) {

    // Add the light to the graph
    lights.push_back(light);
    
    if (light->casts_shadow) {
    
        // Find a spot for the shadow map
        for (int i = 0; i < SHADOW_MAP_ATLAS_TILE_COUNT; i++) {
            for (int j = 0; j < SHADOW_MAP_ATLAS_TILE_COUNT; j++) {
            
                if (!shadow_map_atlas[i][j]) {
                
                    // Save that we used this tile in the atlas and then return
                    shadow_map_atlas[i][j] = true;
                    light->shadow_map_position = glm::vec2(i, j);
                    return;
                
                }
            
            }
        }
        
    }

}

void SSimpleLightGraph::removeLight(SLight* light) {

    // Free up the spot in the shadow map atlas
    if (light->casts_shadow)
        shadow_map_atlas[light->shadow_map_position.x][light->shadow_map_position.y] = false;
    
    // Remove the light from the graph
    lights.remove(light);

}


int SSimpleLightGraph::getLightCount() { return (int)culled_lights.size(); }

std::vector<glm::vec3> SSimpleLightGraph::getLightPositions(double interpolation) {
    
    // Make a place we can store and return the positions
    std::vector<glm::vec3> light_positions;
    
    // Collect the positions of the lights in an array
    for (int i = 0; i < culled_lights.size(); i++)
        light_positions.push_back(culled_lights[i]->transform.translation + culled_lights[i]->transform.translation_velocity * (float)interpolation);
    
    return light_positions;
    
}

std::vector<glm::mat4> SSimpleLightGraph::getShadowMatrices() {

    // Make a place we can store and return the matrices
    std::vector<glm::mat4> light_matrices;
    
    // Collect the light matricies for upload
    for (int i = 0; i < culled_lights.size(); i++)
        if (culled_lights[i]->casts_shadow)
            light_matrices.push_back(culled_lights[i]->light_matrix);
    
    return light_matrices;

}

std::vector<glm::vec2> SSimpleLightGraph::getShadowMapCoordinates() {
    
    // Make a place we can store and return the shadow map coordinates
    std::vector<glm::vec2> shadow_map_coordinates;
    
    // Collect the shadow map coordinates for upload
    for (int i = 0; i < culled_lights.size(); i++)
        if (culled_lights[i]->casts_shadow)
            shadow_map_coordinates.push_back(culled_lights[i]->shadow_map_position);
    
    return shadow_map_coordinates;
    
}

std::vector<glm::vec3> SSimpleLightGraph::getColors() {
    
    // Make a place we can store and return the color
    std::vector<glm::vec3> light_colors;
    
    // Collect the color of the lights in an array
    for (int i = 0; i < culled_lights.size(); i++)
        light_colors.push_back(culled_lights[i]->light_color);
    
    return light_colors;
    
}

std::vector<int> SSimpleLightGraph::getShadowLights() {

    // Make a place we can store and return the shadow bools
    std::vector<int> lights_shadow;
    
    // Collect if the lights shadow
    for (int i = 0; i < culled_lights.size(); i++)
        lights_shadow.push_back(culled_lights[i]->casts_shadow);
    
    return lights_shadow;

}

SSimpleLightGraph::~SSimpleLightGraph() {
    
    // Delete all the lights in the graph
    std::list<SLight*>::iterator i = lights.begin();
    while (i != lights.end()) {
        
        delete *i;
        
        i++;
        
    }
    
    // Delete the light position array
    delete light_positions;

}

