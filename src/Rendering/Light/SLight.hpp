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
    
        SLight();
        virtual ~SLight() { /* intentionally empty */ }
    
		virtual void renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation) = 0;
 
        virtual bool needsShadowUpdate() = 0;
    
        virtual bool shouldBeCulled(glm::mat4& projection_view_matrix) = 0;
		virtual void getScreenSpaceExtents(const glm::mat4& matrix, glm::vec2& mins, glm::vec2& maxes);

        STransform transform;
        glm::vec3 light_color = glm::vec3(1.0);
	
		virtual int getLightType() = 0;
	
        glm::mat4 light_matrix;
        glm::ivec2 shadow_map_position;
        bool casts_shadow = false;
    
        static glm::mat4 bias;
        static SShader* shadow_shader;
    
        bool needs_shadow_update = true;
    
    
};

/******************************************************************************
 *  Definition for point light                                                *
 ******************************************************************************/

class SPointLight : public SLight {
    
    public:
    
        SPointLight();
    
        virtual void renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation);
        virtual bool needsShadowUpdate();
    
        virtual bool shouldBeCulled(glm::mat4& projection_view_matrix);
	
		virtual void getScreenSpaceExtents(const glm::mat4& matrix, glm::vec2& mins, glm::vec2& maxes);
	
		int getLightType() { return LIGHT_TYPE_POINT; }
	
    private:
    
        SBoundingBox bounding_box;
    
};

/******************************************************************************
 *  Definition for directional light                                          *
 ******************************************************************************/

class SDirectionalLight : public SLight {
    
    public:
    
        virtual void renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation);
        virtual bool needsShadowUpdate();
    
        virtual bool shouldBeCulled(glm::mat4& projection_view_matrix);
	
		int getLightType() { return LIGHT_TYPE_DIRECTIONAL; }
    
};

/******************************************************************************
 *  Definition for spot light												  *
 ******************************************************************************/

class SSpotLight : public SLight {
	
	public:
	
		SSpotLight();
	
		virtual void renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation);
		virtual bool needsShadowUpdate();
	
		virtual bool shouldBeCulled(glm::mat4& projection_view_matrix);
	
		virtual void getScreenSpaceExtents(const glm::mat4& matrix, glm::vec2& mins, glm::vec2& maxes);
	
		int getLightType() { return LIGHT_TYPE_SPOT; }
	
	private:
	
		SBoundingBox bounding_box;

	
};

#endif /* SLight_hpp */
