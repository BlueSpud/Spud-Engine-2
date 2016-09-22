//
//  SFramebuffer.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/21/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SFramebuffer_hpp
#define SFramebuffer_hpp

#include <vector>

#include "STexture.hpp"

/******************************************************************************
 *  Definition for framebuffer attatchment                                    *
 ******************************************************************************/

#define FRAMEBUFFER_COLOR 0
#define FRAMEBUFFER_DEPTH 1

struct SFramebufferAttatchment {
    
    int attatchment_kind, basic_format;
    int storage_type, id;
    int internal_format;
    
    /***************************************************************************************************
     *                                                                                                 *
     * attatchment_kind is how its attached to the framebuffer, FRAMEBUFFER_COLOR or FRAMEBUFFER_DEPTH *
     * basic_format is channels, GL_RGB, GL_RGBA, etc.                                                 *
     * storage_type is data type of storage, eg. GL_FLOAT, GL_UNSIGNED_INT                             *
     * id is an indentifier for it to be stored as in the framebuffer                                  *
     * internal_format is actual format like GL_RGBA32F, should match storage_type                     *
     *                                                                                                 *
     ***************************************************************************************************/
    
    SFramebufferAttatchment(int _attatchment_kind, int _internal_format, int _basic_format, int _storage_type, int _id) {
        
        attatchment_kind = _attatchment_kind;
        internal_format = _internal_format;
        basic_format = _basic_format;
        storage_type = _storage_type;
        id = _id;
        
    }
    
};

/******************************************************************************
 *  Definition for framebuffer upload                                         *
 ******************************************************************************/

struct SFramebufferUpload : public SGLUpload {

    virtual void upload();
    virtual void unload();
    
    unsigned int width;
    unsigned int height;
    
    GLuint* framebuffer_id;
    std::map<GLuint*, SFramebufferAttatchment*> attatchments;

};

/******************************************************************************
 *  Definition for framebuffer upload                                         *
 ******************************************************************************/

struct SFramebufferUnload : public SGLUpload {
    
    virtual void upload();
    virtual void unload();
    
    GLuint framebuffer_id;
    std::map<int, GLuint>* textures;
    
};

/******************************************************************************
 *  Definition for framebuffer                                                *
 ******************************************************************************/

class SFramebuffer {
    
    public:
    
        // Default constructor should never be called
        SFramebuffer(std::vector<SFramebufferAttatchment*> attatchments, unsigned int _width, unsigned int _height);
        void unload();
    
        void bind();
        void bindTexture(int id);
    
    private:
    
        unsigned int width;
        unsigned int height;
    
        GLuint framebuffer_id;
        std::map<int, GLuint> textures;
    
        SFramebufferUpload* upload;
    
};

#endif /* SFramebuffer_hpp */
