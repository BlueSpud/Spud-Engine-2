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

/******************************************************************************
 *  Definition for default kind of light                                      *
 ******************************************************************************/

class SLight {
    
    public:
    
        virtual void renderShadowMap(SSceneGraph& scene_graph, double interpolation) = 0;
        virtual bool needsShadowUpdate() = 0;
    
        virtual ~SLight() { /* stub */ }
    
        STransform transform;
        glm::mat4 light_matrix;
    
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
    
    private:
    
        SViewport3D shadow_viewport;
    
};

/******************************************************************************
 *  Definition for directional light                                          *
 ******************************************************************************/

class SDirectionalLight : public SLight {
    
    public:
    
        SDirectionalLight();
    
        virtual void renderShadowMap(SSceneGraph& scene_graph, double interpolation);
        virtual bool needsShadowUpdate();
    
    //private:
    
        SViewport3D shadow_viewport;
        SFramebuffer* shadow_buffer;
    
};

#endif /* SLight_hpp */
