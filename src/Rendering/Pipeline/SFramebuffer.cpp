//
//  SFramebuffer.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/21/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "Rendering/Pipeline/SFramebuffer.hpp"

/******************************************************************************
 *  Implementation for framebuffer                                            *
 ******************************************************************************/

SFramebuffer::SFramebuffer(std::vector<SFramebufferAttachment*> attachments, unsigned int _width, unsigned int _height) {
    
    // Generate a framebuffer upload
    upload = new SFramebufferUpload();
    upload->framebuffer_id = &framebuffer_id;
    
    // Create a spot for the texture and then give it over to the upload
    for (int i = 0; i < attachments.size(); i++) {
        
        textures.insert(std::pair<int, GLuint>(attachments[i]->id, -1));
        attachments[i]->texture_id = &textures[attachments[i]->id];
        
        upload->attachments.push_back(attachments[i]);
        
    }
    
    // Save width and height
    upload->width = width = _width;
    upload->height = height = _height;
    
    upload->buffers_to_render = &buffers_to_render;
	upload->attachment_count = &attachment_count;
    
    SGLUploadSystem::addUpload(upload);
    
}

void SFramebuffer::unload() {

    // Create an unload upload
    SFramebufferUnload* unload = new SFramebufferUnload();
    unload->framebuffer_id = framebuffer_id;
    unload->textures = textures;
    unload->buffers_to_render = buffers_to_render;
    
    SGLUploadSystem::addUpload(unload);

}

void SFramebuffer::bind() {
    
    // Bind the framebuffer and tell it to render all the buffers
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    glDrawBuffers(attachment_count, buffers_to_render);
    
}

void SFramebuffer::bindTexture(int id) {

    // See if we have the texture to bind, otherwise just bind the blank one
    GLuint texture_to_bind = 0;
    if (textures.count(id))
        texture_to_bind = textures[id];
    
    glBindTexture(GL_TEXTURE_2D, texture_to_bind);

}

void SFramebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); } 

/******************************************************************************
 *  Implementation for framebuffer upload                                     *
 ******************************************************************************/

void SFramebufferUpload::upload() {

    // Generate the framebuffer
    glGenFramebuffers(1, framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer_id);
    
    // Create the textures
    int attachment = 0;
    
    for (int i = 0; i < attachments.size(); i++) {
        
        SFramebufferAttachment* a = attachments[i];
        
        // Create a texture to the specifications of the attachment
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, a->internal_format, width, height, 0, a->basic_format,a->storage_type, 0);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        // Attach it to the framebuffer
        GLenum attachment_kind = GL_COLOR_ATTACHMENT0 + attachment;
        
        if (a->attachment_kind != FRAMEBUFFER_COLOR)
            attachment_kind = GL_DEPTH_ATTACHMENT;
        else attachment++;
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_kind, GL_TEXTURE_2D, texture, 0);
        
        // Get rid of the attachment in CPU memory and save the texture properly
        *a->texture_id = texture;
        delete a;
        
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        SLog::verboseLog(SVerbosityLevel::Critical, "Framebuffer failed to be created! Check attachment arguments!");
    
    // Make the buffer array
    GLenum* _buffers_to_render;
    
    if (attachment) {
        
        // If there was some color attachments we render them
        _buffers_to_render = new GLenum[attachment];
        for (int i = 0; i < attachment; i++)
            _buffers_to_render[i] = GL_COLOR_ATTACHMENT0 + i;
        
    } else {
        
        // No color attachments, only render the depth buffer
        _buffers_to_render = new GLenum[1];
        _buffers_to_render[0] = GL_NONE;
        
    }
    
    *buffers_to_render = _buffers_to_render;
	*attachment_count = attachment;
    
    // Clear the framebuffer once
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    // Finish up
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void SFramebufferUpload::unload() { /* nothing */ }

/******************************************************************************
 *  Implementation for framebuffer unload                                     *
 ******************************************************************************/

void SFramebufferUnload::upload() {
    
    // Delete the textures
    for (std::map<int, GLuint>::iterator i = textures.begin(); i != textures.end(); i++)
        glDeleteTextures(1, &i->second);
    
    // Delete the framebuffer
    glDeleteFramebuffers(1, &framebuffer_id);
    
    // Delete the buffer array
    delete[] buffers_to_render;
    
}

void SFramebufferUnload::unload() { /* nothing */ }
