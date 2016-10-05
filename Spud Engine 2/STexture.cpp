//
//  STexture.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "STexture.hpp"

bool STexture::freeimage_initialized = false;

/******************************************************************************
 *  Registration for supported texture extensions                             *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(png, STexture);
REGISTER_RESOURCE_CLASS(psd, STexture);
REGISTER_RESOURCE_CLASS(jpg, STexture);
REGISTER_RESOURCE_CLASS(jpeg, STexture);
REGISTER_RESOURCE_CLASS(tiff, STexture);

/******************************************************************************
 *  Functions for texture                                                     *
 ******************************************************************************/

SResource* STexture::allocate() { return new STexture(); }

void STexture::bind() {
    
    // Bind the texture to texture 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

}

void STexture::bind(int texture_number) {
    
    // Bind the texture to texture specified
    glActiveTexture(GL_TEXTURE0 + texture_number);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
}

bool STexture::load(const SPath& path) {
    
    // We dont use a file here because free image handles its own loading
    std::string absolute_path = path.getPathAsAbsolutePath();
    if (!path.getIsDirectory()) {
        
        // Initialize free image and prepare storage
        if (!freeimage_initialized){
            
            FreeImage_Initialise();
            freeimage_initialized = true;
            
        }

        FREE_IMAGE_FORMAT format;
        
        // Get the file format and load the image
        format = FreeImage_GetFileType((const char*)absolute_path.c_str(),0);
        bitmap = FreeImage_Load(format, (const char*)absolute_path.c_str());
        
        // Make sure we can actualy read the image
        if (FreeImage_HasPixels(bitmap)) {
            
            // Create an upload and send it off
            upload = new STextureUpload();
            upload->image_data = FreeImage_GetBits(bitmap);
            upload->bitmap = bitmap;
            upload->width = FreeImage_GetWidth(bitmap);
            upload->height =  FreeImage_GetHeight(bitmap);
            
            upload->texture_id = &texture_id;
            upload->uploaded = &uploaded;
            
            SGLUploadSystem::addUpload(upload);
            
            return true;
            
        }
        
        FreeImage_DeInitialise();
        
    }

    return false;
    
}

void STexture::unload() {

    // Delete the texture on the GPU
    if (uploaded)
        glDeleteTextures(1, &texture_id);

}

void STexture::hotload(const SPath& path) {
    
    // Delete the last texture
    if (uploaded) {
        
        // Send a deletion command
        STextureUnload* unload = new STextureUnload();
        unload->texture_id = texture_id;
        SGLUploadSystem::addUpload(unload);
        
    } else {
        
        // Cancel the upload we had already sent
        upload->canceled = true;
        
        // Free the stuff we have
        upload->unload();
        
    }
    
    
    // Load the new texture
    load(path);
    
}

/******************************************************************************
 *  Functions for texture upload                                              *
 ******************************************************************************/

void STextureUpload::upload() {

    // Generate a texture
    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_2D, *texture_id);

    // Figure out if we had alpha
    GLint internal_format = GL_RGB;
    GLint external_format = GL_BGR;
    if (FreeImage_GetChannel(bitmap, FICC_ALPHA) != NULL) {
        
        internal_format = GL_RGBA;
        external_format = GL_BGRA;
        
    }
    
    // Set the parameters of the texture
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, external_format, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    unload();
    
    // Bind the default texture just in case
    glBindTexture(GL_TEXTURE_2D, 0);
    
}

void STextureUpload::unload() {
    
    // Clean up image data
    FreeImage_Unload(bitmap);
    
}

/******************************************************************************
 *  Definition for texture safe detroy                                        *
 ******************************************************************************/

void STextureUnload::upload() { glDeleteTextures(1, &texture_id); }

void STextureUnload::unload() { /* nothing */ }

