//
//  SMaterial.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SMaterial_hpp
#define SMaterial_hpp

#include "SGbufferShader.hpp"
#include "STexture.hpp"

/******************************************************************************
 *  Definition for material                                                   *
 ******************************************************************************/

class SMaterial : public SResource {
    
    public:
    
        void bind(SGbufferShaderShaders shader_t);
		SGbufferShader* getShader();
	
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
		virtual void hotload(const SPath& path);
	
        void uploadTextureIDs(SGbufferShaderShaders shader_t);
    
    private:

        SGbufferShader* shader;
    
        std::map<std::string, STexture*> textures;
        std::vector<SUniform*> uniforms;
    
        static SMaterial* currently_bound_material;
    
};

#endif /* SMaterial_hpp */
