//
//  SOctreeLightGraph.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/26/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SOctreeLightGraph_hpp
#define SOctreeLightGraph_hpp

#include "SLightGraph.hpp"
#include "SOctree.hpp"

/******************************************************************************
 *  Definition for octree light graph                                         *
 ******************************************************************************/

class SOctreeLightGraph : public SLightGraph {
	
	public:
	
		virtual ~SOctreeLightGraph();
	
		virtual void cullLights(glm::mat4& projection_view_matrix);
	
		virtual void addLight(SLight* light);
		virtual void removeLight(SLight* light);
	
		virtual void linearizeLights(std::vector<SObject*>& lights);
	
	private:
	
		SOctree octree;
	
};


#endif /* SOctreeLightGraph_hpp */
