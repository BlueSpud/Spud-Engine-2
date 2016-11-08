//
//  SStaticMeshInstance.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/7/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SStaticMeshInstance_hpp
#define SStaticMeshInstance_hpp

#include "SStaticMesh.hpp"
#include "SObject.hpp"

/******************************************************************************
 *  Definition for static mesh instance                                       *
 ******************************************************************************/

class SStaticMeshInstance : public SResource, public SObject {
    
    friend class SStaticMesh;
    
    public:
    
        virtual void render(bool render_material, double interpolation);
        virtual void update();
    
        void setMaterial(SMaterial* new_material, int material_domain);
    
    protected:
    
        // FUNCTIONS SHOULD NEVER BE CALLED
        virtual bool load(const SPath& path);
        virtual void unload();
    
    private:
    
        SStaticMeshInstance(SStaticMesh* _parent_mesh);
        SStaticMesh* parent_mesh;
    
        std::vector<SMaterial*> materials;
    
};

#endif /* SStaticMeshInstance_hpp */