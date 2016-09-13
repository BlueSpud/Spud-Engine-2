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

// Forward declarations
class SMaterial;

/******************************************************************************
 *  Definition for material instance                                          *
 ******************************************************************************/

class SMaterialInstance {
    
    friend class SMaterial;
    
    public:
    
        void useMaterial();
        long getMaterialID();
    
        SShader* getShader();
    
    private:
    
        SMaterialInstance();
        SMaterial* parent_mat;
    
        std::vector<STexture*>textures;
    
};

/******************************************************************************
 *  Definition for material                                                   *
 ******************************************************************************/

class SMaterial : public SResource {
    
    friend class SMaterialInstance;
    
    public:
    
        static SResource* allocate();
    
        SMaterialInstance* createMaterialInstance(std::map<std::string, STexture*>& _textures);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
        virtual void upload();
    
        long mat_id;
    
    private:
    
        SShader* shader;
        std::vector<std::string>req_textures;
        int req_textures_count;
    
        std::vector<SMaterialInstance*>instances;
    
        static long next_mat_id;
        static long current_mat_id;
    
    
};

#endif /* SMaterial_hpp */
