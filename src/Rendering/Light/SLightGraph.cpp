//
//  SLightGraph.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/29/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLightGraph.hpp"

std::shared_ptr<SShader> SLightGraph::shadow_blur_shader_h = nullptr;
std::shared_ptr<SShader> SLightGraph::shadow_blur_shader_v;
SFramebuffer* SLightGraph::intermediate_blur_buffer;

/******************************************************************************
 *  Implementation for default light graph                                    *
 ******************************************************************************/

SLightGraph::SLightGraph() {
	
	// If we havent performed the static init, do it TEMP
	if (!shadow_blur_shader_h) {
		
		shadow_blur_shader_h = SResourceManager::getResource<SShader>(SPath("Shader/lighting/shadow/blur_h.glsl"));
		shadow_blur_shader_v = SResourceManager::getResource<SShader>(SPath("Shader/lighting/shadow/blur_v.glsl"));
		
		std::vector<SFramebufferAttatchment*> attatchments = {new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RG32F, GL_RG, GL_FLOAT, 0)};
		intermediate_blur_buffer = new SFramebuffer(attatchments, SHADOW_MAP_ATLAS_TILE_SIZE / 2.0, SHADOW_MAP_ATLAS_TILE_SIZE / 2.0);
		
	}
	
    // Create a massive framebuffer for a bunch of shadow maps
    std::vector<SFramebufferAttatchment*> attatchments = {
		
			new SFramebufferAttatchment(FRAMEBUFFER_DEPTH, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0),
			new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RG32F, GL_RG, GL_FLOAT, 1)
	};
	
    shadow_map_buffer = new SFramebuffer(attatchments, SHADOW_MAP_ATLAS_SIZE, SHADOW_MAP_ATLAS_SIZE);
    
    // Create the 2D array for the shadow maps
    shadow_map_atlas = new bool*[SHADOW_MAP_ATLAS_TILE_COUNT];
    for (int i = 0; i < SHADOW_MAP_ATLAS_TILE_COUNT; i++)
        shadow_map_atlas[i] = new bool[SHADOW_MAP_ATLAS_TILE_COUNT];
    
}

SLightGraph::~SLightGraph() {

    // Delete the shadow atlas
    for (int i = 0; i < SHADOW_MAP_ATLAS_TILE_COUNT; i++)
        delete shadow_map_atlas[i];
    delete shadow_map_atlas;
    
    // Unload and delete the shadow map framebuffer
    shadow_map_buffer->unload();
    delete shadow_map_buffer;
}

void SLightGraph::blurLightTile(glm::ivec2& tile) {
	
	// Convert ivec2 to vec2
	glm::vec2 tile_c = glm::vec2(tile);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
	
	// Blur the shadow map
	intermediate_blur_buffer->bind();
	
	// Bind the horizontal blur shader and perform a blur
	shadow_blur_shader_h->bind();
	shadow_blur_shader_h->bindUniform(&tile_c, "shadow_tile", UNIFORM_VEC2, 1);
	
	// Change the viewport to fit the framebuffer
	SViewport viewport = SViewport(glm::vec2(SHADOW_MAP_ATLAS_TILE_SIZE / 2.0), glm::vec2(0.0));
	SGL::loadMatrix(SGL::getProjectionMatrix2D(viewport), MAT_PROJECTION);
	SGL::setUpViewport(viewport);
	
	glActiveTexture(GL_TEXTURE0);
	shadow_map_buffer->bindTexture(1);
	
	// Draw, pulling the shadow map from the atlas to the framebuffer and blurring it in the process
	SGL::renderRect(glm::vec2(0.0), glm::vec2(SHADOW_MAP_ATLAS_TILE_SIZE / 2.0));
	
	// Bind the vertical blur shader
	shadow_blur_shader_v->bind();
	
	// Upload the size of the blur, this is half of the resolution of a single shadow map tile
	int shadow_map_tile_size = SHADOW_MAP_ATLAS_TILE_SIZE / 2.0;
	shadow_blur_shader_v->bindUniform(&shadow_map_tile_size, "tile_size", UNIFORM_INT, 1);
	
	// Rebind the shadow map atlas so the new, fully blurred shadow map can be put back
	shadow_map_buffer->bind();
	
	// Bind the already blurred texture
	intermediate_blur_buffer->bindTexture(0);
	
	// Change the viewport to be where the tile is going to be
	viewport = SViewport(glm::vec2(SHADOW_MAP_ATLAS_TILE_SIZE), tile_c * (float)SHADOW_MAP_ATLAS_TILE_SIZE);
	SGL::setUpViewport(viewport);
	SGL::loadMatrix(SGL::getProjectionMatrix2D(viewport), MAT_PROJECTION);
	
	SGL::renderRect(glm::vec2(0.0, 0.0), glm::vec2(SHADOW_MAP_ATLAS_TILE_SIZE));
	
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
}

void SLightGraph::updateShadows(SSceneGraph& scene_graph, glm::mat4& projection_matrix, glm::mat4& view_matrix, double interpolation) {
	
	bool bound_shadow_map_buffer = false;
	
	// Generate the cascades for shadow mapping
	glm::vec3* close_frustum = SCamera::getFrustumWithPlanes(projection_matrix, view_matrix, 0.1, 10.0);
	
	// Take the list of lights that need to be updated and render their shadow maps
	for (int i= 0; i < culled_lights.size(); i++)
		if (culled_lights[i]->needsShadowUpdate()) {
			
			// Check if we havent bound the shadow map buffer yet, if we have no lights we dont need to bind it
			if(!bound_shadow_map_buffer) {
				
				bound_shadow_map_buffer = true;
				shadow_map_buffer->bind();
				
				// Enable scissor testing
				glEnable(GL_SCISSOR_TEST);
				
			}
			
			// Render the shadow map and then blur it
			culled_lights[i]->renderShadowMap(scene_graph, close_frustum, interpolation);
			blurLightTile(culled_lights[i]->shadow_map_position);
			
		}
	
	if (bound_shadow_map_buffer) {
		
		// Disable scissor testing if we enabled it
		glDisable(GL_SCISSOR_TEST);
		
	}
	
	// Delete the cascandes
	delete close_frustum;
	
}

void SLightGraph::uploadCulledLightData(std::shared_ptr<SShader> shader, double interpolation) {
	
	// Datas
	std::vector<int> light_types;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> light_infos;
	std::vector<glm::vec4> params;
	std::vector<glm::vec4> spot_data;
	
	// Shadow info
	std::vector<int> shadows;
	std::vector<glm::mat4> shadow_matricies;
	std::vector<glm::vec2> shadow_coordinates;
	
	for (int i = 0; i < culled_lights.size(); i++) {
		
		SLight* light = culled_lights[i];
		
		glm::vec2 light_info = glm::vec2(1.0, -1.0);
		
		// Get the type of the current light and positon
		int type = light->getLightType();
		light_types.push_back(type);
		
		glm::vec3 position = glm::vec3(light->transform.translation + light->transform.translation_velocity * (float)interpolation);
		
		// Check if the light was a spotlight
		if (type == LIGHT_TYPE_SPOT) {
			
			// Get the spot cutoff and direction of the light
			glm::vec4 spot_data_light = glm::vec4(light->transform.getForwardVector(interpolation), cos(((SSpotLight*)light)->spotlight_cutoff));
			
			// Save the index of the spot
			light_info.y = spot_data.size();
			spot_data.push_back(spot_data_light);
			
		}
		
		
		positions.push_back(position);
		light_infos.push_back(light_info);
		
		// Get the color
		params.push_back(glm::vec4(light->light_color, light->getRadius()));
		
		// Get shadow information
		int shadow = light->casts_shadow;
		
		if (shadow) {
			
			// Get matrix and shadow map positon
			shadow_matricies.push_back(light->light_matrix);
			shadow_coordinates.push_back(light->shadow_map_position);
			
		}
		
		shadows.push_back(shadow);
		
	}
	
	// Upload all of the data to the shader
	int light_count = (int)culled_lights.size();
	shader->bindUniform(&light_count, "light_count", UNIFORM_INT, 1);
	
	// Upload the regular light info
	shader->bindUniform(positions.data(), "light_positions", UNIFORM_VEC3, light_count);
	shader->bindUniform(light_infos.data(), "light_infos", UNIFORM_VEC2, light_count);
	shader->bindUniform(params.data(), "light_params", UNIFORM_VEC4, light_count);
	shader->bindUniform(shadows.data(), "lights_shadow", UNIFORM_INT, light_count);
	shader->bindUniform(spot_data.data(), "spot_data", UNIFORM_VEC4, (int)spot_data.size());
	shader->bindUniform(light_types.data(), "light_types", UNIFORM_INT, light_count);
	
	// Upload the shadow info
	shader->bindUniform(shadow_matricies.data(), "light_matrices", UNIFORM_MAT4, (int)shadow_matricies.size());
	shader->bindUniform(shadow_coordinates.data(), "shadow_map_coordinates", UNIFORM_VEC2, (int)shadow_coordinates.size());
	
}

std::vector<SLight*>& SLightGraph::getCulledLights() { return culled_lights; }

/******************************************************************************
 *  Implementation for simple light graph                                     *
 ******************************************************************************/

void SSimpleLightGraph::cullLights(glm::mat4& projection_view_matrix) {
	
    /******************************************************************************
     *  Cull lights that cant be seen by the camera                               *
     *  Save light list so that we can use it for the lighting pass later         *
     ******************************************************************************/
	
	// Generate a frustum
	SFrustum frustum = SFrustum(projection_view_matrix);
    
    // Clear the old list of lights
    culled_lights.clear();
    
    for (std::list<SLight*>::iterator i = lights.begin(); i != lights.end(); i++) {
        
        // Check if the light needs a shadow update and if we can see it in the frustrum
        if ((*i)->shouldBeRendered(frustum))
            culled_lights.push_back(*i);
        
    }
	
}

SSimpleLightGraph::~SSimpleLightGraph() {
    
    // Delete all the lights in the graph
    for (std::list<SLight*>::iterator i = lights.begin(); i != lights.end(); i++)
        delete *i;

}

void SSimpleLightGraph::addLight(SLight* light) {
	
	// Add the light to the graph
	lights.push_back(light);
	
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

void SSimpleLightGraph::removeLight(SLight* light) {
	
	// Free up the spot in the shadow map atlas
	if (light->casts_shadow)
		shadow_map_atlas[light->shadow_map_position.x][light->shadow_map_position.y] = false;
	
	// Remove the light from the graph
	lights.remove(light);
	
}

void SSimpleLightGraph::linearizeLights(std::vector<SObject*>& lights) {
	
	// Turn the list into a vector
	 for (std::list<SLight*>::iterator i = this->lights.begin(); i != this->lights.end(); i++)
		 lights.push_back(*i);
	
}

