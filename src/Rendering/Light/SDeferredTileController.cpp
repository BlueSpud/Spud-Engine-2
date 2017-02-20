//
//  SDeferredTileController.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/11/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SDeferredTileController.hpp"

/******************************************************************************
 *  Implementation for deferred light grid                                    *
 ******************************************************************************/

SDeferredTileController::SDeferredTileController(glm::vec2 _screen_size) {
	
	screen_size = _screen_size;
	
	// Get the size of the tile grid
	// If we do not perfectly encapsulate, we round up to make sure we cover the last tile
	tile_grid_size.x = ceil(screen_size.x / LIGHT_GRID_TILE_SIZE);
	tile_grid_size.y = ceil(screen_size.y / LIGHT_GRID_TILE_SIZE);
	
	// Create the 2D array for the tile grid
	light_grid = new std::vector<int>*[tile_grid_size.y];
	
	for (int i = 0; i < tile_grid_size.y; i++)
		light_grid[i] = new std::vector<int>[tile_grid_size.x];
	
}

SDeferredTileController::~SDeferredTileController() {
	
	// Delete the tile grid
	for (int i = 0; i < tile_grid_size.y; i++)
		delete [] light_grid[i];
	
	delete light_grid;
	
}

const std::vector<int>& SDeferredTileController::getTileIndicies(int row, int column) const { return light_grid[row][column]; }
glm::ivec2 SDeferredTileController::getGridSize() const { return tile_grid_size; }

void SDeferredTileController::buildLightGrid(const glm::mat4& projection_view_matrix, SLightGraph* light_graph) {
	
	// Get the culled lights, lights that arent on the screen cant contribute
	std::vector<SLight*>& lights = light_graph->getCulledLights();
	
	// Clear the tile values
	for (int i = 0; i < tile_grid_size.y; i++)
		for (int j = 0; j < tile_grid_size.x; j++)
			light_grid[i][j].clear();
	
	// Project the lights and figure out where they go
	for (int i = 0; i < lights.size(); i++) {
		
		glm::vec3 mins, maxes;
		lights[i]->getScreenSpaceExtents(projection_view_matrix, mins, maxes);
		
		// These come as coordinates from [-1, 1], so change them to pixels because that is what the grid is built in
		// Make sure to do 1 - y to compensate for differnt coordinates, we also have to swap min and max on y
		mins = (mins + 1.0f) / 2.0f;
		mins.y = 1.0f - mins.y;
		
		maxes = (maxes + 1.0f) / 2.0f;
		
		float temp = 1.0f - maxes.y;
		maxes.y = mins.y;
		mins.y = temp;
		
		mins = glm::vec3(floor(mins.x * screen_size.x / LIGHT_GRID_TILE_SIZE), ceil(mins.y * screen_size.y / LIGHT_GRID_TILE_SIZE), mins.z);
		maxes = glm::vec3(floor(maxes.x * screen_size.x / LIGHT_GRID_TILE_SIZE), ceil(maxes.y * screen_size.y / LIGHT_GRID_TILE_SIZE), maxes.z);
	
		// Clamp the maxes and mins
		mins.x = glm::clamp(mins.x, 0.0f, tile_grid_size.x - 1.0f);
		mins.y = glm::clamp(mins.y, 0.0f, tile_grid_size.y - 1.0f);
		
		maxes.x = glm::clamp(maxes.x, 0.0f, tile_grid_size.x - 1.0f);
		maxes.y = glm::clamp(maxes.y, 0.0f, tile_grid_size.y - 1.0f);
	
		// Set all of the tiles in the light grid that were intersected to use this light
		for (int r = mins.y; r <= maxes.y; r++)
			for (int c = mins.x; c <= maxes.x; c++)
				light_grid[r][c].push_back(i);
		
	}
	
}

void SDeferredTileController::renderLightGrid(SShader* shader) {
	
	// For each tile
	for (int r = 0; r < tile_grid_size.y; r++) {
		

		for (int c = 0; c < tile_grid_size.x; c++) {
		
			// Tell how many lights we are going to draw
			int light_count = (int)light_grid[r][c].size();
			shader->bindUniform(&light_count, "light_count", UNIFORM_INT, 1);
			
			// Bind the light indicies
			shader->bindUniform(light_grid[r][c].data(), "light_indicies", UNIFORM_INT, light_count);
		
			// Draw a rect
			SGL::renderRect(glm::vec2(c * LIGHT_GRID_TILE_SIZE, r * LIGHT_GRID_TILE_SIZE), glm::vec2(LIGHT_GRID_TILE_SIZE));
		
		
		
		
		}
	
	}
}
