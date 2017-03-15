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
	
	std::vector<SObject*> objects;
	
	bool insert(SObject* object, glm::vec3* points);
	void collectObjects(const SFrustum& frustum, std::vector<SObject*>& culled_objects);
	
	void purge();
	
};

/******************************************************************************
 *  Definition for Octree												      *
 ******************************************************************************/

class SOctree {
	
	public:
	
		SOctree();
		bool insert(SObject* object);
		void collectObjects(const SFrustum& frustum, std::vector<SObject*>& culled_objects);
	
		void purge();
	
	private:
	
		SOctreeNode root_node;
		std::vector<SObject*> excess;
	
};

#endif /* SOctree_hpp */
