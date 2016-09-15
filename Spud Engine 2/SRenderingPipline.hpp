//
//  SRenderingPipline.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SRenderingPipline_hpp
#define SRenderingPipline_hpp

#include "SSceneGraph.hpp"
#include "SShader.hpp"

/******************************************************************************
 *  Definition for post-process pass                                          *
 ******************************************************************************/

class SPostProcessPass {
    
    SShader* shader;
    
};

/******************************************************************************
 *  Definition for default rendering pipline                                  *
 ******************************************************************************/

class SRenderingPipline {
    
    public:
    
        virtual void render(double interpolation, SCamera& camera, SSceneGraph& scene_graph);
    
        void addPostProcessPass(SPostProcessPass pass);
    
    protected:
    
        std::vector<SPostProcessPass> post_process_passes;
    
};

#endif /* SRenderingPipline_hpp */
