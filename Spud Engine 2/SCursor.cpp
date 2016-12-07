//
//  SCursor.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SCursor.hpp"

/******************************************************************************
 *  Registration for supported cursor extensions                              *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(cur, SCursor);

/******************************************************************************
 *  Functions for cursor                                                      *
 ******************************************************************************/

SResource* SCursor::allocate() { return new SCursor(); }

bool SCursor::load(const SPath& path) {
    
    // We dont use a file here because free image handles its own loading
    std::string absolute_path = path.getPathAsAbsolutePath();
    if (!path.getIsDirectory()) {
        
        // Initialize free image and prepare storage
        FreeImage_Initialise();
        
        FREE_IMAGE_FORMAT format;
        
        // Get the file format and load the image
        format = FreeImage_GetFileType((const char*)absolute_path.c_str(),0);
        FIBITMAP* bitmap = FreeImage_Load(format, (const char*)absolute_path.c_str());
        
        // Make sure we can actualy read the image AND IT HAS ALPHA
        if (FreeImage_HasPixels(bitmap) && FreeImage_GetChannel(bitmap, FICC_ALPHA) != NULL) {
            
            // Flip the image vertically
            FreeImage_FlipVertical(bitmap);
            
            // Create a GLFW image to store the data
            GLFWimage image;
            image.width = FreeImage_GetWidth(bitmap);
            image.height = FreeImage_GetHeight(bitmap);
            
            image.pixels = FreeImage_GetBits(bitmap);
            
            // Create the cursor
            cursor = glfwCreateCursor(&image, 0, 0);
            
            // Delete the image
            FreeImage_Unload(bitmap);
            
            return true;

        } else return false;
        
    }

    return false;
    
}

void SCursor::bind() { glfwSetCursor(SGL::window, cursor); }

void SCursor::unload() { glfwDestroyCursor(cursor); }

