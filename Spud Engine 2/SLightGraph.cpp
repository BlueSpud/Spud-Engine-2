//
//  SLightGraph.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/29/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLightGraph.hpp"

SLightGraph::~SLightGraph() { /* stub, destroy objects, light graph manages memory for them */ }

/******************************************************************************
 *  Functions for simple light graph                                          *
 ******************************************************************************/

void SSimpleLightGraph::cullLights() {
    
    /******************************************************************************
     *  Collects the lights who's shadow maps need updating                       *
     *  Cull lights that cant be seen by the camera                               *
     *  Save light list so that we can use it for the lighting pass later         *
     ******************************************************************************/
    
    // Clear the old list of lights
    culled_lights.clear();
    
    std::list<SLight*>::iterator i = lights.begin();
    while (i != lights.end()) {
        
        // Check if the light needs a shadow update and if we can see it in the frustrum
        culled_lights.push_back(*i);
        
        i++;
    }
    
}

void SSimpleLightGraph::updateShadows(SCamera& scene_camera, SSceneGraph& scene_graph, double interpolation) {

    // Take the list of lights that need to be updated and render their shadow maps
    for (int i= 0; i < culled_lights.size(); i++)
        if (culled_lights[i]->needsShadowUpdate())
            culled_lights[i]->renderShadowMap(scene_graph, interpolation);

}

void SSimpleLightGraph::addLight(SLight* light) {

    // Add the light to the graph
    lights.push_back(light);

}

void SSimpleLightGraph::removeLight(SLight* light) {

    // Remove the light from the graph
    lights.remove(light);

}


int SSimpleLightGraph::getLightCount() { return (int)culled_lights.size(); }

glm::vec3* SSimpleLightGraph::getLightPositions(double interpolation) {
    
    // Collect the positions of the lights in an array
    if (light_positions)
        delete light_positions;
    light_positions = new glm::vec3[getLightCount()];
    
    for (int i = 0; i < culled_lights.size(); i++)
        light_positions[i] = culled_lights[i]->transform.translation + culled_lights[i]->transform.translation_velocity * (float)interpolation;
    
    return light_positions;
    
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

