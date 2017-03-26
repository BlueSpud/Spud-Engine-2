//
//  SLight.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLight.hpp"

glm::mat4 SLight::bias = glm::mat4(0.5, 0.0, 0.0, 0.0,
                                   0.0, 0.5, 0.0, 0.0,
                                   0.0, 0.0, 0.5, 0.0,
                                   0.5, 0.5, 0.5, 1.0);

SGbufferShader* SLight::shadow_shader;

REGISTER_CLASS(SPointLight)
REGISTER_CLASS(SSpotLight)
REGISTER_CLASS(SDirectionalLight)

/******************************************************************************
 *  Implementation for generic light                                          *
 ******************************************************************************/

SLight::SLight() {
    
    // Get the shader for shadow mapping if we dont already have it
	if (!shadow_shader)
        shadow_shader = SResourceManager::getResource<SGbufferShader>(SPath("Shader/lighting/shadow/shadow.glsl"));
	
}

void SLight::getScreenSpaceExtents(const glm::mat4& matrix, const glm::vec3& cam_position, glm::vec3& mins, glm::vec3& maxes) {
	
	// By default the light covers the entire screen
	mins.x = -1.0;
	mins.y = -1.0;
	maxes.x = 1.0;
	maxes.y = 1.0;
	
}

void SLight::serialize(SSerializer& serializer) {

	// Serialize the light color
	serializer.addItem(&light_color);
	
	// Serialize if we cast shadow
	serializer.addItem(&casts_shadow);
	
	// Normal object
	SObject::serialize(serializer);

}

void SLight::deserialize(SDeserializer& deserializer) {

	// Deserialize the color
	deserializer.deserialize(&light_color);

	// Deserialize if we cast shadow
	deserializer.deserialize(&casts_shadow);
	
	// Normal object
	SObject::deserialize(deserializer);

}

void SLight::setRadius(float _radius) { radius = _radius; }
float SLight::getRadius() { return radius; }

/******************************************************************************
 *  Implementation for point light                                            *
 ******************************************************************************/

SPointLight::SPointLight() { /* No initialization */ }

void SPointLight::renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation) {
    
    // Shadows not supported currently!
    
}

bool SPointLight::needsShadowUpdate() {
    
    // For now always return false
    return false;
}

bool SPointLight::shouldBeRendered(const SFrustum& frustum) {
    
    // Project thhe bounding box
    return bounding_box.frustrumCull(frustum);
    
}

void SPointLight::getScreenSpaceExtents(const glm::mat4& matrix, const glm::vec3& cam_position, glm::vec3& mins, glm::vec3& maxes) {
	
	// Figure out if the camera was inside of the light
	if (glm::distance(cam_position, transform.translation) <= radius) {
		
		mins = glm::vec3(-1.0);
		maxes = glm::vec3(1.0);
		return;
		
	}
	
	// We do homoginize it because we are doing projection
	bounding_box.project(matrix, true);
		
	// Output projection result
	mins = bounding_box.projected_mins;
	maxes = bounding_box.projected_maxes;
	
}

void SPointLight::setRadius(float _radius) {
	
	// Save the radius and update the bounds of the bounding box for tiles
	radius = _radius;
	bounding_box.mins = glm::vec3(-radius);
	bounding_box.maxes = glm::vec3(radius);
	
}

void SPointLight::serialize(SSerializer& serializer) {
	
	// Serialize the class
	serializer.startClass<SPointLight>();
	
	// Serialize the radius
	serializer.addItem(&radius);
	
	// Serialize the base
	SLight::serialize(serializer);
	
}


void SPointLight::deserialize(SDeserializer& deserializer) {
	
	// Deserialize the radius
	deserializer.deserialize(&radius);
	setRadius(radius);
	
	// Deserialize base
	SLight::deserialize(deserializer);
	
}


/******************************************************************************
 *  Implementation for directional light                                      *
 ******************************************************************************/

void SDirectionalLight::renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation) {
    
    // Create a camera that we can use to render the scene
    SCamera camera;
    camera.transform = transform;
    camera.transform.translation = glm::vec3(0.0);

    // Make a viewport and scissor so we only clear our part of the shadow atlas
    SViewport3D viewport = SViewport3D(glm::vec2(SHADOW_MAP_ATLAS_TILE_SIZE), shadow_map_position * SHADOW_MAP_ATLAS_TILE_SIZE, 45.0f, glm::vec2(0.1, 1000.0));
    SGL::setUpViewport(viewport);
    glScissor(viewport.screen_pos.x, viewport.screen_pos.y, viewport.screen_size.x, viewport.screen_size.y);

    // Calculate the projection matrix, for directional lights we use orthographic projection
    glm::mat4 projection_matrix = glm::ortho(1.0f, -1.0f, 1.0f, -1.0f, LIGHT_DIRECTIONAL_NEAR, LIGHT_DIRECTIONAL_FAR);
    
    // Generate a crop matrix to maximize shadow map resolution
    // First project the frustum to light space and get a bounding box
    glm::mat4 view_matrix = camera.getCameraMatrix(interpolation);
    glm::mat4 light_projection_view_matrix = projection_matrix * view_matrix;
    glm::vec2 mins, maxes;
    
    for (int i = 0; i < 8; i++) {
        
        glm::vec4 projected_point = light_projection_view_matrix * glm::vec4(close_frustum[i], 1.0);
        
        if (i == 0) {
            
            // The first point will always be the min and max
            mins = (glm::vec2)projected_point;
            maxes = (glm::vec2)projected_point;
            
        } else {
        
            // Check if this point makes up a min or a max on one cordinate
            mins.x = glm::min(mins.x, projected_point.x);
            mins.y = glm::min(mins.y, projected_point.y);
        
            maxes.x = glm::max(maxes.x, projected_point.x);
            maxes.y = glm::max(maxes.y, projected_point.y);
        
        }
        
    }
    
    
    // Now that we know where we need to crop to, we can make a crop matrix
    glm::vec2 S = glm::vec2(2.0 / (maxes.x - mins.x), 2.0 / (maxes.y - mins.y));
    glm::vec2 O = glm::vec2(-0.5 * (maxes.x + mins.x) * S.x, -0.5 * (maxes.y + mins.y) * S.y);
    
    glm::mat4 crop_matrix = glm::mat4(S.x,0,  0,  0,
                                     0,  S.y,0,  0,
                                     0,  0,  1.0,0,
                                     O.x,O.y,0,  1);

    // Make a new projection matrix and a new combined light matrix
    projection_matrix = crop_matrix * projection_matrix;
    SGL::loadMatrix(projection_matrix, MAT_PROJECTION);
    
    light_projection_view_matrix = projection_matrix * view_matrix;
    
    // Clear the framebuffer and render the scene
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    // Render the scene from the camera
    scene_graph.render(camera, shadow_shader, interpolation);

    light_matrix = SLight::bias * light_projection_view_matrix;
    
}

bool SDirectionalLight::needsShadowUpdate() {
   
    // Directional lights are now using perspective shadow mapping so they need to be updated every frame
    return true;
   
}

bool SDirectionalLight::shouldBeRendered(const SFrustum& frustum) {
    
    // Directional lights are always rendered as a full screen quad, never culled
    return true;
    
}

void SDirectionalLight::serialize(SSerializer& serializer) {
	
	// Directional light needs a serialize to add the class, but we do not need a deserialize because there is no class-specific info there
	// Serialize the class
	serializer.startClass<SDirectionalLight>();
	
	// Serialize the base
	SLight::serialize(serializer);
	
}

/******************************************************************************
 *  Implementation for spot light                                             *
 ******************************************************************************/

SSpotLight::SSpotLight() { /* No initialization */ }

void SSpotLight::renderShadowMap(SSceneGraph& scene_graph, glm::vec3* close_frustum, double interpolation) {
	
	SCamera camera;
	camera.transform = transform;
	
	// Make a viewport and scissor so we only clear our part of the shadow atlas
	SViewport3D viewport = SViewport3D(glm::vec2(SHADOW_MAP_ATLAS_TILE_SIZE), shadow_map_position * SHADOW_MAP_ATLAS_TILE_SIZE, 180.0, glm::vec2(0.1, 500.0));
	
	glm::mat4 projection_matrix = SGL::getProjectionMatrix3D(viewport);
	glm::mat4 view_matrix = camera.getCameraMatrix(interpolation);
	
	SGL::setUpViewport(viewport);
	SGL::loadMatrix(projection_matrix, MAT_PROJECTION);
	
	// Scissor
	glScissor(viewport.screen_pos.x, viewport.screen_pos.y, viewport.screen_size.x, viewport.screen_size.y);
	
	// Clear the framebuffer and render the scene
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	// Render the scene from the camera
	scene_graph.render(camera, shadow_shader, interpolation);
	
	light_matrix = SLight::bias * projection_matrix * view_matrix;
	
}

bool SSpotLight::needsShadowUpdate() {
	
	// For now always return false
	return true;
}

bool SSpotLight::shouldBeRendered(const SFrustum& frustum) {
	
	// Project thhe bounding box
	return bounding_box.frustrumCull(frustum);
	
}

void SSpotLight::getScreenSpaceExtents(const glm::mat4& matrix, const glm::vec3& cam_position, glm::vec3& mins, glm::vec3& maxes) {
	
	// Figure out if the camera was inside of the light
	if (glm::distance(cam_position, transform.translation) <= radius) {
	
		mins = glm::vec3(-1.0);
		maxes = glm::vec3(1.0);
		return;
		
	}
	
	// We do homoginize it because we are doing projection
	bounding_box.project(matrix, true);
	
	// Output projection result
	mins = glm::vec3(bounding_box.projected_mins);
	maxes = glm::vec3(bounding_box.projected_maxes);
	
}

void SSpotLight::setRadius(float _radius) {
	
	// Save the radius and update the bounds of the bounding box for tiles
	radius = _radius;
	bounding_box.mins = glm::vec3(-radius);
	bounding_box.maxes = glm::vec3(radius);
	
}

void SSpotLight::serialize(SSerializer& serializer) {
	
	// Serialize the class
	serializer.startClass<SSpotLight>();
	
	// Serialize the radius
	serializer.addItem(&radius);
	
	// Serialize cutoff
	serializer.addItem(&spotlight_cutoff);
	
	// Serialize the base
	SLight::serialize(serializer);
	
}


void SSpotLight::deserialize(SDeserializer& deserializer) {
	
	// Deserialize the radius
	deserializer.deserialize(&radius);
	setRadius(radius);
	
	// Deserialize cutoff
	deserializer.deserialize(&spotlight_cutoff);
	
	// Deserialize base
	SLight::deserialize(deserializer);
	
}

