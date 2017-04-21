//
//  SDeferredRenderingPipeline.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/21/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SDeferredPipeline_hpp
#define SDeferredPipeline_hpp

#include "Rendering/Pipeline/SRenderingPipeline.hpp"
#include "GBuffer.h"
#include "Resource/Resources/Rendering/SCubeMap.hpp"
#include "Rendering/Pipeline/Effects/AO/SAmbientOcclusionPass.hpp"
#include "Rendering/Pipeline/Effects/Reflection/SScreenSpaceReflections.hpp"

#include "Rendering/Light/SDeferredTileController.hpp"

/******************************************************************************
 *  Definition for deferred rendering pipeline                                *
 ******************************************************************************/

class SDeferredRenderingPipeline : public SRenderingPipeline {
    
    public:
    
        SDeferredRenderingPipeline(SViewport* _viewport_2D, SViewport* _screen_viewport, SViewport3D* _viewport_3D);
        virtual ~SDeferredRenderingPipeline();
    
        virtual void render(SSceneGraph& scene_graph, SCamera& camera, double interpolation);
	
		std::shared_ptr<SCubeMap> environment_map;
    
    private:
    
        // Stuff for rendering
        SFramebuffer* gbuffer;
    
        std::shared_ptr<SShader> lit_shader;
        std::shared_ptr<SShader> simple_shader;
	
        SUniform* view_pos_u;
	
		SDeferredTileController tile_controller;
		SScreenSpaceReflectionPass* reflection_pass;

};

#endif /* SDeferredRenderingPipeline_hpp */
