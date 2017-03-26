//
//  SLevel.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/24/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SLevel_hpp
#define SLevel_hpp

#include "SResourceManager.hpp"
#include "SOctreeSceneGraph.hpp"
#include "SLightGraph.hpp"

#include "SRenderSystem.hpp"

/******************************************************************************
 *  Declaration for level													  *
 ******************************************************************************/

class SLevel : public SResource {
	
	protected:
	
		virtual bool load(const SPath& path);
		virtual void unload();
	
		SOctreeSceneGraph* scene_graph;
	
};

#endif /* SLevel_hpp */
