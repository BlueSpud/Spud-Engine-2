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

#include "SDeferredTileController.hpp"

/******************************************************************************
 *  Definition for deferred rendering pipeline                                *
 ******************************************************************************/

class SDeferredRenderingPipleline : public SRenderingPipeline {
    
    public:
    
        SDeferredRenderingPipleline(SViewport* _viewport_2D, SViewport* _screen_viewport, SViewport3D* _viewport_3D);
        virtual ~SDeferredRenderingPipleline();
    
        virtual void render(SSceneGraph& scene_graph, SLightGraph& light_graph, SCamera& camera, double interpolation);
    
    private:
    
        // Stuff for rendering
        SFramebuffer* gbuffer;
    
        std::shared_ptr<SShader> lit_shader;
        std::shared_ptr<SShader> simple_shader;
    
        std::shared_ptr<SCubeMap> environment_map;
        SUniform* view_pos_u;
	
		SDeferredTileController tile_controller;

};

#endif /* SDeferredRenderingPipeline_hpp */
