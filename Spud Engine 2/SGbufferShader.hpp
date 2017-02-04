//
//  SGbufferShader.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/1/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SGbufferShader_hpp
#define SGbufferShader_hpp

#include "SShader.hpp"

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
 *  Definition for Gbuffer shader                                             *
 ******************************************************************************/

class SGbufferShader : public SResource {
	
	public:
	
		bool bind(SGbufferShaderShaders shader);
		void bindUniform(SGbufferShaderShaders shader, void* value, const std::string& name, int type, int count);
		void bindUniform(SGbufferShaderShaders shader, SUniform* uniform);
		void bindTextureLocation(SGbufferShaderShaders shader, const std::string& name, int location);
	
	protected:
	
		virtual bool load(const SPath& path);
		virtual void unload();
	
	private:
	
		void compileShader(SPath shader_vert, const SPath& master_vert, const std::string& extension);
	
		std::map<SGbufferShaderShaders, SShader*> shaders;
	
};

#endif /* SGbufferShader_hpp */
