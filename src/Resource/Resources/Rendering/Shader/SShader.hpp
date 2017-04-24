//
//  SShader.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SShader_hpp
#define SShader_hpp

#include "Resource/SResourceManager.hpp"
#include "Resource/SGLUpload.hpp"
#include "SUniform.hpp"

// Forward declarations
class SShader;

/******************************************************************************
 *  Definition for shader upload                                              *
 ******************************************************************************/

struct SShaderUpload : public SGLUpload {
    
    virtual void upload();
    virtual void unload();
    
    char* vert_string;
    char* frag_string;
    
    GLuint* program_id;
	
	SShader* shader;
    
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
	friend class SGbufferShaderHotload;
	friend class SShaderUpload;
	
    public:
    
        bool bind();
        void static unbind();
        static SShader* getBoundShader();
    
        void bindUniform(void* value, const std::string& name, int type, int count);
        void bindUniform(SUniform* uniform);
		void bindTextureLocation(const std::string& name, int location);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
        void unload(bool preuploads);
        virtual void hotload(const SPath& path);
    
        bool force_rebind = false;
	
		virtual bool load(const SPath& vert, const SPath& frag);
	
		int getUniformLocation(const std::string& uniform);
	
    private:
	
		void preuploadUniform(void* value, const std::string& name, int type, int count);
	
		std::map<size_t, int> locations;
		std::vector<SUniform> pre_upload_uniforms;
	
        SFile* vert_file;
        SFile* frag_file;
    
        char* vert_string;
        char* frag_string;
    
        GLuint program_id;

        static SShader* bound_shader;
    
        SShaderUpload* upload;
    
};

#endif /* SShader_hpp */
