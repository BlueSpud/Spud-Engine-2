//
//  SStaticMeshComponent.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SStaticMeshComponent_hpp
#define SStaticMeshComponent_hpp

#include "Rendering/Objects/Component Based/SComponent.hpp"
#include "Resource/Resources/Rendering/Static Mesh/SStaticMesh.hpp"

/******************************************************************************
 *  Definition for static mesh component                                      *
 ******************************************************************************/

class SStaticMeshComponent : public SComponent {
    
    public:
    
        virtual void render(double interpolation);
        void setStaticMesh(SStaticMesh* new_mesh_instance);
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph);
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph);
    
    private:
    
        SStaticMesh* mesh_instance;
    
    
};

#endif /* SStaticMeshComponent_hpp */
