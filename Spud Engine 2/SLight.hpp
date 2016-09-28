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
    
        STransform transform;
        virtual glm::mat4 renderShadowMap(SSceneGraph& scene_graph, double interpolation) = 0;
    
    
};

/******************************************************************************
 *  Definition for directional light                                          *
 ******************************************************************************/

class SDirectionalLight : public SLight {
    
    public:
    
        SDirectionalLight();
        virtual glm::mat4 renderShadowMap(SSceneGraph& scene_graph, double interpolation);
    
    //private:
    
        SViewport3D shadow_viewport;
        SFramebuffer* shadow_buffer;
    
};

#endif /* SLight_hpp */
