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

void SOctreeSceneGraph::linearizeObjects(std::vector<SObject*>& objects) {
	
	// Linearize the octree
	octree.linearizeObjects(objects);
	
}

SObject* SOctreeSceneGraph::pickObject(const glm::vec3& origin, const glm::vec3& direction, float length) { return octree.pickObject(origin, direction, length); }

void SOctreeSceneGraph::collectObjects(SCamera& camera, double interpolation, std::list<SSortedObject>& sorted_objects) {
	
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
	std::list<SSortedObject>::iterator j;
	
	for (int i = 0; i < culled_objects.size(); i++) {
	
		//Save the object and sort it based on how close its transform is to the camera to reduce overdraw
		SSortedObject object_s;
		object_s.object = culled_objects[i];
		
		//Calculate z value
		object_s.z_value = (view_matrix * glm::vec4((object_s.object->getTranslation() +
													 object_s.object->getTranslationVel() * (float)interpolation), 1.0)).z;
		
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

void SOctreeSceneGraph::performAddObject(SObject* object) {
	
	// Add it to the octree
	octree.insert(object);
	object->onMoveToSceneGraph(physics_graph);
	
}

void SOctreeSceneGraph::performRemoveObject(SObject* object) {
	
	// Remove from the octree
	octree.remove(object);
	object->onRemoveFromSceneGraph(physics_graph);
	
}

SOctreeSceneGraph::~SOctreeSceneGraph() {
	
	// Purge the tree, deletes all of the objects
	octree.purge();
	
}
