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
#include "SGLUpload.hpp"
#include "SUniform.hpp"

// Forward declaration
class SGbufferShader;

/******************************************************************************
 *  Definition for shader upload                                              *
 ******************************************************************************/

struct SShaderUpload : public SGLUpload {
    
    virtual void upload();
    virtual void unload();
    
    char* vert_string;
    char* frag_string;
    
    GLuint* program_id;
    
    
};

/******************************************************************************
 *  Definition for shader unload                                              *
 ******************************************************************************/

struct SShaderUnload : public SGLUpload {
    
    virtual void upload();
    virtual void unload();
    GLuint program_id;
    
};

/******************************************************************************
 *  Definition for shader                                                     *
 ******************************************************************************/

class SShader : public SResource {
	
	friend class SGbufferShader;
	
    public:
    
        bool bind();
        void static unbind();
        static SShader* getBoundShader();
    
        static int getUniformLocation(SShader* shader, const std::string& name);
    
        void bindUniform(void* value, const std::string& name, int type, int count);
        void bindUniform(SUniform* uniform);
		void bindTextureLocation(const std::string& name, int location);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
        virtual void hotload(const SPath& path);
    
        bool force_rebind = false;
	
		virtual bool load(const SPath& vert, const SPath& frag);
	
    private:
    
        SFile* vert_file;
        SFile* frag_file;
    
        char* vert_string;
        char* frag_string;
    
        GLuint program_id;

        static SShader* bound_shader;
    
        SShaderUpload* upload;
    
    
};

#endif /* SShader_hpp */
