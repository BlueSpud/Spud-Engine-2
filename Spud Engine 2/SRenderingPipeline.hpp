//
//  SRenderingPipeline.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SRenderingPipeline_hpp
#define SRenderingPipeline_hpp

#include "SSceneGraph.hpp"
#include "SShader.hpp"

/******************************************************************************
 *  Definition for post-process pass                                          *
 ******************************************************************************/

class SPostProcessPass {
    
    SShader* shader;
    
};

/******************************************************************************
 *  Definition for default rendering pipeline                                  *
 ******************************************************************************/

class SRenderingPipeline {
    
    public:
    
        SRenderingPipeline(SViewport* _viewport_2D, SViewport3D* _viewport_3D);
        virtual ~SRenderingPipeline() { /* intentionally blank */ }
    
        virtual void render(SSceneGraph& scene_graph, SCamera& camera, double interpolation) = 0;
    
        void addPostProcessPass(SPostProcessPass pass);
    
    protected:
    
        std::vector<SPostProcessPass> post_process_passes;
    
        SViewport* viewport_2D;
        SViewport3D* viewport_3D;

    
};

#endif /* SRenderingPipeline_hpp */
