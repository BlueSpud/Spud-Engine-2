//
//  SStaticMeshComponent.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SStaticMeshComponent.hpp"

/******************************************************************************
 *  Functions for static mesh component                                       *
 ******************************************************************************/

void SStaticMeshComponent::render(bool render_material, double interpolation) {

    // If we have a mesh attatched, we can render it
    if (mesh_instance) {
        
        // Set the transform of the mesh to the transform of the component
        mesh_instance->transform = transform;
        
        // Render the mesh
        mesh_instance->render(render_material, interpolation);
        
    }
    
    // Render the children
    SComponent::render(render_material, interpolation);

}

void SStaticMeshComponent::setStaticMesh(SStaticMeshInstance* new_mesh_instance) { mesh_instance = new_mesh_instance; }

void SStaticMeshComponent::onMoveToSceneGraph(SPhysicsGraph* physics_graph) {

    // Add physics object to the world
    
    SComponent::onMoveToSceneGraph(physics_graph);

}

void SStaticMeshComponent::onRemoveFromSceneGraph(SPhysicsGraph* physics_graph) {

    // Remove physics object to the world
    
    SComponent::onRemoveFromSceneGraph(physics_graph);

}
