//
//  SObject.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/18/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SObject.hpp"

/******************************************************************************
 *  Implementation for generic renderable object                              *
 ******************************************************************************/

SObject::SObject() : bounding_box(glm::vec3(0.0), glm::vec3(0.0), &transform) {
    
    // Listen to the tick event for subclasses
    event_listener.listenToEvent(EVENT_TICK, EVENT_MEMBER(SObject::update));
	event_listener.listenToEvent(EVENT_END_FRAME, EVENT_MEMBER(SObject::clearTransformUpdated));
    
}


void SObject::render(std::shared_ptr<SGbufferShader> shader, double interpolation) {
	
	// This is going to be object specific, so by default we just render normally
	render(interpolation);
	
}

const SBoundingBox& SObject::getBoundingBox() { return bounding_box; }

void SObject::serialize(SSerializer& serializer) {

	// Object is abstract and therefore does not need to write out its class hash
	
	// Serialize the transform
	transform.serialize(serializer);

}


void SObject::deserialize(SDeserializer& deserializer) {

	// Deserialize the transform
	transform.deserialize(deserializer);

}

STransform& SObject::getTransform() { return transform; }
void SObject::setTransform(const STransform& _transform) { transform = _transform; }

void SObject::setTranslation(const glm::vec3& translation) { transform.translation = translation; }
void SObject::setRotation(const glm::vec3& rotation) { transform.rotation = rotation; }
void SObject::setScale(const glm::vec3& scale) { transform.scale = scale; }

void SObject::setTranslationVel(const glm::vec3& translation_vel) { transform.translation_velocity = translation_vel; }
void SObject::setRotationVel(const glm::vec3& rotation_vel) { transform.rotation_velocity = rotation_vel; }
void SObject::setScaleVel(const glm::vec3& scale_vel) { transform.scale = scale_vel; }

glm::vec3 SObject::getTranslation() { return transform.translation; }
glm::vec3 SObject::getRotation() { return transform.rotation; }
glm::vec3 SObject::getScale() { return transform.scale; }

glm::vec3 SObject::getTranslationVel() { return transform.translation_velocity; }
glm::vec3 SObject::getRotationVel() { return transform.rotation_velocity; }
glm::vec3 SObject::getScaleVel() { return transform.scale_velocity; }

bool SObject::isDynamic() {
	
	// This returns if the object was moving this frame or not
	bool velocity = glm::length(transform.translation_velocity) || glm::length(transform.rotation_velocity) || glm::length(transform.scale_velocity);
	return velocity || transform_updated;
	
}

void SObject::clearTransformUpdated(const SEvent& event) {

	// Remember that we did not have our transform modified this frame
	transform_updated = false;

}
