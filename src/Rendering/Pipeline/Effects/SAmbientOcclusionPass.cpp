//
//  SAmbientOcclusionPass.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SAmbientOcclusionPass.hpp"

SShader* SAmbientOcclusionPass::ambient_occlusion_shader;
SShader* SAmbientOcclusionPass::blur_shader;
SShader* SAmbientOcclusionPass::blend_shader;

/******************************************************************************
 *  Implementation for the ambient occlusion pass                             *
 ******************************************************************************/

SAmbientOcclusionPass::SAmbientOcclusionPass(glm::vec2 main_framebuffer_size) : viewport(main_framebuffer_size, glm::vec2(0)) {
	
    // Get the shader for an ambient occlusion pass if we dont have it
    if (!ambient_occlusion_shader) {
        
        ambient_occlusion_shader = SResourceManager::getResource<SShader>(SPath("Shader/ambient_occlusion/ambient_occlusion.glsl"));
        blur_shader = SResourceManager::getResource<SShader>(SPath("Shader/ambient_occlusion/blur.glsl"));
        blend_shader = SResourceManager::getResource<SShader>(SPath("Shader/ambient_occlusion/blend.glsl"));
		
	}
    
    // Create the framebuffer for ambient occlusion
    std::vector<SFramebufferAttatchment*> attatchments_occlusion;
    attatchments_occlusion.push_back(new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGB8, GL_RGB, GL_UNSIGNED_INT, 0));
    
    occlusion_framebuffer = new SFramebuffer(attatchments_occlusion, viewport.screen_size.x, viewport.screen_size.y);
    
    // Create the framebuffer for the blur
	std::vector<SFramebufferAttatchment*> attatchments_blur = {new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGB8, GL_RGB, GL_UNSIGNED_INT, 0)};
    blur_framebuffer_w = new SFramebuffer(attatchments_blur, viewport.screen_size.x / 2.0f, viewport.screen_size.y / 2.0f);
	
	attatchments_blur = {new SFramebufferAttatchment(FRAMEBUFFER_COLOR, GL_RGB8, GL_RGB, GL_UNSIGNED_INT, 0)};
	blur_framebuffer_h = new SFramebuffer(attatchments_blur, viewport.screen_size.x / 2.0f, viewport.screen_size.y / 2.0f);
	
}

void SAmbientOcclusionPass::render(SPostProcessPassData& data) {
    
    // Set the viewport up
    SGL::setUpViewport(viewport);
    glm::mat4 projection_matrix_2D = SGL::getProjectionMatrix2D(viewport);
    SGL::loadMatrix(projection_matrix_2D, MAT_PROJECTION);
    
    // Bind the ambient occlusion framebuffer and get it ready
    occlusion_framebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Use the shader
    ambient_occlusion_shader->bind();
    ambient_occlusion_shader->bindTextureLocation("tex_depth", GBUFFER_DEPTH);
	
    // Bind the near and far planes
    ambient_occlusion_shader->bindUniform(&data.viewport_3D->planes, "planes", UNIFORM_VEC2, 1);
	
	// Calculate the projection information
	glm::vec4 projection_info = glm::vec4( -2.0f / (viewport.screen_size.x * data.projection_matrix[0][0][0]),
										   -2.0f / (viewport.screen_size.y * data.projection_matrix[0][1][1]),
										   (1.0f - data.projection_matrix[0][0][2]) / data.projection_matrix[0][0][0],
										   (1.0f + data.projection_matrix[0][1][2]) / data.projection_matrix[0][1][1]);
	
	ambient_occlusion_shader->bindUniform(&projection_info, "projection_info", UNIFORM_VEC4, 1);
	ambient_occlusion_shader->bindUniform(&viewport.screen_size, "screen_size", UNIFORM_VEC2, 1);
	
    SGL::renderRect(glm::vec2(0), viewport.screen_size);
	
    // Do a horizontal blur first
    blur_framebuffer_w->bind();
    glClear(GL_COLOR_BUFFER_BIT);
	
	// Here we change the projection a bit, blurring is done at 1/2 resolution
	viewport.screen_size = viewport.screen_size / 2.0f;
	SGL::setUpViewport(viewport);
	projection_matrix_2D = SGL::getProjectionMatrix2D(viewport);
	SGL::loadMatrix(projection_matrix_2D, MAT_PROJECTION);
	
    // Bind the shader
    blur_shader->bind();

    // We use the same shader for horizontal and vertical so we need to pass in an axis
	glm::vec2 axis = glm::vec2(1.0, 0.0);
	blur_shader->bindUniform(&axis, "axis", UNIFORM_VEC2, 1);
	
	blur_shader->bindTextureLocation("tex_occlusion", data.texture_bind_start);
	glActiveTexture(GL_TEXTURE0 + data.texture_bind_start);
    occlusion_framebuffer->bindTexture(0);

	// Blur is dependant on screen size
    blur_shader->bindUniform(&viewport.screen_size, "screen_size", UNIFORM_VEC2, 1);

    SGL::renderRect(glm::vec2(0.0), viewport.screen_size);

	
	// Do a vertical blur
	blur_framebuffer_h->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Bind axis
	axis = glm::vec2(0.0, 1.0);
	blur_shader->bindUniform(&axis, "axis", UNIFORM_VEC2, 1);
	
	// Now we need to bind the texture or the horizontal pass
	glActiveTexture(GL_TEXTURE0 + data.texture_bind_start);
	blur_framebuffer_w->bindTexture(0);
	
	SGL::renderRect(glm::vec2(0.0), viewport.screen_size);
	
	// Reset the viewport
	viewport.screen_size = viewport.screen_size * 2.0f;
	SGL::setUpViewport(viewport);
	projection_matrix_2D = SGL::getProjectionMatrix2D(viewport);
	SGL::loadMatrix(projection_matrix_2D, MAT_PROJECTION);
	
	
    // Blend the main framebuffer with the new occlusion
    data.framebuffer->bind();
    
    blend_shader->bind();
	
    // Bind the textures
    blend_shader->bindTextureLocation("blurred", data.texture_bind_start);
    blend_shader->bindTextureLocation("final_render", data.texture_bind_start + 1);
	
	// Horizontal blur is done second so it is what we use as the blurred framebuffer
    glActiveTexture(GL_TEXTURE0 + data.texture_bind_start);
    blur_framebuffer_h->bindTexture(0);
    
    glActiveTexture(GL_TEXTURE0 + data.texture_bind_start + 1);
    data.framebuffer->bindTexture(0);
    
    SGL::setUpViewport(*data.viewport_2D);
    projection_matrix_2D = SGL::getProjectionMatrix2D(*data.viewport_2D);
    SGL::loadMatrix(projection_matrix_2D, MAT_PROJECTION);
    
    SGL::renderRect(data.viewport_2D->screen_pos, data.viewport_2D->screen_size);
    
}

SAmbientOcclusionPass::~SAmbientOcclusionPass() {
    
    // Unload the framebuffer and delete it
    occlusion_framebuffer->unload();
    blur_framebuffer_w->unload();
	blur_framebuffer_h->unload();
    
    delete occlusion_framebuffer;
    delete blur_framebuffer_w;
	delete blur_framebuffer_h;
    
}
