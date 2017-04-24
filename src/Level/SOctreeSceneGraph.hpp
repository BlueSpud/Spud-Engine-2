//
//  SOctreeSceneGraph.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/14/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SOctreeSceneGraph_hpp
#define SOctreeSceneGraph_hpp

#include "Octree/SOctree.hpp"
#include "Rendering/Scene Graph/SSceneGraph.hpp"

/******************************************************************************
 *  Definition for octree scene graph                                         *
 ******************************************************************************/

class SOctreeSceneGraph : public SSceneGraph {
	
	friend class SLevel;
	
	public:
	
		virtual ~SOctreeSceneGraph();
	
		virtual void linearizeObjects(std::vector<SObject*>& objects);
		virtual SObject* pickObject(const glm::vec3& origin, const glm::vec3& direction, float length);
		SObject* getObjectWithId(unsigned int id);
	
	protected:
	
		virtual void performAddObject(SObject* object);
		virtual void performRemoveObject(SObject* object);
	
		virtual void collectObjects(SCamera& camera, double interpolation, std::list<SSortedObject>& sorted_objects);

	
	private:
	
		SOctree octree;
	
};

#endif /* SOctreeSceneGraph_hpp */
