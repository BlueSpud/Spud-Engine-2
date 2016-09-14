//
//  STexture.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "STexture.hpp"

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
        FreeImage_Initialise();
        FREE_IMAGE_FORMAT format;
        
        // Get the file format and load the image
        format = FreeImage_GetFileType((const char*)absolute_path.c_str(),0);
        bitmap = FreeImage_Load(format, (const char*)absolute_path.c_str());
        
        // Make sure we can actualy read the image
        if (FreeImage_HasPixels(bitmap)) {
            
            // Save the data, done loading
            image_data = FreeImage_GetBits(bitmap);
            width = FreeImage_GetWidth(bitmap);
            height =  FreeImage_GetHeight(bitmap);
            
            return true;
            
        }
        
    }

    return false;
    
}

void STexture::unload() {

    if (!uploaded) {
        
        // Clean up image data still in normal RAM
        FreeImage_Unload(bitmap);
        
    } else {
        
        // Delete the texture on the GPU
        glDeleteTextures(1, &texture_id);
        
    }

}

void STexture::upload() {

    // Generate a texture
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

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
    
    // Clean up image data
    FreeImage_Unload(bitmap);
    
    // Bind the default texture just in case
    glBindTexture(GL_TEXTURE_2D, 0);

    uploaded = true;
    
}
