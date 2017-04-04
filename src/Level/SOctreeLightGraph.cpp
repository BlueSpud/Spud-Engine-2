//
//  SOctreeLightGraph.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/26/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SOctreeLightGraph.hpp"

/******************************************************************************
 *  Implementation for octree light graph                                     *
 ******************************************************************************/

void SOctreeLightGraph::cullLights(glm::mat4& projection_view_matrix) {
	
	/******************************************************************************
	 *  Cull lights that cant be seen by the camera                               *
	 *  Save light list so that we can use it for the lighting pass later         *
	 ******************************************************************************/
	
	// Generate a frustum
	SFrustum frustum = SFrustum(projection_view_matrix);
	
	// Clear the old list of lights
	culled_lights.clear();
	octree.collectObjects(frustum, (std::vector<SObject*>&)culled_lights);
	
}

SOctreeLightGraph::~SOctreeLightGraph() {
	
	// Purge the octree
	octree.purge();
	
}

void SOctreeLightGraph::addLight(SLight* light) {
	
	// Add the light to the graph
	octree.insert(light);
	
	if (light->casts_shadow) {
		
		// Find a spot for the shadow map
		for (int i = 0; i < SHADOW_MAP_ATLAS_TILE_COUNT; i++) {
			for (int j = 0; j < SHADOW_MAP_ATLAS_TILE_COUNT; j++) {
				
				if (!shadow_map_atlas[i][j]) {
					
					// Save that we used this tile in the atlas and then return
					shadow_map_atlas[j][i] = true;
					light->shadow_map_position = glm::vec2(i, j);
					return;
					
				}
				
			}
		}
		
	}
	
}

void SOctreeLightGraph::removeLight(SLight* light) {
	
	// Free up the spot in the shadow map atlas
	if (light->casts_shadow)
		shadow_map_atlas[light->shadow_map_position.x][light->shadow_map_position.y] = false;
	
	// Remove it from the octree
	octree.remove(light);
	
}

void SOctreeLightGraph::linearizeLights(std::vector<SObject*>& lights) {
	
	// Linearize the octree
	octree.linearizeObjects(lights);
	
}

