//
//  SRenderSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/12/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SRenderSystem.hpp"

SRenderingPipeline* SRenderSystem::rendering_pipeline;
SSceneGraph* SRenderSystem::current_scene_graph;
SLightGraph* SRenderSystem::current_light_graph;

/******************************************************************************
 *  Implementation for the render system                                      *
 ******************************************************************************/

void SRenderSystem::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SRenderSystem startup");
    
}

void SRenderSystem::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SRenderSystem shutdown");
    
    // Delete everything we have because we become responsible for something once its given to us
    if (rendering_pipeline)
        delete rendering_pipeline;
    
    if (current_scene_graph)
        delete current_scene_graph;
    
    if (current_light_graph)
        delete current_light_graph;

}

void SRenderSystem::render(double interpolation) {
    
    // If we have a scene graph and a rendering pipline we can render the scene
    if(rendering_pipeline && current_scene_graph && current_light_graph)
        rendering_pipeline->render(*current_scene_graph, *current_light_graph, *SCamera::current_camera, interpolation);
    
}
