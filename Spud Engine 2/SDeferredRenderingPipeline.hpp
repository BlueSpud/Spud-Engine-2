//
//  SDeferredRenderingPipeline.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/21/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SDeferredPipeline_hpp
#define SDeferredPipeline_hpp

#include "SRenderingPipeline.hpp"
#include "GBuffer.h"
#include "SCubeMap.hpp"
#include "SAmbientOcclusionPass.hpp"

/******************************************************************************
 *  Definition for deferred rendering pipeline                                *
 ******************************************************************************/

class SDeferredRenderingPipleline : public SRenderingPipeline {
    
    public:
    
        SDeferredRenderingPipleline(SViewport* _viewport_2D, SViewport3D* _viewport_3D);
        virtual ~SDeferredRenderingPipleline();
    
        virtual void render(SSceneGraph& scene_graph, SLightGraph& light_graph, SCamera& camera, double interpolation);
    
    private:
    
        // Stuff for rendering
        SFramebuffer* gbuffer;
    
        SShader* gbuffer_shader;
        SShader* lit_shader;
        SShader* simple_shader;
    
        SCubeMap* environment_map;
        SUniform* view_pos_u;
    
};

#endif /* SDeferredRenderingPipeline_hpp */
