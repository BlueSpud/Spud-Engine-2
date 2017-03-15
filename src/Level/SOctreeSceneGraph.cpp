//
//  SOctreeSceneGraph.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/14/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SOctreeSceneGraph.hpp"

/******************************************************************************
 *  Implementation for octree scene graph                                     *
 ******************************************************************************/

std::list<SSortedObject> SOctreeSceneGraph::collectObjects(SCamera& camera, double interpolation) {
	
	// Translate everytihng for view space BEFORE so we can perform frustrum and occlusion culling
	SGL::clearMatrix(MAT_VIEW);
	glm::mat4 view_matrix = camera.translateToCameraSpace(interpolation);
	
	// Generate a frustum
	SFrustum frustum = SFrustum(SGL::getMatrix(MAT_PROJECTION) * view_matrix);
	
	// Gather the objects that are in the frustum
	std::vector<SObject*> culled_objects;
	octree.collectObjects(frustum, culled_objects);
	
	// Make sure that anything we want to render is added to the reder que
	// The array is sorted by z value of an object relative to the camera
	std::list<SSortedObject> sorted_objects;
	std::list<SSortedObject>::iterator j;
	
	for (int i = 0; i < culled_objects.size(); i++) {
	
		//Save the object and sort it based on how close its transform is to the camera to reduce overdraw
		SSortedObject object_s;
		object_s.object = culled_objects[i];
		
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
	
	return sorted_objects;
	
}

void SOctreeSceneGraph::addObject(SObject* object) {
	
	// Add it to the list
	octree.insert(object);
	object->onMoveToSceneGraph(physics_graph);
	
}

void SOctreeSceneGraph::removeObject(SObject* object) {
	
	// Remove it from the list
	object->onRemoveFromSceneGraph(physics_graph);
	
}

SOctreeSceneGraph::~SOctreeSceneGraph() {
	
	// Purge the tree
	octree.purge();
	
}
