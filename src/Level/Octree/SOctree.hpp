//
//  SOctree.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/12/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SOctree_hpp
#define SOctree_hpp

#include "SObject.hpp"

#define OCTREE_MAX_LEVELS 12

// Forward declarations
class SOctree;

/******************************************************************************
 *  Definition for Octree node                                                *
 ******************************************************************************/

struct SOctreeNode {
	
	~SOctreeNode();
	
	glm::vec3 center;
	float radius;
	int level = 0;
	
	SOctreeNode* children[8];
	bool has_children = false;
	
	std::list<SObject*> objects;
	
	bool insert(SObject* object, glm::vec3* points);
	bool remove(SObject* object, glm::vec3* points);
	void collectObjects(const SFrustum& frustum, std::vector<SObject*>& culled_objects);
	void linearizeObjects(std::vector<SObject*>& objects);
	
	void update(SOctree& parent_octree);
	void purge();
	
};

/******************************************************************************
 *  Definition for Octree												      *
 ******************************************************************************/

class SOctree {
	
	public:
	
		SOctree();
		bool insert(SObject* object);
		void remove(SObject* object);
		void collectObjects(const SFrustum& frustum, std::vector<SObject*>& culled_objects);
		void linearizeObjects(std::vector<SObject*>& objects);
	
		void update(const SEvent& event);
		void purge();
	
	private:
	
		SOctreeNode root_node;
		std::list<SObject*> excess;
		SEventListener event_listener;
	
};

#endif /* SOctree_hpp */
