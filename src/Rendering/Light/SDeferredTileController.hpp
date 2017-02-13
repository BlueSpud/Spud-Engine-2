//
//  SDeferredTileController.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/11/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SDeferredTileController_hpp
#define SDeferredTileController_hpp

#include "SLightGraph.hpp"

/******************************************************************************
 *  Definition for deferred light grid                                        *
 ******************************************************************************/

class SDeferredTileController {
	
	public:
	
		SDeferredTileController(glm::vec2 _screen_size);
		~SDeferredTileController();
	
		const std::vector<int>& getTileIndicies(int row, int column) const;
		glm::ivec2 getGridSize() const;
	
		void buildLightGrid(const glm::mat4& projection_view_matrix, SLightGraph* light_graph);
	
		void renderLightGrid(SShader* shader);
	
	private:
	
		glm::vec2 screen_size;
	
		glm::ivec2 tile_grid_size;
		std::vector<int>** light_grid;
	
	
};

#endif /* SDeferredTileController_hpp */
