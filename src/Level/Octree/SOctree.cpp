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
		
		// If we get here it means this node was an acceptable fit for the object but either the children couldnt fit it or we didnt have another level to have children
		objects.push_back(object);
		return true;
		
	}
	
	return false;
	
}

bool SOctreeNode::remove(SObject* object, glm::vec3* points) {
	
	// First we check if this node has the object
	for (std::list<SObject*>::iterator i = this->objects.begin(); i != this->objects.end(); i++) {
		
		if (*i == object) {
			
			// Object was found, remove it and return true
			objects.erase(i);
			return true;
			
		}
		
	}
	
	// The object was not in this node, so if the object was not contained in this node we can return false
	
	// Compute the mins and maxes of this node
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
	
	// Check through children if we had a chance of containing the object 
	if (contained && has_children) {
		
		for (int i = 0; i < 8; i++) {
		
			// Try to remove it with the children
			if (children[i]->remove(object, points))
				return true;
			
		}
		
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
	for (std::list<SObject*>::iterator i = this->objects.begin(); i != this->objects.end(); i++) {
		
		if ((*i)->shouldBeRendered(frustum))
			culled_objects.push_back(*i);
		
	}
	
	// If we have children, ask them for objects as well
	if (has_children) {
	
		for (int i = 0; i < 8; i++)
			children[i]->collectObjects(frustum, culled_objects);
	
	}
	
}

void SOctreeNode::linearizeObjects(std::vector<SObject*>& objects) {
	
	// First we know that we need to collect all of the objects
	for (std::list<SObject*>::iterator i = this->objects.begin(); i != this->objects.end(); i++)
		objects.push_back(*i);
	
	// If we have children, collect them as well
	if (has_children)
		for (int i = 0; i < 8; i++)
			children[i]->linearizeObjects(objects);
	
	
}

void SOctreeNode::update(SOctree& parent_octree) {
	
	// Update any dynamic objects so that they are always correct in the tree
	// Do the excess first
	for (std::list<SObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
		
		SObject* object = *i;
		 
		if (object->isDynamic()) {
			
			// Erase from the list
			i = objects.erase(i);
			parent_octree.insert(object);
			
		}
		
	}

	
}

void SOctreeNode::purge() {
	
	// Clean out all of the objects
	for (std::list<SObject*>::iterator i = this->objects.begin(); i != this->objects.end(); i++)
		delete *i;
	
	// If we have children, purge them too
	if (has_children) {
		
		for (int i = 0; i < 8; i++) {
			
			children[i]->purge();
			delete children[i];
			
		}
		
		// Save that we no longer have children
		has_children = false;
	
	}
	
}

void SOctreeNode::pickObject(const glm::vec3& origin, const glm::vec3& direction, float length, float& closest, SObject*& object) {
	
	// Make sure that the ray intersects us, we use a simplified version of the bounding box raycase because this is axis-aligned
	glm::vec3 maxes_p = glm::vec3(center + radius);
	glm::vec3 mins_p = glm::vec3(center - radius);
	
	float time_min, time_max;
	
	// X direction
	float time1 = (mins_p.x - origin.x) / direction.x;
	float time2 = (maxes_p.x - origin.x) / direction.x;
	
	time_max = glm::max(time1, time2);
	time_min = glm::min(time1, time2);
	
	// Y direction
	time1 = (mins_p.y - origin.y) / direction.y;
	time2 = (maxes_p.y - origin.y) / direction.y;
	
	time_max = glm::min(time_max, glm::max(time1, time2));
	time_min = glm::max(time_min, glm::min(time1, time2));
	
	// Z direction
	time1 = (mins_p.z - origin.z) / direction.z;
	time2 = (maxes_p.z - origin.z) / direction.z;
	
	time_max = glm::min(time_max, glm::max(time1, time2));
	time_min = glm::max(time_min, glm::min(time1, time2));
	
	// Check if there was an intersection
	if (time_max >= time_min && time_min <= length) {
		
		// First we check all of the objects inside this node
		for (std::list<SObject*>::iterator i = this->objects.begin(); i != this->objects.end(); i++) {
		
			float distance = (*i)->getBoundingBox().rayTrace(origin, direction, length);
			if (distance < closest && distance != -1.0) {
			
				closest = distance;
				object = *i;
				
			}
			
		}
		
		// If we have children, do the raycast on them as well
		if (has_children)
			for (int i = 0; i < 8; i++)
				children[i]->pickObject(origin, direction, length, closest, object);
		
	}
	
}

/******************************************************************************
 *  Implementation for Octree                                                 *
 ******************************************************************************/

SOctree::SOctree() {
	
	// Create the root node
	root_node.center = glm::vec3(0.0);
	root_node.radius = 512.0;
	
	event_listener.listenToEvent(EVENT_START_FRAME, EVENT_MEMBER(SOctree::update));
	
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

void SOctree::remove(SObject* object) {
	
	// First we get the points of the bounding box of the object
	glm::vec3 points[8];
	object->getBoundingBox().getOrientedPoints(points);
	
	// If for some reason the octree cant take the object, we keep it in an overflow array
	bool removed = root_node.remove(object, points);
	
	// If we didnt find it in the octree, it was probably in the excess
	if (!removed) {
		
		for (std::list<SObject*>::iterator i = excess.begin(); i != excess.end(); i++) {
			
			if (*i == object) {
				
				// Remove
				excess.erase(i);
				return;
				
			}
			
		}
		
	}
	
	SLog::verboseLog(SVerbosityLevel::Warning, "Object %lu was not inserted into the octree before it was attempted to be removed", object);
	
}

void SOctree::collectObjects(const SFrustum& frustum, std::vector<SObject*>& culled_objects) {
	
	// Call collect on the root node
	root_node.collectObjects(frustum, culled_objects);
	
	// Do regular frustum culling on any objects not in the octree
	for (std::list<SObject*>::iterator i = excess.begin(); i != excess.end(); i++) {
		
		if ((*i)->shouldBeRendered(frustum))
			culled_objects.push_back((*i));
			
	}
	
}

void SOctree::linearizeObjects(std::vector<SObject*>& objects) {
	
	// Collect the objects that are outside of the scene graph
	for (std::list<SObject*>::iterator i = excess.begin(); i != excess.end(); i++)
		objects.push_back(*i);
	
	// Collect the objects from the root node
	root_node.linearizeObjects(objects);
	
}

SObject* SOctree::pickObject(const glm::vec3& origin, const glm::vec3& direction, float length) {
	
	SObject* object = nullptr;
	float distance = std::numeric_limits<float>::max();
	
	// Recursively go through the tree
	root_node.pickObject(origin, direction, length, distance, object);
	
	// Check excess
	for (std::list<SObject*>::iterator i = excess.begin(); i != excess.end(); i++) {
	
		float distance_new = (*i)->getBoundingBox().rayTrace(origin, direction, length);
		if (distance_new < distance && distance_new != -1.0) {
			
			distance = distance_new;
			object = *i;
			
		}
		
	}
	
	return object;
	
}

void SOctree::update(const SEvent& event) {
	
	// Update any dynamic objects so that they are always correct in the tree
	// Do the excess first
	for (std::list<SObject*>::iterator i = excess.begin(); i != excess.end(); i++) {
	
		SObject* object = *i;
		
		if (object->isDynamic()) {
			
			// Erase from the list
			i = excess.erase(i);
			insert(object);
			
		}
		
	}
	
	// Update the tree
	root_node.update(*this);
	
}

void SOctree::purge() {
	
	// Call purge on the root node
	root_node.purge();
	
}
