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
#include "SCubeMap.hpp"
#include "SLightGraph.hpp"
#include "SAmbientOcclusionPass.hpp"

/******************************************************************************
 *  Definition for gbuffer components                                         *
 ******************************************************************************/

#define GBUFFER_DEPTH 0
#define GBUFFER_ALBEDO 1
#define GBUFFER_NORMAL 2
#define GBUFFER_ORM 3
#define ENVIRONMENT_MAP 4
#define SHADOW_ATLAS 5
#define AMBIENT_OCCLUSION 6

/******************************************************************************
 *  Definition for deferred rendering pipeline                                *
 ******************************************************************************/

class SDeferredRenderingPipleline : public SRenderingPipeline {
    
    public:
    
        SDeferredRenderingPipleline(SViewport* _viewport_2D, SViewport3D* _viewport_3D);
        void unload();
    
        virtual void render(double interpolation, SCamera& camera, SSceneGraph& scene_graph);
    
    private:
    
        // Stuff for rendering
        SFramebuffer* gbuffer;
        SAmbientOcclusionPass* ambient_occlusion_pass;
    
        SShader* gbuffer_shader;
        SShader* lit_shader;
        SShader* simple_shader;
    
        SCubeMap* environment_map;
        SUniform* view_pos_u;
    
        // Temp stuff
        SLight* light;
        SSimpleLightGraph* light_graph;
    
    
};

#endif /* SDeferredRenderingPipeline_hpp */
