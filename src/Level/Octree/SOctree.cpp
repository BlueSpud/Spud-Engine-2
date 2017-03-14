//
//  SOctree.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/12/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SOctree.hpp"

/******************************************************************************
 *  Implementation for Octree node                                            *
 ******************************************************************************/

SOctreeNode::~SOctreeNode() {
	
	// Delete children if we have them
	if (has_children) {
		
		for (int i = 0; i < 8; i++)
			delete children[i];
		
	}

}

bool SOctreeNode::insert(SObject* object, glm::vec3* points) {
	
	// First we compute the mins and maxes of this node
	glm::vec3 maxes = center + radius;
	glm::vec3 mins = center - radius;
	
	// Check if the object will fit inside us
	bool contained = true;
	for (int i = 0; i < 8; i++) {
		
		if (points[i].x >= maxes.x || points[i].y >= maxes.y || points[i].z >= maxes.z ||
			points[i].x <= mins.x  || points[i].y <= mins.y  || points[i].z <= mins.z) {
			
			// One of the points was outside of the box so we could not contain this object in this node
			contained = false;
			break;
			
		}
		
	}
	
	// If we contained the object, check children
	if (contained) {
		
		if (level < OCTREE_MAX_LEVELS) {
			
			// Check if we have children, otherwise we need to create them
			if (!has_children) {
				
				has_children = true;
				
				float half_radius = radius / 2.0;
			
				children[0] = new SOctreeNode();
				children[0]->center = center + glm::vec3( half_radius,  half_radius,  half_radius);
				children[0]->radius = half_radius;
				children[0]->level = level + 1;
				
				children[1] = new SOctreeNode();
				children[1]->center = center + glm::vec3(-half_radius,  half_radius,  half_radius);
				children[1]->radius = half_radius;
				children[1]->level = level + 1;
				
				children[2] = new SOctreeNode();
				children[2]->center = center + glm::vec3( half_radius,  half_radius, -half_radius);
				children[2]->radius = half_radius;
				children[2]->level = level + 1;
				
				children[3] = new SOctreeNode();
				children[3]->center = center + glm::vec3(-half_radius,  half_radius, -half_radius);
				children[3]->radius = half_radius;
				children[3]->level = level + 1;
				
				children[4] = new SOctreeNode();
				children[4]->center = center + glm::vec3( half_radius, -half_radius,  half_radius);
				children[4]->radius = half_radius;
				children[4]->level = level + 1;
				
				children[5] = new SOctreeNode();
				children[5]->center = center + glm::vec3(-half_radius, -half_radius,  half_radius);
				children[5]->radius = half_radius;
				children[5]->level = level + 1;
				
				children[6] = new SOctreeNode();
				children[6]->center = center + glm::vec3( half_radius, -half_radius, -half_radius);
				children[6]->radius = half_radius;
				children[6]->level = level + 1;
				
				children[7] = new SOctreeNode();
				children[7]->center = center + glm::vec3(-half_radius, -half_radius, -half_radius);
				children[7]->radius = half_radius;
				children[7]->level = level + 1;
				
			}
			
			// Check all the children
			for (int i = 0; i < 8; i++) {
    
				// If the child inserted it, that means that we are good
				if (children[i]->insert(object, points))
					return true;
				
				
			}
			
		}
		
		// If we get here it means this node was an acceptable node for the object but either the children couldnt fit it or we didnt have another level to have children
		objects.push_back(object);
		return true;
		
	}
	
	return false;
	
}

void SOctreeNode::collectObjects(const SFrustum& frustum, std::vector<SObject*>& culled_objects) {
	
	// First we frustum cull the actual node itself, we dont use a bounding box to do this to save on the matrix multiplication
	// Get the points we need to test
	glm::vec3 points[8] = {
		
		center + glm::vec3( radius,  radius,  radius),
		center + glm::vec3(-radius,  radius,  radius),
		center + glm::vec3( radius,  radius, -radius),
		center + glm::vec3(-radius,  radius, -radius),
		center + glm::vec3( radius, -radius,  radius),
		center + glm::vec3(-radius, -radius,  radius),
		center + glm::vec3( radius, -radius, -radius),
		center + glm::vec3(-radius, -radius, -radius)
		
	};
	
	// If all 8 points are outside a plane, we are culled, otherwise we are good to go
	for (int plane = 0; plane < 6; plane++) {
		
		int inside = 8;
		for (int p = 0; p < 8; p++) {
			
			// Do a regular point frustum cull
			if (glm::dot(points[p], glm::vec3(frustum.planes[plane])) + frustum.planes[plane].w < 0.0)
				inside--;
			
		}
		
		// If we had 0 inside, then we are outside
		if (inside == 0)
			return;
		
	}

	// Now we collect the objects inside the box, we frustum cull these now
	for (int i = 0; i < objects.size(); i++) {
		
		if (objects[i]->shouldBeRendered(frustum))
			culled_objects.push_back(objects[i]);
		
	}
	
	// If we have children, ask them for objects as well
	if (has_children) {
	
		for (int i = 0; i < 8; i++)
			children[i]->collectObjects(frustum, culled_objects);
	
	}
	
}

/******************************************************************************
 *  Implementation for Octree                                                 *
 ******************************************************************************/

SOctree::SOctree() {
	
	// Create the root node
	root_node.center = glm::vec3(0.0);
	root_node.radius = 512.0;
	
}

bool SOctree::insert(SObject* object) {
	
	// First we get the points of the bounding box of the object
	glm::vec3 points[8];
	object->getBoundingBox().getOrientedPoints(points);
	
	// If for some reason the octree cant take the object, we keep it in an overflow array
	bool inserted = root_node.insert(object, points);
	
	if (!inserted)
		excess.push_back(object);
	
	return inserted;
	
}

void SOctree::collectObjects(const SFrustum& frustum, std::vector<SObject*>& culled_objects) {
	
	// Call collect on the root node
	root_node.collectObjects(frustum, culled_objects);
	
	// Do regular frustum culling on any objects not in the octree
	for (int i = 0; i < excess.size(); i++) {
		
		if (excess[i]->shouldBeRendered(frustum))
			culled_objects.push_back(excess[i]);
			
	}
	
}
