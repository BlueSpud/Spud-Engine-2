//
//  SOctreeSceneGraph.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/14/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SOctreeSceneGraph_hpp
#define SOctreeSceneGraph_hpp

#include "SOctree.hpp"
#include "SSceneGraph.hpp"

/******************************************************************************
 *  Definition for octree scene graph                                         *
 ******************************************************************************/

class SOctreeSceneGraph : public SSceneGraph {
	
	public:
	
		virtual void addObject(SObject* object);
		virtual void removeObject(SObject* object);
	
		virtual ~SOctreeSceneGraph();
	
	protected:
	
		virtual void collectObjects(SCamera& camera, double interpolation, std::list<SSortedObject>& sorted_objects);

	
	private:
	
		SOctree octree;
	
};

#endif /* SOctreeSceneGraph_hpp */
