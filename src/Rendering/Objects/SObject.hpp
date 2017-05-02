//
//  SObject.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SObject_hpp
#define SObject_hpp

#include "Event/SEventSystem.hpp"
#include "Resource/Resources/Rendering/Shader/Gbuffer/SGbufferShader.hpp"
#include "Engine/Utility/SClass.hpp"

#include "Physics/SPhysicsSystem.hpp"
#include "Rendering/Utility/SBoundingBox.hpp"

/******************************************************************************
 *  Definition for generic renderable object                                  *
 ******************************************************************************/

class SObject : public SSerializable {

    SCLASS_ABSTRACT(SObject)

    public:
    
        SObject();
        unsigned int id;
    
        virtual ~SObject() { /* intentionally empty */ }
    
        virtual void render(double interpolation) = 0;
		virtual void render(std::shared_ptr<SGbufferShader> shader, double interpolation);
        virtual void update(const SEvent& event) = 0;
    
        virtual bool shouldBeRendered(const SFrustum& frustum) { return true; }
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph) { /* intentionally empty */ }
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph) { /* intentionally empty */ }
	
		const SBoundingBox& getBoundingBox();
	
		STransform& getTransform();
		void setTransform(const STransform& _transform);
	
		void setTranslation(const glm::vec3& translation);
		void setRotation(const glm::vec3& rotation);
		void setScale(const glm::vec3& scale);
	
		void setTranslationVel(const glm::vec3& translation_vel);
		void setRotationVel(const glm::vec3& rotation_vel);
		void setScaleVel(const glm::vec3& scale_vel);
	
		glm::vec3 getTranslation();
		glm::vec3 getRotation();
		glm::vec3 getScale();
	
		glm::vec3 getTranslationVel();
		glm::vec3 getRotationVel();
		glm::vec3 getScaleVel();
	
		virtual void serialize(SSerializer& serializer);
		virtual void deserialize(SDeserializer& deserializer);
	
		bool isDynamic();
	
    protected:
    
        SEventListener event_listener;
		SBoundingBox bounding_box;
	
	private:
	
		STransform transform;
		bool transform_updated = false;
		void clearTransformUpdated(const SEvent& event);
	
};

#endif /* SObject_hpp */
