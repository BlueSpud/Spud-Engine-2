//
//  STexture.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef STexture_hpp
#define STexture_hpp

#include <FreeImage.h>

#include "SResourceManager.hpp"
#include "SGLUpload.hpp"

/******************************************************************************
 *  Definition for upload                                                     *
 ******************************************************************************/

struct STextureUpload : public SGLUpload {
    
    virtual void upload();
    virtual void unload();
    
    FIBITMAP* bitmap;
    BYTE* image_data;
    
    unsigned int width;
    unsigned int height;
    
    GLuint* texture_id;
    
};

/******************************************************************************
 *  Definition for safe detroy                                                *
 ******************************************************************************/

struct STextureUnload : public SGLUpload {
  
    virtual void upload();
    virtual void unload();
    
    GLuint texture_id;
    
};

/******************************************************************************
 *  Definition for texture                                                    *
 ******************************************************************************/

class STexture : public SResource {
    
    public:
    
        glm::ivec2 size;
    
        static SResource* allocate();
    
        virtual void bind();
        virtual void bind(int texture_number);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
        virtual void hotload(const SPath& path);
    
        static bool freeimage_initialized;
    
    private:
    
        FIBITMAP* bitmap;
        BYTE* image_data;
    
        unsigned int width;
        unsigned int height;
    
        GLuint texture_id;
    
        STextureUpload* upload;
    
};

#endif /* STexture_hpp */
