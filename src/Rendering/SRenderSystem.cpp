//
//  SRenderSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/12/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SRenderSystem.hpp"

SRenderingPipeline* SRenderSystem::rendering_pipeline;
SRenderingPipeline* SRenderSystem::cubemap_pipeline;
SSceneGraph* SRenderSystem::current_scene_graph;

std::shared_ptr<SShader> SRenderSystem::brdf_shader;
SFramebuffer* SRenderSystem::brdf_buffer;

/******************************************************************************
 *  Implementation for the render system                                      *
 ******************************************************************************/

void SRenderSystem::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SRenderSystem startup");

	// Generate the BRDF buffer
	brdf_shader = SResourceManager::getResource<SShader>(SPath("Shader/lighting/brdf/brdf_integral.glsl"));
	
	std::vector<SFramebufferAttachment*> attatchments = { new SFramebufferAttachment(FRAMEBUFFER_COLOR, GL_RG32F, GL_RG, GL_FLOAT, 0) };
	brdf_buffer = new SFramebuffer(attatchments, 1024.0, 1024.0);
	
}

void SRenderSystem::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SRenderSystem shutdown");
    
    // Delete everything we have because we become responsible for something once its given to us
    if (rendering_pipeline)
        delete rendering_pipeline;
    
    if (current_scene_graph)
        delete current_scene_graph;
	
	brdf_buffer->unload();
	delete brdf_buffer;

}

void SRenderSystem::render(double interpolation) {
    
    // If we have a scene graph and a rendering pipline we can render the scene
    if(rendering_pipeline && current_scene_graph && current_scene_graph->light_graph) {
        
        // Render and then finalize
        rendering_pipeline->render(*current_scene_graph, *SCamera::current_camera, interpolation);
        rendering_pipeline->finalizeRender(nullptr);
        
    }
    
}

void SRenderSystem::generateBRDF() {
	
	SLog::verboseLog(SVerbosityLevel::Debug, "Generating BRDF, this may take a while");
	
	// Set up a temporary viewport and a renderer
	SViewport viewport = SViewport(glm::vec2(1024.0), glm::vec2(0.0));
	SGL::setUpViewport(viewport);
	SGL::loadMatrix(SGL::getProjectionMatrix2D(viewport), MAT_PROJECTION);
	SGL::clearMatrix(MAT_VIEW);
	SGL::clearMatrix(MAT_MODEL);
	
	// Draw
	brdf_buffer->bind();
	brdf_shader->bind();
	SGL::renderRect(glm::vec2(0.0), glm::vec2(1024.0));
	
}

void SRenderSystem::bindBRDF(int texture) {
	
	// Bind the BRDF teacher
	glActiveTexture(GL_TEXTURE0 + texture);
	brdf_buffer->bindTexture(0);

}

SCubeMap* SRenderSystem::generateCubeMap(const glm::vec3& position) {
	
	// Create a new cubemap
	return new SCubeMap(position, cubemap_pipeline, *current_scene_graph, 1024.0);
	
}
