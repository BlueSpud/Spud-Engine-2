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
#include "SUniform.hpp"

/******************************************************************************
 *  Definition for shader                                                     *
 ******************************************************************************/

class SShader : public SResource {
    
    public:
    
        static SResource* allocate();
    
        bool bind();
        void static unbind();
        static SShader* getBoundShader();
    
        static int getUniformLocation(SShader* shader, const std::string& name);
    
        void bindUniform(void* value, const std::string& name, int type, int count);
        void bindUniform(SUniform* uniform);
    
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
