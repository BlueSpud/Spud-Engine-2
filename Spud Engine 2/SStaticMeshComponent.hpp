//
//  SStaticMeshComponent.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SStaticMeshComponent_hpp
#define SStaticMeshComponent_hpp

#include "SComponent.hpp"
#include "SStaticMeshInstance.hpp"

/******************************************************************************
 *  Definition for static mesh component                                      *
 ******************************************************************************/

class SStaticMeshComponent : public SComponent {
    
    public:
    
        virtual void render(bool render_material, double interpolation);
        void setStaticMesh(SStaticMeshInstance* new_mesh_instance);
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph);
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph);
    
    private:
    
        SStaticMeshInstance* mesh_instance;
    
    
};

#endif /* SStaticMeshComponent_hpp */
