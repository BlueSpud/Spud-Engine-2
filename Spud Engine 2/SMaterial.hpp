//
//  SMaterial.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SMaterial_hpp
#define SMaterial_hpp

#include "SShader.hpp"
#include "STexture.hpp"

/******************************************************************************
 *  Definition for material                                                   *
 ******************************************************************************/

class SMaterial : public SResource {
    
    public:
    
        static SResource* allocate();
        void bind();
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
        void uploadTextureIDs(SShader* shader);
    
    private:

        SShader* shader;
    
        std::map<std::string, STexture*>textures;
        std::vector<SUniform*>uniforms;
    
        static SMaterial* currently_bound_material;
    
};

#endif /* SMaterial_hpp */
