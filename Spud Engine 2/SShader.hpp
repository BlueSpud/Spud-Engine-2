//
//  SShader.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SShader_hpp
#define SShader_hpp

#include "SResourceManager.hpp"
#include "SGL.hpp"

/******************************************************************************
 *  Definition for shader                                                     *
 ******************************************************************************/

class SShader : public SResource {
    
    public:
    
        static SResource* allocate();
    
        void bind();
        void static unbind();
        static SShader* getBoundShader();
    
        static int getUniformLocation(SShader* shader, const char* name);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
        virtual void upload();
    
    private:
    
        char* vert_string;
        char* frag_string;
    
        GLuint program_id;

        static SShader* bound_shader;
    
    
};

#endif /* SShader_hpp */
