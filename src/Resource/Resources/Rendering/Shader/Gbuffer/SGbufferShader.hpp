//
//  SGbufferShader.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/1/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SGbufferShader_hpp
#define SGbufferShader_hpp

#include "Resource/Resources/Rendering/Shader/SShader.hpp"

// Forward declarations
class SGbufferShader;

/******************************************************************************
 *  Definition for Gbuffer shader attatchments information                    *
 ******************************************************************************/

enum SGbufferShaderShaders {
	
	SGbufferShaderStatic,
	SGbufferShaderSkinned,
	
	
	SGbufferShaderCount
	
};

static const char* SGbufferShaderExtensions[] = { "static",
												  "skinned" };

/******************************************************************************
 *  Definition for Gbuffer hotload                                            *
 ******************************************************************************/

struct SGbufferShaderHotload : public SGLUpload {
	
	SGbufferShaderHotload(const SPath& _path) : path(_path) {}
	
	virtual void upload();
	virtual void unload();
	
	SGbufferShader* shader;
	SPath path;
	
};

/******************************************************************************
 *  Definition for Gbuffer shader                                             *
 ******************************************************************************/

class SGbufferShader : public SResource {
	
	friend class SGbufferShaderHotload;
	
	public:
	
		bool bind(SGbufferShaderShaders shader);
		void bindUniform(SGbufferShaderShaders shader, void* value, const std::string& name, int type, int count);
		void bindUniform(SGbufferShaderShaders shader, SUniform* uniform);
		void bindTextureLocation(SGbufferShaderShaders shader, const std::string& name, int location);
	
	protected:
	
		virtual bool load(const SPath& path);
		virtual void unload();
		virtual void hotload(const SPath& path);
	
	private:
	
		void compileShader(SPath shader_vert, const SPath& master_vert, const std::string& extension);
	
		std::map<SGbufferShaderShaders, SShader*> shaders;
	
};

#endif /* SGbufferShader_hpp */
