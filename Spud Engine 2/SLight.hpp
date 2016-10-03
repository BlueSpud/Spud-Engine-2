//
//  SLight.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SLight_hpp
#define SLight_hpp

#include "SFramebuffer.hpp"
#include "SSceneGraph.hpp"
#include "SCamera.hpp"
#include "SLightingConstants.h"
#include "SBoundingBox.hpp"

// Forward declarations
class SLightGraph;

/******************************************************************************
 *  Definition for default kind of light                                      *
 ******************************************************************************/

class SLight {
    
    friend class SLightGraph;
    
    public:
    
        SLight() { /* stub */ };
        virtual ~SLight() { /* stub */ }
    
        virtual void renderShadowMap(SSceneGraph& scene_graph, double interpolation) = 0;
 
        virtual bool needsShadowUpdate() = 0;
    
        virtual bool shouldBeCulled(glm::mat4& projection_view_matrix) = 0;
    
    
        STransform transform;
        glm::mat4 light_matrix;
    
        glm::ivec2 shadow_map_position;
    
        static glm::mat4 bias;
    
    
};

/******************************************************************************
 *  Definition for point light                                                *
 ******************************************************************************/

class SPointLight : public SLight {
    
    public:
    
        SPointLight();
    
        virtual void renderShadowMap(SSceneGraph& scene_graph, double interpolation);
        virtual bool needsShadowUpdate();
    
        virtual bool shouldBeCulled(glm::mat4& projection_view_matrix);
    
    private:
    
        SBoundingBox bounding_box;
    
};

/******************************************************************************
 *  Definition for directional light                                          *
 ******************************************************************************/

class SDirectionalLight : public SLight {
    
    public:
    
        virtual void renderShadowMap(SSceneGraph& scene_graph, double interpolation);
        virtual bool needsShadowUpdate();
    
        virtual bool shouldBeCulled(glm::mat4& projection_view_matrix);
    
};

#endif /* SLight_hpp */
