//
//  SLight.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SLight_hpp
#define SLight_hpp

#include "Rendering/Pipeline/SFramebuffer.hpp"
#include "Rendering/Objects/SObject.hpp"
#include "Rendering/Utility/SCamera.hpp"
#include "SLightingConstants.h"

// Forward declarations
class SLightGraph;
class SSceneGraph;

/******************************************************************************
 *  Definition for default kind of light                                      *
 ******************************************************************************/

class SLight : public SObject {

    friend class SLightGraph;

    SCLASS_ABSTRACT(SLight)
    
    public:
    
        SLight();
        virtual ~SLight() { /* intentionally empty */ }
	
		// SObject methods
		virtual void render(double interpolation) { /* intentionally empty */ };
		virtual void update(const SEvent& event) { /* intentionally empty */ };
	
		virtual void renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation) = 0;
 
        virtual bool needsShadowUpdate() = 0;
		virtual void getScreenSpaceExtents(const glm::mat4& matrix, const glm::vec3& cam_position, glm::vec3& mins, glm::vec3& maxes);
	
		virtual void serialize(SSerializer& serializer);
		virtual void deserialize(SDeserializer& deserializer);

        glm::vec3 light_color = glm::vec3(1.0);
	
		virtual int getLightType() = 0;
	
		virtual void setRadius(float _radius);
		float getRadius();
	
        glm::mat4 light_matrix;
        glm::ivec2 shadow_map_position;
        bool casts_shadow = false;
    
        bool needs_shadow_update = true;
	
	
	protected:
	
		float radius = 1.0;
	
		static glm::mat4 bias;
		static std::shared_ptr<SGbufferShader> shadow_shader;
	
};

/******************************************************************************
 *  Definition for point light                                                *
 ******************************************************************************/

class SPointLight : public SLight {

	SCLASS(SPointLight)

    public:
    
        SPointLight();
	
        virtual void renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation);
        virtual bool needsShadowUpdate();
    
        virtual bool shouldBeRendered(const SFrustum& frustum);
	
		virtual void getScreenSpaceExtents(const glm::mat4& matrix, const glm::vec3& cam_position, glm::vec3& mins, glm::vec3& maxes);
	
		int getLightType() { return LIGHT_TYPE_POINT; }
	
		virtual void setRadius(float _radius);
	
		virtual void serialize(SSerializer& serializer);
		virtual void deserialize(SDeserializer& deserializer);
	
};

/******************************************************************************
 *  Definition for directional light                                          *
 ******************************************************************************/

class SDirectionalLight : public SLight {

	SCLASS(SDirectionalLight)

    public:
	
		SDirectionalLight();
	
        virtual void renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation);
        virtual bool needsShadowUpdate();
    
        virtual bool shouldBeRendered(const SFrustum& frustum);
	
		int getLightType() { return LIGHT_TYPE_DIRECTIONAL; }
	
		virtual void serialize(SSerializer& serializer);
	
};

/******************************************************************************
 *  Definition for spot light												  *
 ******************************************************************************/

class SSpotLight : public SLight {

    SCLASS(SSpotLight)

	public:
	
		SSpotLight();
	
		virtual void renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation);
		virtual bool needsShadowUpdate();
	
		virtual bool shouldBeRendered(const SFrustum& frustum);
	
		virtual void getScreenSpaceExtents(const glm::mat4& matrix, const glm::vec3& cam_position, glm::vec3& mins, glm::vec3& maxes);
	
		int getLightType() { return LIGHT_TYPE_SPOT; }
	
		virtual void setRadius(float _radius);

		float spotlight_cutoff = M_PI_4;
	
		virtual void serialize(SSerializer& serializer);
		virtual void deserialize(SDeserializer& deserializer);

};

#endif /* SLight_hpp */
