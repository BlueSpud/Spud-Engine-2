//
//  SScreenSpaceReflections.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 4/8/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SScreenSpaceReflections.hpp"

SScreenSpaceReflectionPass::SScreenSpaceReflectionPass(glm::vec2 main_framebuffer_size) {
	
	size = main_framebuffer_size;
	
	// Get the reflection shader
	reflection_shader = SResourceManager::getResource<SShader>(SPath("Shader/lighting/reflections/deferred_reflections.glsl"));
	reflection_shader->bindTextureLocation("tex_depth", GBUFFER_DEPTH);
	reflection_shader->bindTextureLocation("tex_normal", GBUFFER_NORMAL);
	
	blur_shader = SResourceManager::getResource<SShader>(SPath("Shader/lighting/reflections/blur.glsl"));
	
	// Create the framebuffers
	std::vector<SFramebufferAttachment*> attatchments = { new SFramebufferAttachment(FRAMEBUFFER_COLOR, GL_RGB, GL_RGBA, GL_UNSIGNED_INT, 0) };
	reflection_buffer = new SFramebuffer(attatchments, main_framebuffer_size.x, main_framebuffer_size.y);
	
	attatchments = { new SFramebufferAttachment(FRAMEBUFFER_COLOR, GL_RGB, GL_RGBA, GL_UNSIGNED_INT, 0) };
	blur_buffer = new SFramebuffer(attatchments, main_framebuffer_size.x, main_framebuffer_size.y);
	
	// Get the view pos
	view_pos_u = SUniformManger::instance()->getUniformWithName("view_position");
	
}

void SScreenSpaceReflectionPass::generateReflections(glm::mat4& projection_view_matrix, glm::mat4& inv_matrix, SFramebuffer* rendered_buffer) {

	// We make some assumptions here
	// 1. We are already in the 2D projection that is the size of main_framebuffer_size
	// 2. The Gbuffer texutes are bound and in their correct places according to Gbuffer.h
	
	reflection_buffer->bind();
	
	// Bind shader and texture locations
	reflection_shader->bind();
	
	// The rendered buffer needs to be bound
	reflection_shader->bindTextureLocation("tex_albedo", POST_RPOCESS_START);
	glActiveTexture(GL_TEXTURE0 + POST_RPOCESS_START);
	rendered_buffer->bindTexture(0);
	
	// Bind the matrices
	reflection_shader->bindUniform(&projection_view_matrix, "mat_view_proj", UNIFORM_MAT4, 1);
	reflection_shader->bindUniform(&inv_matrix, "inverse_proj_view", UNIFORM_MAT4, 1);

	reflection_shader->bindUniform(view_pos_u);

	SGL::renderRect(glm::vec2(0.0), size);
	
	// Bind the blur buffer
	blur_buffer->bind();
	blur_shader->bind();
	
	blur_shader->bindUniform(&size, "screen_size", UNIFORM_VEC2, 1);
	
	blur_shader->bindTextureLocation("tex_albedo", POST_RPOCESS_START);
	glActiveTexture(GL_TEXTURE0 + POST_RPOCESS_START);
	reflection_buffer->bindTexture(0);
	
	// Blur
	SGL::renderRect(glm::vec2(0.0), size);
	
}

void SScreenSpaceReflectionPass::bindReflections(int texture) {

	glActiveTexture(GL_TEXTURE0 + texture);
	blur_buffer->bindTexture(0);

}
