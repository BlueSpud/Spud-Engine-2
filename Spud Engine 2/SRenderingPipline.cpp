//
//  SRenderingPipline.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SRenderingPipline.hpp"

/******************************************************************************
 *  Functions for default rendering pipline                                   *
 ******************************************************************************/

void SRenderingPipline::render(double interpolation, SCamera& camera, SSceneGraph& scene_graph) {
    
    // Simply render the scene graph to the screen
    scene_graph.render(camera, interpolation);
    
}

void SRenderingPipline::addPostProcessPass(SPostProcessPass pass) { post_process_passes.push_back(pass); } 
