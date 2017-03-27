//
//  SSceneGraph.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/11/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SSceneGraph.hpp"
#include "SRenderSystem.hpp"

SSceneGraph::SSceneGraph() {
	
	// Create a physics graph
	physics_graph = new SPhysicsGraph();
	
}

void SSceneGraph::makeCurrent() {
	
	// This is bad, TEMP
	SRenderSystem::current_scene_graph = this;
	SPhysicsSystem::current_physics_graph = physics_graph;
	
}

SSceneGraph::~SSceneGraph() {

	// If we have a light graph, delete it
	if (light_graph)
		delete light_graph;

}

void SSceneGraph::render(SCamera& camera, double interpolation) {
	
	std::list<SSortedObject>rendered_objects;
	collectObjects(camera, interpolation, rendered_objects);
	
	// Objects are collected, now they are rendered
	for (std::list<SSortedObject>::iterator j = rendered_objects.begin(); j != rendered_objects.end(); j++) {
		
		// Set up the matricies for this
		SGL::clearMatrix(MAT_MODEL);
		
		// Render the object
		(*j).object->render(interpolation);
		
	}
	
}

void SSceneGraph::render(SCamera& camera, SGbufferShader* shader, double interpolation) {

	std::list<SSortedObject>rendered_objects;
	collectObjects(camera, interpolation, rendered_objects);
	
	// Objects are collected, now they are rendered
	for (std::list<SSortedObject>::iterator j = rendered_objects.begin(); j != rendered_objects.end(); j++) {
		
		// Set up the matricies for this
		SGL::clearMatrix(MAT_MODEL);
		
		// Render the object
		(*j).object->render(shader, interpolation);
		
	}

}

/******************************************************************************
 *  Implementation for basic scene graph                                      *
 ******************************************************************************/

void SSimpleSceneGraph::linearizeObjects(std::vector<SObject*>& objects) {
	
	// Convert the list to a vector basically
	for (std::list<SObject*>::iterator i = this->objects.begin(); i != this->objects.end(); i++)
		objects.push_back(*i);
	
}

void SSimpleSceneGraph::collectObjects(SCamera& camera, double interpolation, std::list<SSortedObject>& sorted_objects) {
	
	// Translate everytihng for view space BEFORE so we can perform frustrum and occlusion culling
	SGL::clearMatrix(MAT_VIEW);
	glm::mat4 view_matrix = camera.translateToCameraSpace(interpolation);

	// Generate a frustum
	SFrustum frustum = SFrustum(SGL::getMatrix(MAT_PROJECTION) * view_matrix);
	
	// Make sure that anything we want to render is added to the reder que
	// The array is sorted by z value of an object relative to the camera
	std::list<SSortedObject>::iterator j;
	
	for (std::list<SObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
		
		// Check if we should render this object
		if ((*i)->shouldBeRendered(frustum)) {
			
			//Save the object and sort it based on how close its transform is to the camera to reduce overdraw
			SSortedObject object_s;
			object_s.object = *i;
			
			//Calculate z value
			object_s.z_value = (view_matrix * glm::vec4((object_s.object->transform.translation +
														 object_s.object->transform.translation_velocity * (float)interpolation), 1.0)).z;
			
			// Do an insertion sort of the new object into the array
			bool added = false;
			for (j = sorted_objects.begin(); j != sorted_objects.end(); j++) {
				
				// Check if the object has a z value greater than us
				if ((*j).z_value < object_s.z_value) {
					
					// Insert the object here and break
					sorted_objects.insert(j, object_s);
					added = true;
					break;
					
				}
				
			}
			
			// If it wasnt added, add it back
			if (!added)
				sorted_objects.push_back(object_s);
			
		}
		
	}
	
}

void SSimpleSceneGraph::performAddObject(SObject* object) {

    // Add it to the list
    objects.push_back(object);
    object->onMoveToSceneGraph(physics_graph);

}

void SSimpleSceneGraph::performRemoveObject(SObject* object) {

    // Remove it from the list
    objects.remove(object);
    object->onRemoveFromSceneGraph(physics_graph);

}

SSimpleSceneGraph::~SSimpleSceneGraph() {
	
    // Delete all the objects from memory
    std::vector<SObject*>rendered_objects;

    for (std::list<SObject*>::iterator i = objects.begin(); i != objects.end(); i++)
        delete *i;
    
}
