//
//  SRenderingPipeline.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SRenderingPipeline.hpp"

/******************************************************************************
 *  Functions for default rendering pipeline                                   *
 ******************************************************************************/

SRenderingPipeline::SRenderingPipeline(SViewport* _viewport_2D, SViewport3D* _viewport_3D) {
    
    // Save the viewports
    viewport_2D = _viewport_2D;
    viewport_3D = _viewport_3D;
    
}

void SRenderingPipeline::addPostProcessPass(SPostProcessPass pass) { post_process_passes.push_back(pass); }
