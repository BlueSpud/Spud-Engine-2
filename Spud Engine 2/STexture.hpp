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
#include "SGL.hpp"

/******************************************************************************
 *  Definition for texture                                                    *
 ******************************************************************************/

class STexture : public SResource {
    
    public:
    
        static SResource* allocate();
    
        void bind();
        void bind(int texture_number);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
        virtual void upload();
    
    private:
    
        FIBITMAP *bitmap;
        BYTE* image_data;
    
        unsigned int width;
        unsigned int height;
    
        GLuint texture_id;
    
};

#endif /* STexture_hpp */
