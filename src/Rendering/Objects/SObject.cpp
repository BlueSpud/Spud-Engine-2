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
