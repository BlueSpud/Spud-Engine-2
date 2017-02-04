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
#include "SLightGraph.hpp"
#include "SShader.hpp"

/******************************************************************************
 *  Definition for post-process pass                                          *
 ******************************************************************************/

struct SPostProcessPassData {
    
    SViewport* viewport_2D;
    SViewport3D* viewport_3D;
    
    SFramebuffer* framebuffer;
    int texture_bind_start;
    
    glm::mat4* view_matrix;
    glm::mat4* projection_matrix;
    
};

class SPostProcessPass {
    
    public:
    
        virtual void render(SPostProcessPassData& data) = 0;
    
};

/******************************************************************************
 *  Definition for default rendering pipeline                                  *
 ******************************************************************************/

class SRenderingPipeline {
    
    public:
    
        SRenderingPipeline(SViewport* _viewport_2D, SViewport* _screen_viewport, SViewport3D* _viewport_3D);
        virtual ~SRenderingPipeline() { /* intentionally blank */ }
    
        virtual void render(SSceneGraph& scene_graph, SLightGraph& light_graph, SCamera& camera, double interpolation) = 0;
        void finalizeRender(SFramebuffer* output_framebuffer);
    
        void runPostProcess(glm::mat4& view_matrix, glm::mat4& projection_matrix, int texture_bind_start);
        void addPostProcessPass(SPostProcessPass* pass);
    
    protected:
    
        SFramebuffer* final_framebuffer;
    
        std::vector<SPostProcessPass*> post_process_passes;
    
        SShader* simple_shader;
        SViewport* viewport_2D;
		SViewport* screen_viewport;
        SViewport3D* viewport_3D;

    
};

#endif /* SRenderingPipeline_hpp */
