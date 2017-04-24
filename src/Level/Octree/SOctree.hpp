//
//  SOctree.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/12/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SOctree_hpp
#define SOctree_hpp

#include "Rendering/Objects/SObject.hpp"

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
	
	SOctreeNode* insert(SObject* object, glm::vec3* points);
	void collectObjects(const SFrustum& frustum, std::vector<SObject*>& culled_objects);
	void pickObject(const glm::vec3& origin, const glm::vec3& direction, float length, float& closest, SObject*& object);

	void purge();
	
};

/******************************************************************************
 *  Definition for Octree Object Storage                                      *
 ******************************************************************************/

struct SOctreeObject {

    SOctreeNode* node;
    SObject* object;

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
        SObject* getObjectWithId(unsigned int id);
		SObject* pickObject(const glm::vec3& origin, const glm::vec3& direction, float length);
	
		void update(const SEvent& event);
		void purge();
	
	private:

        SOctreeObject* binarySearch(unsigned int id);

        unsigned int current_id = 0;
        std::vector<SOctreeObject> objects;

		SOctreeNode root_node;
		std::list<SObject*> excess;
		SEventListener event_listener;
	
};

#endif /* SOctree_hpp */
