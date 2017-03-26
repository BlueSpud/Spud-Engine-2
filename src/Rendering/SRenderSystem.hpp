//
//  SRenderSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/12/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SRenderSystem_hpp
#define SRenderSystem_hpp

#include "SSubsystem.hpp"

#include "SRenderingPipeline.hpp"

/******************************************************************************
 *  Definition for the render system                                          *
 ******************************************************************************/

class SRenderSystem : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void render(double interpolation);
    
        static SRenderingPipeline* rendering_pipeline;
        static SSceneGraph* current_scene_graph;
    
    
};

#endif /* SRenderSystem_hpp */
