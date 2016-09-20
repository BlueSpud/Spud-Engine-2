//
//  SCubeMap.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SCubeMap.hpp"

std::string side_names[] = {"right.png", "left.png", "bottom.png", "top.png", "front.png", "back.png"};

/******************************************************************************
 *  Registration for supported texture extensions                             *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(cube, SCubeMap);

/******************************************************************************
 *  Functions for texture                                                     *
 ******************************************************************************/

SResource* SCubeMap::allocate() { return new SCubeMap(); }

void SCubeMap::bind() {

    // Bind the cube map to texture 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    
}

void SCubeMap::bind(int texture_number) {

    // Bind the cube map to the texture specified
    glActiveTexture(GL_TEXTURE0 + texture_number);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    
}

bool SCubeMap::load(const SPath& path) {

    std::string absolute_path = path.getPathAsAbsolutePath();
    FreeImage_Initialise();
    
    // Loop through each of the names of the files and load them up
    for (int i = 0; i < 6; i++) {
    
        std::string file_path = absolute_path + "/" + side_names[i];
        
        // Save the path
        SPath new_path = SPath(path.getPathAsString() + "/" + side_names[i]);
        paths.push_back(new_path);
    
        if (!path.getIsDirectory()) {
        
            // Initialize free image and prepare storage
            FREE_IMAGE_FORMAT format;
        
            // Get the file format and load the image
            format = FreeImage_GetFileType((const char*)file_path.c_str(),0);
            bitmaps[i] = FreeImage_Load(format, (const char*)file_path.c_str());
        
            // Make sure we can actualy read the image
            if (FreeImage_HasPixels(bitmaps[i])) {
            
                // Save the data, done loading
                image_data[i] = FreeImage_GetBits(bitmaps[i]);
                width = FreeImage_GetWidth(bitmaps[i]);
                height =  FreeImage_GetHeight(bitmaps[i]);
            
            } else return false;
            
            
        } else return false;
        
    }
    
    // Create an upload
    SCubeMapUpload* upload = new SCubeMapUpload();
    upload->bitmaps = bitmaps;
    upload->image_data = image_data;
    
    upload->width = width;
    upload->height = height;
    upload->texture_id = &texture_id;
    upload->uploaded = &uploaded;
    
    SGLUploadSystem::addUpload(upload);
    
    return true;
}


void SCubeMap::unload() {

    // Delete the texture on the GPU
    if (uploaded)
        glDeleteTextures(1, &texture_id);

}

/******************************************************************************
 *  Functions for cube map upload                                             *
 ******************************************************************************/

void SCubeMapUpload::upload() {

    // Generate a texture
    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture_id);
    
    // Parameters for the cube map
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Figure out if we had alpha
    GLint internal_format = GL_RGB;
    GLint external_format = GL_BGR;
    if (FreeImage_GetChannel(bitmaps[0], FICC_ALPHA) != NULL) {
        
        internal_format = GL_RGBA;
        external_format = GL_BGRA;
        
    }
    
    // Add all the textures into the cube map
    for (int i = 0; i < 6; i++) {
    
        // Set the parameters of the texture
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, external_format, GL_UNSIGNED_BYTE, image_data[i]);
        
        // Generate a mipmap
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        
    }
    
    unload();

    *uploaded = true;
    
}

void SCubeMapUpload::unload() {
    
    // Clean up image data still in normal RAM
    for (int i = 0; i < 6; i++)
        FreeImage_Unload(bitmaps[i]);
    
}
